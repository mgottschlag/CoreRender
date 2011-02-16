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
#include "CoreRender/render/RenderTarget.hpp"

namespace cr
{
namespace render
{
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
				setViewport(command->settarget.viewport[0],
				            command->settarget.viewport[1],
				            command->settarget.viewport[2],
				            command->settarget.viewport[3]);
				break;
			case RenderCommandType::RenderQueue:
				renderQueue(command->renderqueue.queue);
				break;
			case RenderCommandType::BindTextures:
				bindTextures(command->bindtextures.textures,
				            command->bindtextures.texturecount);
				break;
			case RenderCommandType::DrawQuad:
				if (command->drawquad.camera)
					setMatrices(command->drawquad.camera->projmat,
					            command->drawquad.camera->viewmat,
					            command->drawquad.camera->viewer);
				drawQuad(command->drawquad.quad,
				         command->drawquad.shader,
				         command->drawquad.material,
				         command->drawquad.light);
				break;
			default:
				// TODO: Warning here
				break;
		}
	}

	void VideoDriver::renderQueue(RenderQueue *queue)
	{
		// Setup common data
		setMatrices(queue->camera->projmat,
		            queue->camera->viewmat,
		            queue->camera->viewer);
		setLightUniforms(queue->light);
		// Render batches
		for (unsigned int i = 0; i < queue->batches.size(); i++)
		{
			draw(queue->batches[i]);
		}
	}
}
}
