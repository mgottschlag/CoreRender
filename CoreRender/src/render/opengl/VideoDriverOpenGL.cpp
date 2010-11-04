/*
Copyright (C) 2010, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "VideoDriverOpenGL.hpp"
#include "Texture2DOpenGL.hpp"
#include "IndexBufferOpenGL.hpp"
#include "VertexBufferOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "FrameBufferOpenGL.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/render/VertexLayout.hpp"
#include "CoreRender/render/Material.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	VideoDriverOpenGL::VideoDriverOpenGL(core::Log::Ptr log)
		: log(log), currentfb(0), currentshader(0), currentvertices(0),
		currentindices(0), currentblendmode(BlendMode::Replace),
		currentdepthwrite(true), currentdepthtest(DepthTest::Less),
		currentdrawbuffers(1)
	{
	}
	VideoDriverOpenGL::~VideoDriverOpenGL()
	{
	}

	bool VideoDriverOpenGL::init()
	{
		// Init extensions
		if (glewInit() != GLEW_OK)
		{
			log->error("Could not initialize GLEW.");
			return false;
		}
		if (!GLEW_VERSION_2_0)
		{
			log->error("OpenGL 2.0 not available.");
			return false;
		}
		if (!GLEW_EXT_framebuffer_object)
		{
			log->error("GLEW_EXT_framebuffer_object not available.");
			return false;
		}
		if (!caps.init())
		{
			log->error("Could not initialize capabilities.");
			return false;
		}
		// Init static OpenGL states
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		return true;
	}
	bool VideoDriverOpenGL::shutdown()
	{
		return true;
	}

	Texture2D::Ptr VideoDriverOpenGL::createTexture2D(UploadManager &uploadmgr,
	                                                  res::ResourceManager *rmgr,
	                                                  const std::string &name)
	{
		return new Texture2DOpenGL(uploadmgr, rmgr, name);
	}
	IndexBuffer::Ptr VideoDriverOpenGL::createIndexBuffer(UploadManager &uploadmgr,
	                                           res::ResourceManager *rmgr,
	                                           const std::string &name)
	{
		return new IndexBufferOpenGL(uploadmgr, rmgr, name);
	}
	VertexBuffer::Ptr VideoDriverOpenGL::createVertexBuffer(UploadManager &uploadmgr,
	                                             res::ResourceManager *rmgr,
	                                             const std::string &name)
	{
		return new VertexBufferOpenGL(uploadmgr, rmgr, name);
	}
	Shader::Ptr VideoDriverOpenGL::createShader(UploadManager &uploadmgr,
	                                            res::ResourceManager *rmgr,
	                                            const std::string &name)
	{
		return new ShaderOpenGL(uploadmgr, rmgr, name);
	}
	FrameBuffer::Ptr VideoDriverOpenGL::createFrameBuffer(UploadManager &uploadmgr,
	                                                      res::ResourceManager *rmgr,
	                                                      const std::string &name)
	{
		return new FrameBufferOpenGL(uploadmgr, rmgr, name);
	}

	void VideoDriverOpenGL::setRenderTarget(const RenderTargetInfo *target)
	{
		// Bind frame buffer object
		FrameBuffer::Configuration *newfb = 0;
		if (target)
			newfb = target->framebuffer;
		if (newfb != currentfb)
		{
			// Generate mipmaps
			if (currentfb)
			{
				generateMipmaps(currentfb);
			}
			// Bind new framebuffer object
			currentfb = newfb;
			if (newfb)
			{
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, newfb->handle);
			}
			else
			{
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				forceDrawBuffers(0x1);
			}
		}
		if (!target || !newfb)
			return;
		// Unbind unused color buffers
		for (unsigned int i = target->colorbuffercount;
			i < currentfb->colorbuffers.size(); i++)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			                          GL_COLOR_ATTACHMENT0_EXT + i,
			                          GL_TEXTURE_2D,
			                          0,
			                          0);
		}
		// Enlarge color buffer array if necessary
		for (unsigned int i = currentfb->colorbuffers.size();
			i < target->colorbuffercount; i++)
		{
			currentfb->colorbuffers.push_back(0);
		}
		// Bind color buffers
		for (unsigned int i = 0; i < target->colorbuffercount; i++)
		{
			if (target->colorbuffers[i] == currentfb->colorbuffers[i])
				continue;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			                          GL_COLOR_ATTACHMENT0_EXT + i,
			                          GL_TEXTURE_2D,
			                          target->colorbuffers[i],
			                          0);
			currentfb->colorbuffers[i] = target->colorbuffers[i];
		}
		// Bind depth buffer
		if (target->depthbuffer != currentfb->depthbuffer)
		{
			if (!target->depthbuffer)
			{
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
				                             GL_DEPTH_ATTACHMENT_EXT,
				                             GL_RENDERBUFFER_EXT,
				                             currentfb->defaultdepthbuffer);
			}
			else
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				                          GL_DEPTH_ATTACHMENT_EXT,
				                          GL_TEXTURE_2D,
				                          target->depthbuffer,
				                          0);
			}
			currentfb->depthbuffer = target->depthbuffer;
		}
		// Change draw buffers
		unsigned int drawbuffers = 0xFFFFFFFF >> (32 - target->colorbuffercount);
		forceDrawBuffers(drawbuffers);
		// Check fbo state
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			log->error("Invalid framebuffer (%d).", status);
		}
	}
	void VideoDriverOpenGL::setViewport(unsigned int x,
	                                    unsigned int y,
	                                    unsigned int width,
	                                    unsigned int height)
	{
		// Set viewport
		glViewport(x, y, width, height);
	}
	void VideoDriverOpenGL::clear(unsigned int buffers,
	                              float *color,
	                              float depth)
	{
		if (buffers == 0)
			return;
		glClearColor(color[0], color[1], color[2], color[3]);
		glClearDepth(depth);
		// Enable writing to the buffers we want to write to
		bool clearz = (buffers & 1) != 0;
		if (clearz)
			setDepthWrite(true);
		buffers = buffers >> 1;
		unsigned int currentbuffers = getDrawBuffers();
		if (buffers != 0 && currentbuffers != buffers)
			setDrawBuffers(buffers);
		// Clear buffers
		if (buffers && clearz)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else if (buffers)
			glClear(GL_COLOR_BUFFER_BIT);
		else
			glClear(GL_DEPTH_BUFFER_BIT);
		// Reset draw buffers again
		// Note that we do not have to reset depth writes as this is a per-batch
		// setting and is reset anyways if necessary
		if (buffers != 0 && currentbuffers != buffers)
			setDrawBuffers(currentbuffers);
	}

	void VideoDriverOpenGL::draw(Batch *batch)
	{
		// Change blend mode if necessary
		setBlendMode(batch->shader->uploadeddata.blendmode);
		// Change depth test
		setDepthTest(batch->shader->uploadeddata.depthtest);
		// Change depth write
		setDepthWrite(batch->shader->uploadeddata.depthwrite);
		// Change vertex/index buffer
		setVertexBuffer(batch->vertices);
		setIndexBuffer(batch->indices);
		// Bind buffers/shader
		// TODO: Error checking
		if (currentshader != batch->shader)
		{
			glUseProgram(batch->shader->programobject);
			currentshader = batch->shader;
		}
		Shader::ShaderInfo *shaderinfo = batch->shader->shader->getUploadedData();
		// Apply attribs
		for (unsigned int i = 0; i < shaderinfo->attribs.size(); i++)
		{
			int attribhandle = batch->shader->attriblocations[i];
			if (attribhandle == -1)
				continue;
			unsigned int name = shaderinfo->attribs[i];
			VertexLayoutElement *layoutelem = batch->layout->getElementByName(name);
			if (!layoutelem)
				continue;
			unsigned int opengltype = GL_FLOAT;
			switch (layoutelem->type)
			{
				case VertexElementType::Float:
					opengltype = GL_FLOAT;
					break;
				case VertexElementType::DoubleFloat:
					opengltype = GL_DOUBLE;
					break;
				case VertexElementType::HalfFloat:
					// TODO
					opengltype = GL_FLOAT;
					break;
				case VertexElementType::Integer:
					opengltype = GL_INT;
					break;
				case VertexElementType::Short:
					opengltype = GL_SHORT;
					break;
				case VertexElementType::Byte:
					opengltype = GL_BYTE;
					break;
			}
			glEnableVertexAttribArray(attribhandle);
			glVertexAttribPointer(attribhandle,
			                      layoutelem->components,
			                      opengltype,
			                      GL_FALSE,
			                      layoutelem->stride,
			                      (void*)(layoutelem->offset + batch->vertexoffset));
		}
		// Custom uniforms
		for (unsigned int i = 0; i < shaderinfo->uniforms.size(); i++)
		{
			// Get uniform shader handle
			int uniformhandle = batch->shader->customuniforms[i];
			if (uniformhandle == -1)
				continue;
			// Get uniform data
			unsigned int size = 0;
			float *data = 0;
			for (unsigned int j = 0; j < batch->customuniformcount; j++)
			{
				if (shaderinfo->uniforms[i].name == batch->customuniforms[j].name)
				{
					size = batch->customuniforms[j].size;
					data = batch->customuniforms[j].data;
					break;
				}
			}
			if (!data)
			{
				size = shaderinfo->uniforms[i].size;
				data = shaderinfo->uniforms[i].defvalue;
			}
			// Disallow uploading of more than 4x4 matrices
			if (size > 16)
				size = 16;
			// Upload uniform value
			// TODO: Integer uniforms?
			switch (size)
			{
				case 1:
					glUniform1f(uniformhandle, data[0]);
					break;
				case 2:
					glUniform2f(uniformhandle, data[0], data[1]);
					break;
				case 3:
					glUniform3f(uniformhandle, data[0], data[1], data[2]);
					break;
				case 4:
					glUniform4f(uniformhandle, data[0], data[1], data[2], data[3]);
					break;
				case 9:
					glUniformMatrix3fv(uniformhandle, 1, GL_FALSE, data);
					break;
				case 12:
					glUniformMatrix4x3fv(uniformhandle, 1, GL_FALSE, data);
					break;
				case 16:
					glUniformMatrix4fv(uniformhandle, 1, GL_FALSE, data);
					break;
			}
		}
		// Default uniforms
		{
			UniformLocations &locations = batch->shader->uniforms;
			if (locations.projmat != -1)
				glUniformMatrix4fv(locations.projmat, 1, GL_FALSE, projmat.m);
			if (locations.viewmat != -1)
				glUniformMatrix4fv(locations.viewmat, 1, GL_FALSE, viewmat.m);
			if (locations.viewmatinv != -1)
				glUniformMatrix4fv(locations.viewmatinv, 1, GL_FALSE, viewmatinv.m);
			if (locations.viewprojmat != -1)
				glUniformMatrix4fv(locations.viewprojmat, 1, GL_FALSE, viewprojmat.m);
			math::Matrix4 worldmat = viewprojmat * batch->transmat;
			if (locations.worldmat != -1)
				glUniformMatrix4fv(locations.worldmat, 1, GL_FALSE, worldmat.m);
			if (locations.worldnormalmat != -1)
			{
				// TODO: Is this correct?
				math::Matrix4 worldnormalmat = worldmat.inverse().transposed();
				glUniformMatrix4fv(locations.worldnormalmat, 1, GL_FALSE, worldnormalmat.m);
			}
			// TODO: Other uniforms
		}
		// Apply textures
		for (unsigned int i = 0; i < shaderinfo->samplers.size(); i++)
		{
			int samplerhandle = batch->shader->samplerlocations[i];
			if (samplerhandle == -1)
				continue;
			Material::TextureInfo *textureinfo = 0;
			Material::TextureList *texturelist = batch->material->getUploadedTextures();
			if (!texturelist)
				continue;
			for (unsigned int j = 0; j < texturelist->texturecount; j++)
			{
				if (texturelist->textures[j].name == shaderinfo->samplers[i].name)
				{
					textureinfo = &texturelist->textures[j];
					break;
				}
			}
			if (!textureinfo)
				continue;
			int opengltype = GL_TEXTURE_2D;
			switch (textureinfo->texture->getTextureType())
			{
				case TextureType::Texture1D:
					opengltype = GL_TEXTURE_1D;
					break;
				case TextureType::Texture2D:
					opengltype = GL_TEXTURE_2D;
					break;
				case TextureType::Texture3D:
					opengltype = GL_TEXTURE_3D;
					break;
				case TextureType::TextureCube:
					opengltype = GL_TEXTURE_CUBE_MAP;
					break;
			}
			// TODO: Better manage texture units
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(opengltype, textureinfo->texture->getHandle());
			glUniform1i(samplerhandle, i);
			// TODO: Additionally bound textures (getBoundTextures())
		}
		// Render triangles
		unsigned int indexcount = batch->endindex - batch->startindex;
		if (batch->indextype == 1)
		{
			glDrawElements(GL_TRIANGLES,
			               indexcount,
			               GL_UNSIGNED_BYTE,
			               (void*)(batch->startindex));
		}
		else if (batch->indextype == 2)
		{
			glDrawElements(GL_TRIANGLES,
			               indexcount,
			               GL_UNSIGNED_SHORT,
			               (void*)(batch->startindex * 2));
		}
		else if (batch->indextype == 4)
		{
			glDrawElements(GL_TRIANGLES,
			               indexcount,
			               GL_UNSIGNED_INT,
			               (void*)(batch->startindex * 4));
		}
		// Increase polygon/batch counters
		getStats().increaseBatchCount(1);
		getStats().increasePolygonCount(indexcount / 3);
		// Clean up attribs
		for (unsigned int i = 0; i < batch->shader->attriblocations.size(); i++)
		{
			if (batch->shader->attriblocations[i] == -1)
				continue;
			glDisableVertexAttribArray(batch->shader->attriblocations[i]);
		}
	}

	void VideoDriverOpenGL::endFrame()
	{
		// Unbind render target
		if (currentfb)
		{
			generateMipmaps(currentfb);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			currentfb = 0;
		}
		// TODO: Unbind buffers, textures, program
		glUseProgram(0);
		currentshader = 0;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		currentvertices = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		currentindices = 0;
		// Unbind currently used textures
		unbindTextures();
	}

	void VideoDriverOpenGL::setMatrices(math::Matrix4 projmat,
	                                    math::Matrix4 viewmat)
	{
		this->projmat = projmat;
		this->viewmat = viewmat;
		viewmatinv = viewmat.inverse();
		viewprojmat = projmat * viewmat;
		// TODO: These should not only updated in the shader when necessary
	}

	void VideoDriverOpenGL::generateMipmaps(FrameBuffer::Configuration *fb)
	{
		for (unsigned int i = 0; i < fb->colorbuffers.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, fb->colorbuffers[i]);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void VideoDriverOpenGL::setDepthWrite(bool depthwrite)
	{
		if (currentdepthwrite == depthwrite)
			return;
		if (depthwrite)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		currentdepthwrite = depthwrite;
	}
	void VideoDriverOpenGL::setDepthTest(DepthTest::List test)
	{
		if (test == currentdepthtest)
			return;
		switch (test)
		{
			case DepthTest::Always:
				glDepthFunc(GL_ALWAYS);
				break;
			case DepthTest::Equal:
				glDepthFunc(GL_EQUAL);
				break;
			case DepthTest::Less:
				glDepthFunc(GL_LESS);
				break;
			case DepthTest::LessEqual:
				glDepthFunc(GL_LEQUAL);
				break;
			case DepthTest::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case DepthTest::GreaterEqual:
				glDepthFunc(GL_GEQUAL);
				break;
		}
		currentdepthtest = test;
	}
	void VideoDriverOpenGL::setDrawBuffers(unsigned int buffers)
	{
		if (buffers == currentdrawbuffers)
			return;
		forceDrawBuffers(buffers);
	}
	void VideoDriverOpenGL::forceDrawBuffers(unsigned int buffers)
	{
		if (!currentfb)
		{
			if (buffers & 1)
				glDrawBuffer(GL_BACK);
			else
				glDrawBuffer(GL_NONE);
		}
		else
		{
			unsigned int drawbuffers[16];
			unsigned int buffercount = 0;
			unsigned int bufferindex = 0;
			while (buffers != 0)
			{
				if (buffers & 1)
				{
					drawbuffers[buffercount] = GL_COLOR_ATTACHMENT0_EXT + bufferindex;
					buffercount++;
				}
				bufferindex++;
			}
			glDrawBuffers(buffercount, drawbuffers);
		}
		currentdrawbuffers = buffers;
	}
	void VideoDriverOpenGL::setBlendMode(BlendMode::List mode)
	{
		if (mode == currentblendmode)
			return;
		// Enable blending if we previously rendered solid geometry
		if (currentblendmode == BlendMode::Replace)
			glEnable(GL_BLEND);
		// Switch between blending equations
		if (mode == BlendMode::Minimum)
			glBlendEquation(GL_MIN);
		else if (mode == BlendMode::Maximum)
			glBlendEquation(GL_MAX);
		else if (currentblendmode == BlendMode::Minimum
		         || currentblendmode == BlendMode::Maximum)
			glBlendEquation(GL_FUNC_ADD);
		// Set blend function
		switch (mode)
		{
			case BlendMode::Replace:
				glDisable(GL_BLEND);
				break;
			case BlendMode::Add:
			case BlendMode::Minimum:
			case BlendMode::Maximum:
				glBlendFunc(GL_ONE, GL_ONE);
				break;
			case BlendMode::AddBlended:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			case BlendMode::Blend:
				glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
				break;
			case BlendMode::Multiply:
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				break;
		}
		currentblendmode = mode;
	}
	void VideoDriverOpenGL::setVertexBuffer(VertexBuffer *vertices)
	{
		if (vertices != currentvertices)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertices->getHandle());
			currentvertices = vertices;
		}
	}
	void VideoDriverOpenGL::setIndexBuffer(IndexBuffer *indices)
	{
		if (indices != currentindices)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices->getHandle());
			currentindices = indices;
		}
	}
}
}
}
