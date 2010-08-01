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

#include "RenderWindowGLCML.hpp"

#include <glcml.h>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderWindowGLCML::RenderWindowGLCML() : handle(-1)
	{
		// It is safe to call init/terminate here even if other windows exist
		// as glcml uses reference counting here
		glcml_init();
	}
	RenderWindowGLCML::~RenderWindowGLCML()
	{
		if (handle != -1)
		{
			// Close window again
			glcml_window_destroy(handle);
		}
		glcml_terminate();
	}

	bool RenderWindowGLCML::open(unsigned int width,
	                             unsigned int height,
	                             bool fullscreen)
	{
		// Create a 32bit render window
		handle = glcml_window_create(width,
		                             height,
		                             8,
		                             8,
		                             8,
		                             8,
		                             24,
		                             8,
		                             fullscreen,
		                             0);
		if (handle == -1)
			return false;
		return true;
	}
	bool RenderWindowGLCML::resize(unsigned int width,
	                               unsigned int height,
	                               bool fullscreen)
	{
		// TODO
		return false;
	}
}
}
}
