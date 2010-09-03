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

#include "CoreRender/render/RenderTarget.hpp"

namespace cr
{
namespace render
{
	RenderTarget::RenderTarget(Renderer *renderer,
	                           res::ResourceManager *rmgr,
	                           const std::string &name)
		: RenderResource(renderer, rmgr, name)
	{
	}
	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::setFrameBuffer(FrameBuffer::Ptr framebuffer)
	{
		this->framebuffer = framebuffer;
	}
	FrameBuffer::Ptr RenderTarget::getFrameBuffer()
	{
		return framebuffer;
	}

	void RenderTarget::setDepthBuffer(Texture2D::Ptr texture)
	{
		depthbuffer = texture;
	}
	Texture::Ptr RenderTarget::getDepthBuffer()
	{
		return depthbuffer;
	}

	void RenderTarget::addColorBuffer(Texture2D::Ptr texture)
	{
		colorbuffers.push_back(texture);
	}
	Texture2D::Ptr RenderTarget::getColorBuffer(unsigned int index)
	{
		if (index >= colorbuffers.size())
			return 0;
		return colorbuffers[index];
	}
	void RenderTarget::removeColorBuffer(unsigned int index)
	{
		if (index >= colorbuffers.size())
			return;
		colorbuffers.erase(colorbuffers.begin() + index);
	}
	unsigned int RenderTarget::getColorBufferCount()
	{
		return colorbuffers.size();
	}
}
}
