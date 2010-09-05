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
#include "../FrameData.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	VideoDriverOpenGL::VideoDriverOpenGL(core::Log::Ptr log)
		: log(log), currenttarget(0)
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

	Texture2D::Ptr VideoDriverOpenGL::createTexture2D(Renderer *renderer,
	                                                  res::ResourceManager *rmgr,
	                                                  const std::string &name)
	{
		return new Texture2DOpenGL(renderer, rmgr, name);
	}
	IndexBuffer::Ptr VideoDriverOpenGL::createIndexBuffer(Renderer *renderer,
	                                           res::ResourceManager *rmgr,
	                                           const std::string &name)
	{
		return new IndexBufferOpenGL(renderer, rmgr, name);
	}
	VertexBuffer::Ptr VideoDriverOpenGL::createVertexBuffer(Renderer *renderer,
	                                             res::ResourceManager *rmgr,
	                                             const std::string &name)
	{
		return new VertexBufferOpenGL(renderer, rmgr, name);
	}
	Shader::Ptr VideoDriverOpenGL::createShader(Renderer *renderer,
	                                            res::ResourceManager *rmgr,
	                                            const std::string &name)
	{
		return new ShaderOpenGL(renderer, rmgr, name);
	}
	FrameBuffer::Ptr VideoDriverOpenGL::createFrameBuffer(Renderer *renderer,
	                                                      res::ResourceManager *rmgr,
	                                                      const std::string &name)
	{
		return new FrameBufferOpenGL(renderer, rmgr, name);
	}

	void VideoDriverOpenGL::setRenderTarget(const RenderTargetInfo &target)
	{
		// Set viewport
		glViewport(0, 0, target.width, target.height);
		// Bind frame buffer object
		if (target.framebuffer == 0)
		{
			if (currenttarget)
			{
				// Generate mipmaps
				generateMipmaps(currenttarget);
				// Unbind framebuffer
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				currenttarget = 0;
			}
			return;
		}
		if (!currenttarget || target.framebuffer != currenttarget->framebuffer)
		{
			// Generate mipmaps
			if (currenttarget)
				generateMipmaps(currenttarget);
			// Change framebuffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, target.framebuffer);
		}
		// Bind color buffers
		for (unsigned int i = 0; i < target.colorbuffercount; i++)
		{
			if (currenttarget
				&& target.colorbuffers[i] == currenttarget->colorbuffers[i])
				continue;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			                          GL_COLOR_ATTACHMENT0_EXT + i,
			                          GL_TEXTURE_2D,
			                          target.colorbuffers[i],
			                          0);
		}
		unsigned int buffers[1] = {GL_COLOR_ATTACHMENT0_EXT};
		glDrawBuffers(1, buffers);
		// Unbind unused color buffers
		// TODO: We are currently possibly doing it on the wrong fbo!
		if (currenttarget)
		{
			for (unsigned int i = target.colorbuffercount;
				i < currenttarget->colorbuffercount; i++)
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				                          GL_COLOR_ATTACHMENT0_EXT + i,
				                          GL_TEXTURE_2D,
				                          0,
				                          0);
			}
		}
		// Bind depth buffer
		// TODO: We maybe need information about the previous depth buffer here
		if (target.depthbuffer)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			                          GL_DEPTH_ATTACHMENT_EXT,
			                          GL_TEXTURE_2D,
			                          target.depthbuffer,
			                          0);
		}
		else if (currenttarget && currenttarget->depthbuffer)
		{
			// TODO: We are currently possibly doing it on the wrong fbo!
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			                          GL_DEPTH_ATTACHMENT_EXT,
			                          GL_TEXTURE_2D,
			                          0,
			                          0);
		}
		// Check fbo state
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			log->error("Invalid framebuffer (%d).", status);
		}
		// Store information about current buffer
		currenttarget = &target;
	}
	void VideoDriverOpenGL::clear(bool colorbuffer,
	                              bool zbuffer,
	                              core::Color color,
	                              float depth)
	{
		if (!colorbuffer && !zbuffer)
			return;
		glClearColor((float)color.getRed() / 255.0f,
		             (float)color.getGreen() / 255.0f,
		             (float)color.getBlue() / 255.0f,
		             (float)color.getAlpha() / 255.0f);
		glClearDepth(depth);
		if (colorbuffer && zbuffer)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else if (colorbuffer)
			glClear(GL_COLOR_BUFFER_BIT);
		else if (zbuffer)
			glClear(GL_DEPTH_BUFFER_BIT);
	}

	void VideoDriverOpenGL::draw(RenderBatch *batch)
	{
		// TODO: Keep track of state changes, do not change too much
		// Bind buffers/shader
		// TODO: Error checking
		glUseProgram(batch->shader);

		glBindBuffer(GL_ARRAY_BUFFER, batch->vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->indices);
		// Apply attribs
		for (unsigned int i = 0; i < batch->attribcount; i++)
		{
			if (batch->attribs[i].shaderhandle == -1)
				continue;
			unsigned int opengltype = GL_FLOAT;
			switch (batch->attribs[i].type)
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
			glEnableVertexAttribArray(batch->attribs[i].shaderhandle);
			glVertexAttribPointer(batch->attribs[i].shaderhandle,
			                      batch->attribs[i].components,
			                      opengltype,
			                      GL_FALSE,
			                      batch->attribs[i].stride,
			                      (void*)(batch->attribs[i].address + batch->vertexoffset));
		}
		// Apply uniforms
		for (unsigned int i = 0; i < batch->uniformcount; i++)
		{
			if (batch->uniforms[i].shaderhandle == -1)
				continue;
			switch (batch->uniforms[i].type)
			{
				case ShaderVariableType::Float:
					glUniform1f(batch->uniforms[i].shaderhandle,
					            batch->uniforms[i].data[0]);
					break;
				case ShaderVariableType::Float2:
					glUniform2f(batch->uniforms[i].shaderhandle,
					            batch->uniforms[i].data[0],
					            batch->uniforms[i].data[1]);
					break;
				case ShaderVariableType::Float3:
					glUniform3f(batch->uniforms[i].shaderhandle,
					            batch->uniforms[i].data[0],
					            batch->uniforms[i].data[1],
					            batch->uniforms[i].data[2]);
					break;
				case ShaderVariableType::Float4:
					glUniform4f(batch->uniforms[i].shaderhandle,
					            batch->uniforms[i].data[0],
					            batch->uniforms[i].data[1],
					            batch->uniforms[i].data[2],
					            batch->uniforms[i].data[3]);
					break;
				case ShaderVariableType::Float4x4:
					glUniformMatrix4fv(batch->uniforms[i].shaderhandle,
					                   1,
					                   GL_FALSE,
					                   batch->uniforms[i].data);
					break;
				case ShaderVariableType::Float3x3:
					glUniformMatrix3fv(batch->uniforms[i].shaderhandle,
					                   1,
					                   GL_FALSE,
					                   batch->uniforms[i].data);
					break;
				case ShaderVariableType::Float4x3:
					glUniformMatrix4x3fv(batch->uniforms[i].shaderhandle,
					                     1,
					                     GL_FALSE,
					                     batch->uniforms[i].data);
					break;
				case ShaderVariableType::Float3x4:
					glUniformMatrix3x4fv(batch->uniforms[i].shaderhandle,
					                     1,
					                     GL_FALSE,
					                     batch->uniforms[i].data);
					break;
				default:
					break;
			}
		}
		// TODO
		// Apply textures
		for (unsigned int i = 0; i < batch->texcount; i++)
		{
			if (batch->textures[i].shaderhandle == -1)
				continue;
			if (batch->textures[i].texhandle == -1)
				continue;
			int opengltype = GL_TEXTURE_2D;
			switch (batch->textures[i].type)
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
			glActiveTexture(GL_TEXTURE0 + batch->textures[i].textureindex);
			glBindTexture(opengltype, batch->textures[i].texhandle);
			glUniform1i(batch->textures[i].shaderhandle,
			            batch->textures[i].textureindex);
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
		for (unsigned int i = 0; i < batch->attribcount; i++)
		{
			if (batch->attribs[i].shaderhandle == -1)
				continue;
			glDisableVertexAttribArray(batch->attribs[i].shaderhandle);
		}
		// Clean up memory
		for (unsigned int i = 0; i < batch->uniformcount; i++)
		{
			delete[] batch->uniforms[i].data;
		}
		delete[] batch->attribs;
		delete[] batch->uniforms;
		delete[] batch->textures;
		delete batch;
		// TODO
		// TODO: Should be done by the memory pool class?
	}

	void VideoDriverOpenGL::endFrame()
	{
		// Unbind render target
		if (currenttarget)
		{
			generateMipmaps(currenttarget);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			currenttarget = 0;
		}
		// TODO: Unbind buffers, textures, program
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void VideoDriverOpenGL::generateMipmaps(const RenderTargetInfo *target)
	{
		for (unsigned int i = 0; i < target->colorbuffercount; i++)
		{
			glBindTexture(GL_TEXTURE_2D, target->colorbuffers[i]);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
}
}
