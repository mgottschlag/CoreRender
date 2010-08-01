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

#include "RenderContextGLFW.hpp"

#include <GL/glfw.h>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderContextGLFW::RenderContextGLFW()
	{
		glfwInit();
	}
	RenderContextGLFW::~RenderContextGLFW()
	{
		glfwTerminate();
	}

	bool RenderContextGLFW::create(unsigned int width,
	                                unsigned int height,
	                                bool fullscreen)
	{
		int mode = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;
		if (glfwOpenWindow(width, height, 8, 8, 8, 8, 24, 8, mode) == GL_FALSE)
			return false;
		// TODO: Input
		return true;
	}

	bool RenderContextGLFW::resize(unsigned int width,
	                                unsigned int height,
	                                bool fullscreen)
	{
		// TODO
		return false;
	}

	void RenderContextGLFW::makeCurrent(bool current)
	{
	}
	void RenderContextGLFW::swapBuffers()
	{
		glfwSwapBuffers();
	}

	void RenderContextGLFW::update()
	{
		glfwPollEvents();
	}
}
}
}
