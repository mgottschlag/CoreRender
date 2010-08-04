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
#include "CoreRender/render/RenderBatch.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	VideoDriverOpenGL::VideoDriverOpenGL(core::Log::Ptr log)
		: log(log)
	{
	}
	VideoDriverOpenGL::~VideoDriverOpenGL()
	{
	}

	bool VideoDriverOpenGL::init()
	{
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
		if (!caps.init())
		{
			log->error("Could not initialize capabilities.");
			return false;
		}
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
	                                             const std::string &name,
	                                             VertexBufferUsage::List usage)
	{
		return new VertexBufferOpenGL(renderer, rmgr, name, usage);
	}
	Shader::Ptr VideoDriverOpenGL::createShader(Renderer *renderer,
	                                            res::ResourceManager *rmgr,
	                                            const std::string &name)
	{
		return new ShaderOpenGL(renderer, rmgr, name);
	}

	void VideoDriverOpenGL::setRenderTarget(int handle)
	{
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
		glVertexPointer(3, GL_FLOAT, 32, 0);
		// TODO
		// Apply uniforms
		// TODO
		// Render triangles
		// TODO: The client state never changes
		glEnableClientState(GL_VERTEX_ARRAY);
		log->debug("Drawing.");
		glDrawElements(GL_TRIANGLES,
		               batch->endindex - batch->startindex,
		               GL_UNSIGNED_SHORT,
		               (void*)(batch->startindex * 2));
		// TODO
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void VideoDriverOpenGL::endFrame()
	{
		// TODO: Unbind buffers, textures, program
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
}
}
