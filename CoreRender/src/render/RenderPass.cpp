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

#include <cstring>

namespace cr
{
namespace render
{
	RenderPass::RenderPass(const std::string &context)
		: context(context)
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
		// Copy batches for the rendering thread
		info->target = 0;
		//if (target)
		//	info->target = target->getHandle();
		info->batches = new RenderBatch*[batches.size()];
		memcpy(info->batches, &batches[0], sizeof(RenderBatch*) * batches.size());
		info->batchcount = batches.size();
		// Clean up
		batches.clear();
	}
	void RenderPass::render(VideoDriver *driver)
	{
		/*// Set render target
		driver->setRenderTarget(target);
		// Clear background
		// TODO
		driver->clear(true, true);
		// Draw batches
		for (unsigned int i = 0; i < preparedcount; i++)
		{
			driver->draw(prepared[i]);
		}
		// Clean up
		// TODO: Not necessary with proper memory manager
		delete[] prepared;
		prepared = 0;
		preparedcount = 0;*/
	}
}
}
