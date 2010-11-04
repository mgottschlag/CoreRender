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

#include "VideoDriver.hpp"
#include "CoreRender/render/FrameData.hpp"

namespace cr
{
namespace render
{
	void VideoDriver::bindTexture(const char *sampler, Texture *texture)
	{
		TextureBinding binding;
		binding.sampler = sampler;
		binding.texture = texture;
		boundtextures.push_back(binding);
	}
	void VideoDriver::unbindTextures()
	{
		boundtextures.clear();
	}
	const std::vector<TextureBinding> &VideoDriver::getBoundTextures()
	{
		return boundtextures;
	}

	void VideoDriver::executeCommands(RenderCommand *command)
	{
		while (command)
		{
			executeCommand(command);
			command = command->next;
		}
	}

	void VideoDriver::executeCommand(RenderCommand *command)
	{
		switch (command->type)
		{
			case RenderCommandType::ClearTarget:
				clear(command->cleartarget.buffers,
				      command->cleartarget.color,
				      command->cleartarget.depth);
				break;
			case RenderCommandType::SetTarget:
				setRenderTarget(command->settarget.target);
				break;
			case RenderCommandType::RenderQueue:
				renderQueue(command->renderqueue.queue);
				break;
			case RenderCommandType::BindTexture:
				bindTexture(command->bindtexture.sampler,
				            command->bindtexture.texture);
				break;
			case RenderCommandType::UnbindTextures:
				unbindTextures();
				break;
			case RenderCommandType::DrawQuad:
				// TODO
				break;
		}
	}

	void VideoDriver::renderQueue(RenderQueue *queue)
	{
		// Setup common data
		setRenderTarget(queue->target);
		setViewport(queue->viewport[0],
		            queue->viewport[1],
		            queue->viewport[2],
		            queue->viewport[3]);
		setMatrices(queue->projmat, queue->viewmat);
		// Render batches
		for (unsigned int i = 0; i < queue->batchcount; i++)
		{
			draw(&queue->batches[i]);
		}
	}
}
}
