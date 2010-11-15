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
#include "CoreRender/core/HashMap.hpp"
#include "CoreRender/render/Material.hpp"

#include <cstring>

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
		std::vector<render::TextureBinding> boundtextures;
		render::TextureBinding *preparedtextures = 0;
		// We need to keep track of the current pipeline state as targets etc
		// are set with a command but actually passed for every queue
		for (unsigned int i = 0; i < pipeline->getStageCount(); i++)
		{
			render::PipelineStage *stage = pipeline->getStage(i);
			for (unsigned int j = 0; j < stage->commands.size(); j++)
			{
				render::PipelineCommand *command = &stage->commands[j];
				if (command->type == render::PipelineCommandType::DrawGeometry)
				{
					prepareTextures(frame, boundtextures, preparedtextures, memory);
					// DrawGeometry commands render using only the camera itself
					queue[queuecount].context = command->uintparams[0];
					queue[queuecount].projmat = camera->getProjMat();
					queue[queuecount].viewmat = camera->getViewMat();
					// TODO: Clipping
					// Add draw command to the queue
					void *ptr = memory->allocate(sizeof(render::RenderCommand));
					render::RenderCommand *cmd = (render::RenderCommand*)ptr;
					cmd->type = render::RenderCommandType::RenderQueue;
					cmd->renderqueue.queue = &queue[queuecount];
					frame->addCommand(cmd);
					queuecount++;
				}
				else if (command->type == render::PipelineCommandType::DoForwardLightLoop)
				{
					prepareTextures(frame, boundtextures, preparedtextures, memory);
					// TODO
					queuecount += shadowqueuecount;
				}
				else if (command->type == render::PipelineCommandType::DoDeferredLightLoop)
				{
					prepareTextures(frame, boundtextures, preparedtextures, memory);
					// TODO
					queuecount += shadowqueuecount;
				}
				else if (command->type == render::PipelineCommandType::ClearTarget)
				{
					void *ptr = memory->allocate(sizeof(render::RenderCommand));
					render::RenderCommand *cmd = (render::RenderCommand*)ptr;
					cmd->type = render::RenderCommandType::ClearTarget;
					cmd->cleartarget.buffers = command->uintparams[0];
					cmd->cleartarget.depth = command->floatparams[0];
					cmd->cleartarget.color[0] = command->floatparams[1];
					cmd->cleartarget.color[1] = command->floatparams[2];
					cmd->cleartarget.color[2] = command->floatparams[3];
					cmd->cleartarget.color[3] = command->floatparams[4];
					frame->addCommand(cmd);
				}
				else if (command->type == render::PipelineCommandType::SetTarget)
				{
					// TODO: Configurable viewport
					render::RenderTargetInfo *targetinfo = 0;
					render::FrameBuffer::Ptr fb;
					if (command->resources[0])
					{
						void *ptr = memory->allocate(sizeof(render::RenderTargetInfo));
						targetinfo = (render::RenderTargetInfo*)ptr;
						render::RenderTarget *newtarget;
						newtarget = (render::RenderTarget*)command->resources[0].get();
						newtarget->getRenderTargetInfo(*targetinfo, memory);
						fb = newtarget->getFrameBuffer();
					}
					else
						targetinfo = 0;
					void *ptr = memory->allocate(sizeof(render::RenderCommand));
					render::RenderCommand *cmd = (render::RenderCommand*)ptr;
					cmd->type = render::RenderCommandType::SetTarget;
					cmd->settarget.target = targetinfo;
					if (!targetinfo)
					{
						// If we render to the final target, use the camera
						// viewport
						unsigned int *viewport = camera->getViewport();
						for (unsigned int k = 0; k < 4; k++)
							cmd->settarget.viewport[k] = viewport[k];
					}
					else
					{
						// If we render to an intermediate buffer, use the
						// whole buffer
						cmd->settarget.viewport[0] = 0;
						cmd->settarget.viewport[1] = 0;
						cmd->settarget.viewport[2] = fb->getWidth();
						cmd->settarget.viewport[3] = fb->getHeight();
					}
					frame->addCommand(cmd);
				}
				else if (command->type == render::PipelineCommandType::BindTexture)
				{
					render::Texture *tex = (render::Texture*)command->resources[0].get();
					//boundtextures.insert(std::make_pair(command->stringparams[0], tex));
					bool found = false;
					for (unsigned int i = 0; i < boundtextures.size(); i++)
					{
						if (command->stringparams[0] == boundtextures[i].name)
						{
							found = true;
							if (tex == 0)
							{
								boundtextures[i] = boundtextures[boundtextures.size() - 1];
								boundtextures.pop_back();
							}
							else
								boundtextures[i].tex = tex;
						}
					}
					if (!found && tex)
					{
						// Allocate memory for the string from the memory pool
						unsigned int namelength = command->stringparams[0].length();
						char *name = (char*)memory->allocate(namelength + 1);
						strcpy(name, command->stringparams[0].c_str());
						render::TextureBinding newentry;
						newentry.tex = tex;
						newentry.name = name;
						boundtextures.push_back(newentry);
					}
					preparedtextures = 0;
				}
				else if (command->type == render::PipelineCommandType::UnbindTextures)
				{
					if (boundtextures.size() > 0)
					{
						boundtextures.clear();
						preparedtextures = 0;
					}
				}
				else if (command->type == render::PipelineCommandType::DrawFullscreenQuad)
				{
					prepareTextures(frame, boundtextures, preparedtextures, memory);
					render::Material *mat = (render::Material*)command->resources[0].get();
					if (!mat)
						continue;
					render::Shader::Ptr shader = mat->getShader();
					if (!shader)
						continue;
					unsigned int context = command->uintparams[0];
					render::ShaderCombination::Ptr comb = shader->getCombination(context,
					                                                             0,
					                                                             0,
					                                                             false,
					                                                             false);
					if (!comb)
						continue;
					void *ptr = memory->allocate(sizeof(render::RenderCommand));
					render::RenderCommand *cmd = (render::RenderCommand*)ptr;
					cmd->type = render::RenderCommandType::DrawQuad;
					cmd->drawquad.quad[0] = cmd->drawquad.quad[1] = -1.0f;
					cmd->drawquad.quad[2] = cmd->drawquad.quad[3] = 1.0f;
					cmd->drawquad.material = mat;
					cmd->drawquad.shader = comb.get();
					frame->addCommand(cmd);
				}
				else
				{
					// TODO: Log
					std::cout << "Warning: Unimplemented pipeline command type "
						<< command->type << std::endl;
				}
			}
		}
		return queuecount;
	}

	void Scene::prepareTextures(render::SceneFrameData *frame,
	                            const std::vector<render::TextureBinding> &textures,
	                            render::TextureBinding *&prepared,
	                            core::MemoryPool *memory)
	{
		if (prepared)
			return;
		if (textures.size() == 0)
			return;
		unsigned int memsize = sizeof(render::TextureBinding) * textures.size();
		prepared = (render::TextureBinding*)memory->allocate(memsize);
		memcpy(prepared, &textures[0], memsize);
		void *ptr = memory->allocate(sizeof(render::RenderCommand));
		render::RenderCommand *cmd = (render::RenderCommand*)ptr;
		cmd->type = render::RenderCommandType::BindTextures;
		cmd->bindtextures.texturecount = textures.size();
		cmd->bindtextures.textures = prepared;
		frame->addCommand(cmd);
		
	}
}
}
