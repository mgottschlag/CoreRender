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
#include "TextureOpenGL.hpp"
#include "IndexBufferOpenGL.hpp"
#include "VertexBufferOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "FrameBufferOpenGL.hpp"
#include "MeshOpenGL.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/render/VertexLayout.hpp"
#include "CoreRender/render/Material.hpp"
#include "CoreRender/render/RenderTarget.hpp"
#include "CoreRender/res/ResourceManager.hpp"

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
		// Initialize instancing transMat buffer object
		// TODO: This is a hack, but using plain vertex arrays hits a slow path
		// at least here
		glGenBuffers(1, &instancingbuffer);
		return true;
	}
	bool VideoDriverOpenGL::shutdown()
	{
		return true;
	}

	Texture::Ptr VideoDriverOpenGL::createTexture(UploadManager &uploadmgr,
	                                              res::ResourceManager *rmgr,
	                                              const std::string &name)
	{
		return new TextureOpenGL(uploadmgr, rmgr, name);
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
	Mesh::Ptr VideoDriverOpenGL::createMesh(UploadManager &uploadmgr)
	{
		// TODO: Conditionally enable vertex array objects
		return new MeshOpenGL(uploadmgr, false);
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
			if (target->colorbuffers[i])
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				                          GL_COLOR_ATTACHMENT0_EXT + i,
				                          GL_TEXTURE_2D,
				                          target->colorbuffers[i]->getHandle(),
				                          0);
			else
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				                          GL_COLOR_ATTACHMENT0_EXT + i,
				                          GL_TEXTURE_2D,
				                          0,
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
				                          target->depthbuffer->getHandle(),
				                          0);
			}
			currentfb->depthbuffer = target->depthbuffer;
		}
		// Change draw buffers
		unsigned int drawbuffers;
		if (target->colorbuffercount > 0)
			drawbuffers = 0xFFFFFFFF >> (32 - target->colorbuffercount);
		else
			drawbuffers = 0;
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
		viewport[0] = x;
		viewport[1] = y;
		viewport[2] = width;
		viewport[3] = height;
	}
	void VideoDriverOpenGL::clear(unsigned int buffers,
	                              float *color,
	                              float depth)
	{
		if (buffers == 0)
			return;
		glClearColor(color[0], color[1], color[2], color[3]);
		glClearDepth(depth);
		// We want to restrict clears to the current viewport
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		glEnable(GL_SCISSOR_TEST);
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
		glDisable(GL_SCISSOR_TEST);
		// Reset draw buffers again
		// Note that we do not have to reset depth writes as this is a per-batch
		// setting and is reset anyways if necessary
		if (buffers != 0 && currentbuffers != buffers)
			setDrawBuffers(currentbuffers);
	}

	void VideoDriverOpenGL::draw(Batch *batch)
	{
		if (batch->shader->programobject == 0)
			return;
		// Change blend mode if necessary
		setBlendMode(batch->shader->uploadeddata.blendmode);
		// Change depth test
		setDepthTest(batch->shader->uploadeddata.depthtest);
		// Change depth write
		setDepthWrite(batch->shader->uploadeddata.depthwrite);
		// Change vertex/index buffer
		Mesh::MeshData *mesh = ((MeshOpenGL*)batch->mesh)->getData();
		setVertexBuffer(mesh->vertices);
		if (mesh->indices)
			setIndexBuffer(mesh->indices);
		// Bind shader
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
			VertexLayoutElement *layoutelem = mesh->layout->getElementByName(name);
			if (!layoutelem)
				continue;
			unsigned int opengltype = GL_FLOAT;
			int normalize = GL_TRUE;
			switch (layoutelem->type)
			{
				case VertexElementType::Float:
					opengltype = GL_FLOAT;
					normalize = GL_FALSE;
					break;
				case VertexElementType::DoubleFloat:
					opengltype = GL_DOUBLE;
					normalize = GL_FALSE;
					break;
				case VertexElementType::HalfFloat:
					// TODO
					opengltype = GL_FLOAT;
					normalize = GL_FALSE;
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
				case VertexElementType::UnsignedInteger:
					opengltype = GL_UNSIGNED_INT;
					break;
				case VertexElementType::UnsignedShort:
					opengltype = GL_UNSIGNED_SHORT;
					break;
				case VertexElementType::UnsignedByte:
					opengltype = GL_UNSIGNED_BYTE;
					break;
			}
			glEnableVertexAttribArray(attribhandle);
			glVertexAttribPointer(attribhandle,
			                      layoutelem->components,
			                      opengltype,
			                      normalize,
			                      layoutelem->stride,
			                      (void*)(layoutelem->offset + mesh->vertexoffset));
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
		// Apply textures
		applyTextures(batch->shader, batch->material);
		// Default uniforms (not dependant on the transformation matrix)
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
			if (locations.viewerpos != -1)
				glUniform3f(locations.viewerpos, viewer.x, viewer.y, viewer.z);
			if (batch->shader->skinning && locations.skinmat != -1)
				glUniformMatrix4fv(locations.skinmat,
				                   batch->skinmatcount,
				                   GL_FALSE,
				                   batch->skinmat);
			if (locations.framebufsize != -1)
				glUniform2f(locations.framebufsize, viewport[2], viewport[3]);
			// TODO: Other uniforms
			LightUniforms *light = getLightUniforms();
			if (light)
			{
				if (locations.lightpos != -1)
					glUniform4f(locations.lightpos,
					            light->position[0],
					            light->position[1],
					            light->position[2],
					            light->position[3]);
				if (locations.lightdir != -1)
					glUniform3f(locations.lightdir,
					            light->direction[0],
					            light->direction[1],
					            light->direction[2]);
				if (locations.lightcolor != -1)
					glUniform4f(locations.lightcolor,
					            light->color[0],
					            light->color[1],
					            light->color[2],
					            light->color[3]);
				if (locations.shadowmat != -1)
					glUniformMatrix4fv(locations.shadowmat, 1, GL_FALSE, light->shadowmat.m);
				if (locations.shadowmap && light->shadowmap)
				{
					unsigned int texcount = shaderinfo->samplers.size();
					glActiveTexture(GL_TEXTURE0 + texcount);
					glBindTexture(GL_TEXTURE_2D, light->shadowmap->getHandle());
					glUniform1i(locations.shadowmap, texcount);
				}
			}
		}
		// Draw geometry
		if (batch->transmatcount == 0)
			drawSingle(batch, batch->transmat);
		else
		{
			if (batch->shader->instancing)
				drawInstanced(batch, batch->transmatcount, batch->transmatlist);
			else
			{
				for (unsigned int i = 0; i < batch->transmatcount; i++)
				{
					drawSingle(batch, batch->transmatlist[i]);
				}
			}
		}
		// Clean up attribs
		for (unsigned int i = 0; i < batch->shader->attriblocations.size(); i++)
		{
			if (batch->shader->attriblocations[i] == -1)
				continue;
			glDisableVertexAttribArray(batch->shader->attriblocations[i]);
		}
	}

	void VideoDriverOpenGL::drawQuad(float *vertices,
	                                 ShaderCombination *shader,
	                                 Material *material,
	                                 LightUniforms *light)
	{
		if (shader->programobject == 0)
			return;
		// Change blend mode if necessary
		setBlendMode(shader->uploadeddata.blendmode);
		// Change depth test
		setDepthTest(shader->uploadeddata.depthtest);
		// Change depth write
		setDepthWrite(shader->uploadeddata.depthwrite);
		// Disable vertex/index buffers, we directly read the vertices from the
		// passed buffer
		setVertexBuffer(0);
		setIndexBuffer(0);
		// Bind shader
		// TODO: Error checking
		if (currentshader != shader)
		{
			glUseProgram(shader->programobject);
			currentshader = shader;
		}
		Shader::ShaderInfo *shaderinfo = shader->shader->getUploadedData();
		// Apply attribs
		// We only bind the hardcoded pos attribute here
		float vertexdata[8];
		vertexdata[0] = vertices[0];
		vertexdata[1] = vertices[1];
		vertexdata[2] = vertices[2];
		vertexdata[3] = vertices[1];
		vertexdata[4] = vertices[0];
		vertexdata[5] = vertices[3];
		vertexdata[6] = vertices[2];
		vertexdata[7] = vertices[3];
		unsigned int posattribname = material->getManager()->getNameRegistry().getAttrib("pos");
		for (unsigned int i = 0; i < shaderinfo->attribs.size(); i++)
		{
			int attribhandle = shader->attriblocations[i];
			if (attribhandle == -1)
				continue;
			unsigned int name = shaderinfo->attribs[i];
			if (name != posattribname)
				continue;
			glEnableVertexAttribArray(attribhandle);
			glVertexAttribPointer(attribhandle, 2, GL_FLOAT, GL_FALSE, 0, vertexdata);
		}
		// Apply textures
		applyTextures(shader, material);
		// Uniforms
		// TODO
		UniformLocations &locations = shader->uniforms;
		if (locations.framebufsize != -1)
			glUniform2f(locations.framebufsize, viewport[2], viewport[3]);
		if (locations.projmat != -1)
			glUniformMatrix4fv(locations.projmat, 1, GL_FALSE, projmat.m);
		if (locations.viewmat != -1)
			glUniformMatrix4fv(locations.viewmat, 1, GL_FALSE, viewmat.m);
		if (locations.viewmatinv != -1)
			glUniformMatrix4fv(locations.viewmatinv, 1, GL_FALSE, viewmatinv.m);
		if (locations.viewprojmat != -1)
			glUniformMatrix4fv(locations.viewprojmat, 1, GL_FALSE, viewprojmat.m);
		if (locations.viewerpos != -1)
			glUniform3f(locations.viewerpos, viewer.x, viewer.y, viewer.z);
		if (light)
		{
			if (locations.lightpos != -1)
				glUniform4f(locations.lightpos,
				            light->position[0],
				            light->position[1],
				            light->position[2],
				            light->position[3]);
			if (locations.lightdir != -1)
				glUniform3f(locations.lightdir,
				            light->direction[0],
				            light->direction[1],
				            light->direction[2]);
			if (locations.lightcolor != -1)
				glUniform4f(locations.lightcolor,
				            light->color[0],
				            light->color[1],
				            light->color[2],
				            light->color[3]);
			if (locations.shadowmat != -1)
				glUniformMatrix4fv(locations.shadowmat, 1, GL_FALSE, light->shadowmat.m);
			if (locations.shadowmap && light->shadowmap)
			{
				unsigned int texcount = shaderinfo->samplers.size();
				glActiveTexture(GL_TEXTURE0 + texcount);
				glBindTexture(GL_TEXTURE_2D, light->shadowmap->getHandle());
				glUniform1i(locations.shadowmap, texcount);
			}
		}
		// Draw a single quad
		unsigned char indices[] = {
			0, 1, 2,
			2, 1, 3
		};
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
		// Clean up attribs
		for (unsigned int i = 0; i < shader->attriblocations.size(); i++)
		{
			if (shader->attriblocations[i] == -1)
				continue;
			unsigned int name = shaderinfo->attribs[i];
			if (name != posattribname)
				return;
			glDisableVertexAttribArray(shader->attriblocations[i]);
		}
	}

	void VideoDriverOpenGL::beginFrame()
	{
		// Init static OpenGL states
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	void VideoDriverOpenGL::endFrame()
	{
		// Unbind render target
		if (currentfb)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			generateMipmaps(currentfb);
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
		bindTextures(0, 0);
		setLightUniforms(0);
	}

	void VideoDriverOpenGL::setMatrices(math::Mat4f projmat,
	                                    math::Mat4f viewmat,
	                                    math::Vec3f viewer)
	{
		this->projmat = projmat;
		this->viewmat = viewmat;
		viewmatinv = viewmat.inverse();
		viewprojmat = projmat * viewmat;
		this->viewer = viewer;
		// TODO: These should not only updated in the shader when necessary
	}

	void VideoDriverOpenGL::generateMipmaps(FrameBuffer::Configuration *fb)
	{
		for (unsigned int i = 0; i < fb->colorbuffers.size(); i++)
		{
			if (fb->colorbuffers[i] && fb->colorbuffers[i]->hasMipmaps())
			{
				unsigned int type = fb->colorbuffers[i]->getOpenGLType();
				glBindTexture(type, fb->colorbuffers[i]->getHandle());
				glEnable(type);
				glGenerateMipmapEXT(type);
				glDisable(type);
				glBindTexture(type, 0);
			}
		}
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
		currentdrawbuffers = buffers;
		if (!currentfb)
		{
			if (buffers & 1)
				glDrawBuffer(GL_BACK);
			else
				glDrawBuffer(GL_NONE);
		}
		else if (buffers == 0)
		{
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
				buffers = buffers >> 1;
				bufferindex++;
			}
			glDrawBuffers(buffercount, drawbuffers);
		}
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
			if (vertices)
				glBindBuffer(GL_ARRAY_BUFFER, vertices->getHandle());
			else
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			currentvertices = vertices;
		}
	}
	void VideoDriverOpenGL::setIndexBuffer(IndexBuffer *indices)
	{
		if (indices != currentindices)
		{
			if (indices)
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices->getHandle());
			else
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			currentindices = indices;
		}
	}

	void VideoDriverOpenGL::drawSingle(Batch *batch, const math::Mat4f &transmat)
	{
		// Default uniforms (not dependant on the transformation matrix)
		{
			UniformLocations &locations = batch->shader->uniforms;
			if (locations.worldmat != -1)
				glUniformMatrix4fv(locations.worldmat, 1, GL_FALSE, transmat.m);
			if (locations.worldnormalmat != -1)
			{
				// TODO: Is this correct?
				math::Mat4f worldnormalmat = transmat.inverse().transposed();
				glUniformMatrix4fv(locations.worldnormalmat, 1, GL_FALSE, worldnormalmat.m);
			}
			// TODO: Other uniforms
		}
		// Render triangles
		Mesh::MeshData *mesh = ((MeshOpenGL*)batch->mesh)->getData();
		bool useindices = true;
		if (mesh->primitivetype == PrimitiveType::QuadList
			|| mesh->primitivetype == PrimitiveType::TriangleList)
			useindices = false;
		if (useindices)
		{
			// TODO: Primitive type
			unsigned int indexcount = mesh->indexcount;
			if (mesh->indextype == 1)
			{
				glDrawElements(GL_TRIANGLES,
				               indexcount,
				               GL_UNSIGNED_BYTE,
				               (void*)(mesh->startindex));
			}
			else if (mesh->indextype == 2)
			{
				glDrawElements(GL_TRIANGLES,
				               indexcount,
				               GL_UNSIGNED_SHORT,
				               (void*)(mesh->startindex * 2));
			}
			else if (mesh->indextype == 4)
			{
				glDrawElements(GL_TRIANGLES,
				               indexcount,
				               GL_UNSIGNED_INT,
				               (void*)(mesh->startindex * 4));
			}
			// Increase polygon/batch counters
			getStats().increaseBatchCount(1);
			getStats().increasePolygonCount(indexcount / 3);
		}
		else
		{
			glDrawArrays(GL_QUADS, 0, mesh->vertexcount);
			// TODO
		}
	}
	void VideoDriverOpenGL::drawInstanced(Batch *batch,
	                                      unsigned int instancecount,
	                                      math::Mat4f *transmat)
	{
		// Disable vertex buffer as we specify the transformation matrices as
		// an attrib directly without using a buffer object
		// TODO: Not using a buffer hits a slow path
		//setVertexBuffer(0);
		// Upload transformation matrices
		glBindBuffer(GL_ARRAY_BUFFER, instancingbuffer);
		glBufferData(GL_ARRAY_BUFFER, instancecount * 16 * sizeof(float),
			transmat, GL_STREAM_DRAW);
		// Set transMat attrib
		if (batch->shader->transmatattrib != -1)
		{
			glEnableVertexAttribArray(batch->shader->transmatattrib);
			glVertexAttribPointer(batch->shader->transmatattrib, 4, GL_FLOAT,
				GL_FALSE, 16 * sizeof(float), 0);
			glEnableVertexAttribArray(batch->shader->transmatattrib + 1);
			glVertexAttribPointer(batch->shader->transmatattrib + 1, 4,
				GL_FLOAT, GL_FALSE, 16 * sizeof(float),
				(void*)(4 * sizeof(float)));
			glEnableVertexAttribArray(batch->shader->transmatattrib + 2);
			glVertexAttribPointer(batch->shader->transmatattrib + 2, 4,
				GL_FLOAT, GL_FALSE, 16 * sizeof(float),
				(void*)(8 * sizeof(float)));
			glEnableVertexAttribArray(batch->shader->transmatattrib + 3);
			glVertexAttribPointer(batch->shader->transmatattrib + 3, 4,
				GL_FLOAT, GL_FALSE, 16 * sizeof(float),
				(void*)(12 * sizeof(float)));
			// Enable instancing
			glVertexAttribDivisorARB(batch->shader->transmatattrib, 1);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 1, 1);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 2, 1);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 3, 1);
		}
		// Render triangles
		Mesh::MeshData *mesh = ((MeshOpenGL*)batch->mesh)->getData();
		unsigned int indexcount = mesh->indexcount;
		if (mesh->indextype == 1)
		{
			glDrawElementsInstancedARB(GL_TRIANGLES,
			                           indexcount,
			                           GL_UNSIGNED_BYTE,
			                           (void*)(mesh->startindex),
			                           instancecount);
		}
		else if (mesh->indextype == 2)
		{
			glDrawElementsInstancedARB(GL_TRIANGLES,
			                           indexcount,
			                           GL_UNSIGNED_SHORT,
			                           (void*)(mesh->startindex * 2),
			                           instancecount);
		}
		else if (mesh->indextype == 4)
		{
			glDrawElementsInstancedARB(GL_TRIANGLES,
			                           indexcount,
			                           GL_UNSIGNED_INT,
			                           (void*)(mesh->startindex * 4),
			                           instancecount);
		}
		// Increase polygon/batch counters
		getStats().increaseBatchCount(instancecount);
		getStats().increasePolygonCount(indexcount / 3 * instancecount);
		// Clean up buffer binding
		setVertexBuffer(0);
		// Clean up attribs
		if (batch->shader->transmatattrib != -1)
		{
			glVertexAttribDivisorARB(batch->shader->transmatattrib, 0);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 1, 0);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 2, 0);
			glVertexAttribDivisorARB(batch->shader->transmatattrib + 3, 0);
			glDisableVertexAttribArray(batch->shader->transmatattrib);
			glDisableVertexAttribArray(batch->shader->transmatattrib + 1);
			glDisableVertexAttribArray(batch->shader->transmatattrib + 2);
			glDisableVertexAttribArray(batch->shader->transmatattrib + 3);
		}
	}

	void VideoDriverOpenGL::applyTextures(ShaderCombination *shader,
	                                      Material *material)
	{
		Shader::ShaderInfo *shaderinfo = shader->shader->getUploadedData();
		// Apply textures
		for (unsigned int i = 0; i < shaderinfo->samplers.size(); i++)
		{
			int samplerhandle = shader->samplerlocations[i];
			if (samplerhandle == -1)
				continue;
			Texture *texture = 0;
			// Fetch the texture from the material
			Material::TextureList *texturelist = material->getUploadedTextures();
			if (texturelist)
			{
				for (unsigned int j = 0; j < texturelist->texturecount; j++)
				{
					if (texturelist->textures[j].name == shaderinfo->samplers[i].name)
					{
						texture = texturelist->textures[j].texture.get();
						break;
					}
				}
			}
			// Additionally bound textures (getBoundTextures())
			for (unsigned int j = 0; j < getBoundTextureCount(); j++)
			{
				if (shaderinfo->samplers[i].name == getBoundTextures()[j].name)
				{
					texture = getBoundTextures()[j].tex;
					break;
				}
			}
			if (!texture)
				continue;
			int opengltype = GL_TEXTURE_2D;
			switch (texture->getTextureType())
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
			glBindTexture(opengltype, texture->getHandle());
			glUniform1i(samplerhandle, i);
			// TODO: Take care of unbinding textures later?
		}
	}
}
}
}
