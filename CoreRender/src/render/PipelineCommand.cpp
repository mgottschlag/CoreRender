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

#include "CoreRender/render/PipelineCommand.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "FrameData.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	void ClearCommand::apply(Renderer *renderer, PipelineCommandInfo *command)
	{
		// Allocate clear info
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		ClearInfo *info = (ClearInfo*)memory->allocate(sizeof(ClearInfo));
		info->depth = depth;
		info->depthbuffer = cleardepth;
		info->colorbuffercount = color.size();
		unsigned int memsize = color.size() * sizeof(ColorBufferClearInfo);
		info->colorbuffers = (ColorBufferClearInfo*)memory->allocate(memsize);
		for (unsigned int i = 0; i < color.size(); i++)
		{
			info->colorbuffers[i].index = color[i].index;
			info->colorbuffers[i].color[0] = color[i].color[0];
			info->colorbuffers[i].color[1] = color[i].color[1];
			info->colorbuffers[i].color[2] = color[i].color[2];
			info->colorbuffers[i].color[3] = color[i].color[3];
		}
		// Fill in command
		command->type = getType();
		command->clear = info;
	}

	void SetTargetCommand::apply(Renderer *renderer, PipelineCommandInfo *command)
	{
		if (!target)
		{
			command->target = 0;
			command->type = getType();
			return;
		}
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		command->target = (RenderTargetInfo*)memory->allocate(sizeof(RenderTargetInfo));
		FrameBuffer::Ptr framebuffer = target->getFrameBuffer();
		command->target->framebuffer = &framebuffer->getConfiguration();
		command->target->width = framebuffer->getWidth();
		command->target->height = framebuffer->getHeight();
		// Fill in depth buffer info
		if (target->getDepthBuffer())
			command->target->depthbuffer = target->getDepthBuffer()->getHandle();
		else
			command->target->depthbuffer = 0;
		// Fill in color buffer info
		unsigned int colorbuffercount = target->getColorBufferCount();
		command->target->colorbuffercount = colorbuffercount;
		unsigned int memsize = sizeof(unsigned int) * colorbuffercount;
		command->target->colorbuffers = (unsigned int*)memory->allocate(memsize);
		for (unsigned int i = 0; i < colorbuffercount; i++)
		{
			command->target->colorbuffers[i] = target->getColorBuffer(i)->getHandle();
		}
		// Fill in general command info
		command->type = getType();
	}

	void BatchListCommand::submit(RenderBatch *batch)
	{
		batches.push_back(batch);
	}

	void BatchListCommand::finish()
	{
		// Sort batches
		// TODO
		// Allocate memory for the batches
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		info->batchlist = (BatchList*)memory->allocate(sizeof(BatchList));
		unsigned int memsize = batches.size() * sizeof(RenderBatch*);
		info->batchlist->batchcount = batches.size();
		info->batchlist->batches = (RenderBatch**)memory->allocate(memsize);
		// Copy batches
		memcpy(info->batchlist->batches, &batches[0], memsize);
		// Clear temporary batch list
		batches.clear();
	}

	void BatchListCommand::apply(Renderer *renderer, PipelineCommandInfo *command)
	{
		command->type = getType();
		info = command;
		this->renderer = renderer;
	}
}
}
