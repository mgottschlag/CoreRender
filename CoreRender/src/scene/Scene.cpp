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

#include "CoreRender/scene/Scene.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/core/MemoryPool.hpp"

namespace cr
{
namespace scene
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}

	void Scene::addCamera(Camera::Ptr camera)
	{
		tbb::mutex::scoped_lock lock(cameramutex);
		cameras.push_back(camera);
	}
	void Scene::removeCamera(Camera::Ptr camera)
	{
		tbb::mutex::scoped_lock lock(cameramutex);
		for (unsigned int i = 0; i < cameras.size(); i++)
		{
			if (cameras[i] == camera)
			{
				cameras.erase(cameras.begin() + i);
				return;
			}
		}
	}
	void Scene::addLight(Light::Ptr light)
	{
		tbb::mutex::scoped_lock lock(lightmutex);
		lights.push_back(light);
	}
	void Scene::removeLight(Light::Ptr light)
	{
		tbb::mutex::scoped_lock lock(lightmutex);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			if (lights[i] == light)
			{
				lights.erase(lights.begin() + i);
				return;
			}
		}
	}

	render::SceneFrameData *Scene::beginFrame(render::FrameData *frame)
	{
		// Compute the count of render queues
		// We have to do this here before allocating the render queue list
		unsigned int queuecount = 0;
		for (unsigned int i = 0; i < cameras.size(); i++)
		{
			queuecount += getRenderQueueCount(cameras[i]);
		}
		// Create render queues
		core::MemoryPool *memory = frame->getMemory();
		void *ptr = memory->allocate(sizeof(render::RenderQueue) * queuecount);
		render::RenderQueue *queues = new(ptr) render::RenderQueue[queuecount];
		for (int i = queuecount - 1; i >= 0; i--)
			memory->registerDestructor(&queues[i]);
		for (unsigned int i = 0; i < queuecount; i++)
			queues[i].memory = memory;
		// Create scene frame data
		void *allocated = memory->allocate(sizeof(render::SceneFrameData));
		render::SceneFrameData *framedata = new(allocated) render::SceneFrameData(queues, queuecount);
		// Initialize queues and pipeline commands
		unsigned int currentqueue = 0;
		for (unsigned int i = 0; i < cameras.size(); i++)
		{
			currentqueue += beginFrame(framedata, &queues[currentqueue], cameras[i]);
		}
		frame->addScene(framedata);
		return framedata;
	}

	unsigned int Scene::getRenderQueueCount(Camera::Ptr camera)
	{
		render::Pipeline::Ptr pipeline = camera->getPipeline();
		if (!pipeline)
			return 0;
		unsigned int shadowqueuecount = getShadowRenderQueueCount(camera);
		unsigned int queuecount = 0;
		for (unsigned int i = 0; i < pipeline->getStageCount(); i++)
		{
			render::PipelineStage *stage = pipeline->getStage(i);
			for (unsigned int j = 0; j < stage->commands.size(); j++)
			{
				render::PipelineCommand *command = &stage->commands[j];
				if (command->type == render::PipelineCommandType::DrawGeometry)
					queuecount++;
				else if (command->type == render::PipelineCommandType::DoForwardLightLoop)
					queuecount += shadowqueuecount;
				else if (command->type == render::PipelineCommandType::DoDeferredLightLoop)
					queuecount += shadowqueuecount;
			}
		}
		return queuecount;
	}
	unsigned int Scene::getShadowRenderQueueCount(Camera::Ptr camera)
	{
		// TODO
		return 0;
	}
	unsigned int Scene::beginFrame(render::SceneFrameData *frame,
	                               render::RenderQueue *queue,
	                               Camera::Ptr camera)
	{
		render::Pipeline::Ptr pipeline = camera->getPipeline();
		if (!pipeline)
			return 0;
		core::MemoryPool *memory = queue->memory;
		unsigned int shadowqueuecount = getShadowRenderQueueCount(camera);
		unsigned int queuecount = 0;
		for (unsigned int i = 0; i < pipeline->getStageCount(); i++)
		{
			render::PipelineStage *stage = pipeline->getStage(i);
			for (unsigned int j = 0; j < stage->commands.size(); j++)
			{
				render::PipelineCommand *command = &stage->commands[j];
				if (command->type == render::PipelineCommandType::DrawGeometry)
				{
					// DrawGeometry commands render using only the camera itself
					queue[queuecount].context = command->uintparams[0];
					queue[queuecount].projmat = camera->getProjMat();
					queue[queuecount].viewmat = camera->getViewMat();
					// TODO: Clipping
					unsigned int *viewport = camera->getViewport();
					for (unsigned int k = 0; k < 4; k++)
						queue[queuecount].viewport[k] = viewport[k];
					// Add draw command to the queue
					void *ptr = memory->allocate(sizeof(render::RenderCommand));
					render::RenderCommand *cmd = (render::RenderCommand*)ptr;
					cmd->type = render::RenderCommandType::RenderQueue;
					cmd->renderqueue.queue = &queue[queuecount];
					queuecount++;
				}
				else if (command->type == render::PipelineCommandType::DoForwardLightLoop)
				{
					// TODO
					queuecount += shadowqueuecount;
				}
				else if (command->type == render::PipelineCommandType::DoDeferredLightLoop)
				{
					// TODO
					queuecount += shadowqueuecount;
				}
			}
		}
		return queuecount;
	}
}
}
