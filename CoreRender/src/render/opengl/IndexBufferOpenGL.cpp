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

#include "IndexBufferOpenGL.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	IndexBufferOpenGL::IndexBufferOpenGL(Renderer *renderer,
	                                     res::ResourceManager *rmgr,
	                                     const std::string &name)
		: IndexBuffer(renderer, rmgr, name)
	{
	}
	IndexBufferOpenGL::~IndexBufferOpenGL()
	{
	}

	bool IndexBufferOpenGL::create()
	{
		glGenBuffers(1, &handle);
		// TODO: Error checking
		return true;
	}
	bool IndexBufferOpenGL::destroy()
	{
		glDeleteBuffers(1, &handle);
		return true;
	}
	bool IndexBufferOpenGL::upload()
	{
		// TODO: Type
		tbb::spin_mutex::scoped_lock lock(datamutex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return true;
	}
}
}
}
