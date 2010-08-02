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

#include "CoreRender/render/VertexBuffer.hpp"

#include <cstring>
#include <cstdlib>

namespace cr
{
namespace render
{
	VertexBuffer::VertexBuffer(Renderer *renderer,
	             res::ResourceManager *rmgr,
	             const std::string &name,
	             VertexBufferUsage::List usage)
		: RenderResource(renderer, rmgr, name), usage(usage), size(0), data(0)
	{
	}
	VertexBuffer::~VertexBuffer()
	{
		if (data)
			free(data);
	}

	void VertexBuffer::set(unsigned int size,
	                       void *data,
	                       bool copy)
	{
		// Copy the data
		void *datacopy;
		if (copy)
		{
			datacopy = malloc(size);
			memcpy(datacopy, data, size);
		}
		else
			datacopy = data;
		void *prevdata = 0;
		// Fill in info
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			prevdata = this->data;
			this->size = size;
			this->data = datacopy;
		}
		// Delete old data
		if (prevdata)
			free(prevdata);
		// Register for uploading
		registerUpload();
	}
	void VertexBuffer::update(unsigned int offset,
	                          unsigned int size,
	                          const void *data)
	{
		// TODO
	}
	void VertexBuffer::discardData()
	{
		// TODO
	}
}
}
