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

#include "FrameBufferOpenGL.hpp"

#include <GL/glew.h>
#include <iostream>

namespace cr
{
namespace render
{
namespace opengl
{
	FrameBufferOpenGL::FrameBufferOpenGL(Renderer *renderer,
	                                     res::ResourceManager *rmgr,
	                                     const std::string &name)
		: FrameBuffer(renderer, rmgr, name)
	{
	}
	FrameBufferOpenGL::~FrameBufferOpenGL()
	{
	}

	bool FrameBufferOpenGL::create()
	{
		glGenFramebuffersEXT(1, &config.handle);
		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cout << "Could not create index buffer: " << gluErrorString(error) << std::endl;
		}
		// TODO: Error checking
		return true;
	}
	bool FrameBufferOpenGL::destroy()
	{
		if (config.defaultdepthbuffer)
		{
			// Destroy depth buffer
			glDeleteRenderbuffersEXT(1, &config.defaultdepthbuffer);
			config.defaultdepthbuffer = 0;
		}
		glDeleteFramebuffersEXT(1, &config.handle);
		config.handle = 0;
		return true;
	}
	bool FrameBufferOpenGL::upload()
	{
		if (hasDepthBuffer())
		{
			// Generate depth buffer if necessary
			if (!config.defaultdepthbuffer)
			{
				glGenRenderbuffersEXT(1, &config.defaultdepthbuffer);
			}
			// Change depth buffer size
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,
			                      config.defaultdepthbuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
			                         GL_DEPTH_COMPONENT,
			                         getWidth(),
			                         getHeight());
			// Add depth buffer to frame buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, config.handle);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
			                             GL_DEPTH_ATTACHMENT_EXT,
			                             GL_RENDERBUFFER_EXT,
			                             config.defaultdepthbuffer);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		else if (config.defaultdepthbuffer)
		{
			// Destroy depth buffer
			glDeleteRenderbuffersEXT(1, &config.defaultdepthbuffer);
			config.defaultdepthbuffer = 0;
		}
		return true;
	}
}
}
}
