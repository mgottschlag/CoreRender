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
#include "CoreRender/res/ResourceManager.hpp"

#include <cstring>
#include <iostream>

namespace cr
{
namespace scene
{
	Scene::Scene(res::ResourceManager *rmgr)
	{
		shadowmap = rmgr->createResource<render::Texture>("Texture");
		shadowmap->set2D(512, 512, render::TextureFormat::Depth24);
		shadowmap->setFiltering(render::TextureFiltering::Nearest);
		shadowmap->setMipmapsEnabled(false);
		shadowmap->setDepthCompare(true);
		render::FrameBuffer::Ptr shadowfb;
		shadowfb = rmgr->createResource<render::FrameBuffer>("FrameBuffer");
		shadowfb->setSize(512, 512, false);
		shadowtarget = rmgr->createResource<render::RenderTarget>("RenderTarget");
		shadowtarget->setDepthBuffer(shadowmap);
		shadowtarget->setFrameBuffer(shadowfb);
	}
	Scene::~Scene()
	{
		destroy();
	}

	void Scene::destroy()
	{
		cameras.clear();
		lights.clear();
		shadowmap = 0;
		shadowtarget = 0;
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
		render::RenderQueue *queues = (render::RenderQueue*)ptr;
		for (int i = queuecount - 1; i >= 0; i--)
		{
			new(&queues[i])render::RenderQueue;
			memory->registerDestructor(&queues[i]);
		}
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
		// Only lights which are visible need to be drawn
		std::vector<Light::Ptr> lights;
		clipLights(camera, lights);
		// We need the exact number of shadows/lights to be rendered for
		// allocating render queues
		unsigned int forwardlightcount, forwardshadowcount;
		getForwardLightCount(camera,
		                     lights,
		                     forwardlightcount,
		                     forwardshadowcount);
		unsigned int deferredlightcount, deferredshadowcount;
		getDeferredLightCount(camera,
		                      lights,
		                      deferredlightcount,
		                      deferredshadowcount);
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
				{
					queuecount += forwardshadowcount;
					queuecount += forwardlightcount;
				}
				else if (command->type == render::PipelineCommandType::DoDeferredLightLoop)
					queuecount += deferredshadowcount;
			}
		}
		return queuecount;
	}
	void Scene::getForwardLightCount(Camera::Ptr camera,
	                                 std::vector<Light::Ptr> &lights,
	                                 unsigned int &lightcount,
	                                 unsigned int &shadowcount)
	{
		lightcount = 0;
		shadowcount = 0;
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			if (lights[i]->getLightContext() == -1)
				continue;
			lightcount++;
			if (lights[i]->getShadowsEnabled()
				&& lights[i]->getShadowContext() != -1)
				shadowcount++;
		}
	}
	void Scene::getDeferredLightCount(Camera::Ptr camera,
	                                  std::vector<Light::Ptr> &lights,
	                                  unsigned int &lightcount,
	                                  unsigned int &shadowcount)
	{
		lightcount = 0;
		shadowcount = 0;
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			if (!lights[i]->getDeferredMaterial())
				continue;
			lightcount++;
			if (lights[i]->getShadowsEnabled()
				&& lights[i]->getShadowContext() != -1)
				shadowcount++;
		}
	}
	void Scene::clipLights(Camera::Ptr camera,
	                       std::vector<Light::Ptr> &visible)
	{
		// TODO
		visible = lights;
	}
	unsigned int Scene::beginFrame(render::SceneFrameData *frame,
	                               render::RenderQueue *queue,
	                               Camera::Ptr camera)
	{
		render::Pipeline::Ptr pipeline = camera->getPipeline();
		if (!pipeline)
			return 0;
		core::MemoryPool *memory = queue->memory;
		// Initialize camera uniforms
		void *ptr = memory->allocate(sizeof(render::CameraUniforms));
		render::CameraUniforms *camerauniforms = (render::CameraUniforms*)ptr;
		camerauniforms->projmat = camera->getProjMat();
		camerauniforms->viewmat = camera->getViewMat();
		camerauniforms->viewer = camera->getViewMat().inverse().transformPoint(math::Vec3f(0, 0, 0));
		// Only lights which are visible need to be drawn
		std::vector<Light::Ptr> lights;
		clipLights(camera, lights);
		unsigned int queuecount = 0;
		std::vector<render::TextureBinding> boundtextures;
		render::TextureBinding *preparedtextures = 0;
		render::RenderTarget::Ptr currenttarget = 0;
		insertSetTarget(frame, currenttarget, camera, memory);
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
					queue[queuecount].camera = camerauniforms;
					queue[queuecount].light = 0;
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
					// Render all lights one by one (forward lighting)
					for (unsigned int i = 0; i < lights.size(); i++)
					{
						if (lights[i]->getLightContext() == -1)
							continue;
						void *ptr = memory->allocate(sizeof(render::LightUniforms));
						render::LightUniforms *light = (render::LightUniforms*)ptr;
						light->shadowmap = 0;
						lights[i]->getLightInfo(light);
						math::Mat4f shadowmat;
						if (lights[i]->getShadowsEnabled()
							&& lights[i]->getShadowContext() != -1)
						{
							light->shadowmap = shadowmap.get();
							unsigned int shadowmapcount = lights[i]->getShadowMapCount();
							insertSetTarget(frame, shadowtarget, camera, memory);
							void *ptr = memory->allocate(sizeof(render::RenderCommand));
							render::RenderCommand *cmd = (render::RenderCommand*)ptr;
							cmd->type = render::RenderCommandType::ClearTarget;
							cmd->cleartarget.buffers = 1;
							cmd->cleartarget.depth = 1.0f;
							frame->addCommand(cmd);
							lights[i]->prepareShadowMaps(frame,
							                             &queue[queuecount],
							                             camera,
							                             &shadowmat,
							                             light);
							insertSetTarget(frame, currenttarget, camera, memory);
							queuecount += shadowmapcount;
							// The shadow matrix has to be modified to output
							// texture coordinates (0..1 instead of -1..1)
							shadowmat = math::Mat4f::TransMat(0.5f, 0.5f, 0.0f)
							          * math::Mat4f::ScaleMat(0.5f, 0.5f, 1.0f)
							          * shadowmat;
						}
						light->shadowmat = shadowmat;
						// Light pass using the current camera and the light
						// context
						queue[queuecount].context = lights[i]->getLightContext();
						queue[queuecount].camera = camerauniforms;
						queue[queuecount].light = light;
						// TODO: Clipping
						// Add draw command to the queue
						ptr = memory->allocate(sizeof(render::RenderCommand));
						render::RenderCommand *cmd = (render::RenderCommand*)ptr;
						cmd->type = render::RenderCommandType::RenderQueue;
						cmd->renderqueue.queue = &queue[queuecount];
						frame->addCommand(cmd);
						queuecount++;
					}
				}
				else if (command->type == render::PipelineCommandType::DoDeferredLightLoop)
				{
					prepareTextures(frame, boundtextures, preparedtextures, memory);
					// Render all lights one by one (forward lighting)
					for (unsigned int i = 0; i < lights.size(); i++)
					{
						if (!lights[i]->getDeferredMaterial())
							continue;
						void *ptr = memory->allocate(sizeof(render::LightUniforms));
						render::LightUniforms *light = (render::LightUniforms*)ptr;
						light->shadowmap = 0;
						lights[i]->getLightInfo(light);
						math::Mat4f shadowmat;
						if (lights[i]->getShadowsEnabled()
							&& lights[i]->getShadowContext() != -1)
						{
							light->shadowmap = shadowmap.get();
							unsigned int shadowmapcount = lights[i]->getShadowMapCount();
							insertSetTarget(frame, shadowtarget, camera, memory);
							void *ptr = memory->allocate(sizeof(render::RenderCommand));
							render::RenderCommand *cmd = (render::RenderCommand*)ptr;
							cmd->type = render::RenderCommandType::ClearTarget;
							cmd->cleartarget.buffers = 1;
							cmd->cleartarget.depth = 1.0f;
							frame->addCommand(cmd);
							lights[i]->prepareShadowMaps(frame,
							                             &queue[queuecount],
							                             camera,
							                             &shadowmat,
							                             light);
							insertSetTarget(frame, currenttarget, camera, memory);
							queuecount += shadowmapcount;
							// The shadow matrix has to be modified to output
							// texture coordinates (0..1 instead of -1..1)
							shadowmat = math::Mat4f::TransMat(0.5f, 0.5f, 0.0f)
							          * math::Mat4f::ScaleMat(0.5f, 0.5f, 1.0f)
							          * shadowmat;
						}
						light->shadowmat = shadowmat;
						render::Material::Ptr material = lights[i]->getDeferredMaterial();
						render::Shader::Ptr shader = material->getShader();
						unsigned int context = lights[i]->getLightContext();
						render::ShaderCombination::Ptr comb = shader->getCombination(context,
						                                                             0,
						                                                             0,
						                                                             false,
						                                                             false);
						if (!comb)
							continue;

						// Light quad
						ptr = memory->allocate(sizeof(render::RenderCommand));
						render::RenderCommand *cmd = (render::RenderCommand*)ptr;
						cmd->type = render::RenderCommandType::DrawQuad;
						lights[i]->getLightQuad(camera, cmd->drawquad.quad);
						cmd->drawquad.material = material.get();
						cmd->drawquad.shader = comb.get();
						cmd->drawquad.camera = camerauniforms;
						cmd->drawquad.light = light;
						frame->addCommand(cmd);
					}
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
					currenttarget = (render::RenderTarget*)command->resources[0].get();
					insertSetTarget(frame, currenttarget, camera, memory);
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
					cmd->drawquad.camera = camerauniforms;
					cmd->drawquad.light = 0;
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

	void Scene::insertSetTarget(render::SceneFrameData *frame,
	                            render::RenderTarget::Ptr target,
	                            Camera::Ptr camera,
	                            core::MemoryPool *memory)
	{
		// If we do not have any target, use the camera default target
		bool defaulttarget = false;
		if (!target)
		{
			target = camera->getTarget();
			defaulttarget = true;
		}
		render::RenderTargetInfo *targetinfo = 0;
		render::FrameBuffer::Ptr fb;
		if (target)
		{
			void *ptr = memory->allocate(sizeof(render::RenderTargetInfo));
			targetinfo = (render::RenderTargetInfo*)ptr;
			target->getRenderTargetInfo(*targetinfo, memory);
			fb = target->getFrameBuffer();
		}
		else
			targetinfo = 0;
		void *ptr = memory->allocate(sizeof(render::RenderCommand));
		render::RenderCommand *cmd = (render::RenderCommand*)ptr;
		cmd->type = render::RenderCommandType::SetTarget;
		cmd->settarget.target = targetinfo;
		// If we use the camera target, we have to use the camera viewport
		if (!targetinfo || defaulttarget)
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
}
}
