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
#include <cstdlib>

namespace cr
{
namespace render
{
namespace opengl
{
	IndexBufferOpenGL::IndexBufferOpenGL(UploadManager &uploadmgr,
	                                     res::ResourceManager *rmgr,
	                                     const std::string &name)
		: IndexBuffer(uploadmgr, rmgr, name)
	{
	}
	IndexBufferOpenGL::~IndexBufferOpenGL()
	{
		if (handle)
			glDeleteBuffers(1, &handle);
	}

	void IndexBufferOpenGL::upload(void *data)
	{
		BufferData *uploaddata = (BufferData*)data;
		// Create buffer object if necessary
		if (!handle)
		{
			glGenBuffers(1, &handle);
			// TODO: Error checking
		}
		// Upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, uploaddata->size, uploaddata->data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		free(uploaddata->data);
		delete uploaddata;
	}
}
}
}
