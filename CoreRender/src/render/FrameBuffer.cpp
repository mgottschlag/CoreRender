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

#include "CoreRender/render/FrameBuffer.hpp"

namespace cr
{
namespace render
{
	FrameBuffer::FrameBuffer(UploadManager &uploadmgr,
	                         res::ResourceManager *rmgr,
	                         const std::string &name)
		: RenderResource(uploadmgr, rmgr, name)
	{
		currentdata.width = 0;
		currentdata.height = 0;
		currentdata.depthbuffer = false;
		config.handle = 0;
		config.depthbuffer = 0;
		config.defaultdepthbuffer = 0;
	}
	FrameBuffer::~FrameBuffer()
	{
	}

	void FrameBuffer::setSize(unsigned int width,
	                          unsigned int height,
	                          bool depthbuffer)
	{
		currentdata.width = width;
		currentdata.height = height;
		currentdata.depthbuffer = depthbuffer;
		registerUpload();
	}
	unsigned int FrameBuffer::getWidth()
	{
		return currentdata.width;
	}
	unsigned int FrameBuffer::getHeight()
	{
		return currentdata.height;
	}
	bool FrameBuffer::hasDepthBuffer()
	{
		return currentdata.depthbuffer;
	}

	void *FrameBuffer::getUploadData()
	{
		return new FrameBufferData(currentdata);
	}
}
}
