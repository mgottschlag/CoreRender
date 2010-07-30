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

#include "RenderContextGLCML.hpp"

#include <glcml.h>
#include <iostream>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderContextGLCML::RenderContextGLCML() : handle(-1)
	{
	}
	RenderContextGLCML::~RenderContextGLCML()
	{
		if (handle != -1)
		{
			glcml_context_destroy(handle);
		}
	}

	bool RenderContextGLCML::create(unsigned int width,
	                                unsigned int height,
	                                bool fullscreen)
	{
		// Open render window
		window = new RenderWindowGLCML();
		if (!window->open(width, height, fullscreen))
		{
			window = 0;
			return false;
		}
		// Create context
		handle = glcml_context_create(window->getHandle());
		if (handle == -1)
		{
			window = 0;
			return false;
		}
		return true;
	}

	bool RenderContextGLCML::resize(unsigned int width,
	                                unsigned int height,
	                                bool fullscreen)
	{
		return window->resize(width, height, fullscreen);
	}

	void RenderContextGLCML::makeCurrent(bool current)
	{
		glcml_context_set_active(handle, current);
	}
	void RenderContextGLCML::swapBuffers()
	{
		glcml_context_swap_buffers(handle);
	}

	RenderContext::Ptr RenderContextGLCML::clone()
	{
		RenderContextGLCML *context = new RenderContextGLCML();
		context->window = window;
		context->handle = glcml_context_clone(handle);
		if (context->handle == -1)
		{
			delete context;
			return 0;
		}
		return context;
	}

	void RenderContextGLCML::update()
	{
		glcml_window_poll_events();
	}
}
}
}
