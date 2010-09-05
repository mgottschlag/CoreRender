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

#include "CoreRender/render/RenderPass.hpp"
#include "CoreRender/render/VideoDriver.hpp"
#include "FrameData.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	RenderPass::RenderPass(const std::string &context)
		: clearcolor(true), cleardepth(true), color(0), depth(1.0f), context(context)
	{
	}
	RenderPass::~RenderPass()
	{
	}

	void RenderPass::setRenderTarget(RenderTarget::Ptr target)
	{
		this->target = target;
	}
	RenderTarget::Ptr RenderPass::getRenderTarget()
	{
		return target;
	}

	void RenderPass::setClear(bool clearcolor,
	                          bool cleardepth,
	                          const core::Color &color,
	                          float depth)
	{
		this->clearcolor = clearcolor;
		this->cleardepth = cleardepth;
		this->depth = depth;
		this->color = color;
	}

	void RenderPass::beginFrame()
	{
	}
	void RenderPass::insert(RenderBatch *batch)
	{
		batches.push_back(batch);
	}
	void RenderPass::prepare(RenderPassInfo *info)
	{
		// Optimize batches
		// TODO
		// Set clear info
		info->clear.clearcolor = clearcolor;
		info->clear.cleardepth = cleardepth;
		info->clear.depth = depth;
		info->clear.color = color.get();
		// Set render target info
		RenderTarget::Ptr target = getRenderTarget();
		if (!target || !target->getFrameBuffer())
		{
			info->target.framebuffer = 0;
			info->target.width = 0;
			info->target.height = 0;
		}
		else
		{
			// Fill in framebuffer info
			FrameBuffer::Ptr framebuffer = target->getFrameBuffer();
			info->target.framebuffer = framebuffer->getHandle();
			info->target.width = framebuffer->getWidth();
			info->target.height = framebuffer->getHeight();
			// Fill in depth buffer info
			if (target->getDepthBuffer())
				info->target.depthbuffer = target->getDepthBuffer()->getHandle();
			else
				info->target.depthbuffer = 0;
			// Fill in color buffer info
			unsigned int colorbuffercount = target->getColorBufferCount();
			info->target.colorbuffercount = colorbuffercount;
			info->target.colorbuffers = new unsigned int[colorbuffercount];
			for (unsigned int i = 0; i < colorbuffercount; i++)
			{
				info->target.colorbuffers[i] = target->getColorBuffer(i)->getHandle();
			}
		}
		// Copy batches for the rendering thread
		info->batches = new RenderBatch*[batches.size()];
		memcpy(info->batches, &batches[0], sizeof(RenderBatch*) * batches.size());
		info->batchcount = batches.size();
		// Clean up
		batches.clear();
	}
}
}
