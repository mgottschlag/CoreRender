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

#include "CoreRender/scene/LightManager.hpp"
#include <CoreRender/scene/LightSceneNode.hpp>

namespace cr
{
namespace scene
{
	LightManager::LightManager()
	{
	}
	LightManager::~LightManager()
	{
	}

	void LightManager::addLight(LightSceneNode *light)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		lights.push_back(light);
		// Add light to all light loops
		for (unsigned int i = 0; i < forwardloops.size(); i++)
			addToForwardLoop(light, forwardloops[i]);
		for (unsigned int i = 0; i < deferredloops.size(); i++)
			addToDeferredLoop(light, deferredloops[i]);
	}
	void LightManager::removeLight(LightSceneNode *light)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			if (lights[i] == light)
			{
				// Remove light from all light loops
				removeFromForwardLoops(i);
				removeFromDeferredLoops(i);
				// Remove light from the light list
				lights.erase(lights.begin() + i);
				break;
			}
		}
	}

	/*void LightManager::addCamera(CameraSceneNode::Ptr camera)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		cameras.push_back(camera);
		// TODO: Add all lights
	}
	void LightManager::removeCamera(CameraSceneNode::Ptr camera)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < cameras.size(); i++)
		{
			if (cameras[i] == camera)
			{
				// Remove all lights from the camera
				// TODO
				// Remove camera from the list
				cameras.erase(cameras.begin() + i);
				break;
			}
		}
	}*/

	void LightManager::addForwardLightLoop(CameraSceneNode *camera, render::CommandList *commands)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		// Add to light loop list
		LightLoopInfo *loop = new LightLoopInfo(camera, commands);
		forwardloops.push_back(loop);
		// Rebuild loop
		for (unsigned int i = 0; i < lights.size(); i++)
			addToForwardLoop(lights[i], loop);
	}
	void LightManager::removeForwardLightLoop(CameraSceneNode *camera, render::CommandList *commands)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < forwardloops.size(); i++)
		{
			if (forwardloops[i]->commands == commands)
			{
				// Remove all lights from this loop
				for (int j = lights.size() - 1; j >= 0; j--)
				{
					lights[j]->removeFromForwardLightLoop(forwardloops[i], j);
				}
				// Remove loop
				delete forwardloops[i];
				forwardloops[i] = forwardloops[forwardloops.size() - 1];
				forwardloops.pop_back();
				break;
			}
		}
	}
	void LightManager::removeForwardLightLoops(CameraSceneNode *camera)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (int i = forwardloops.size() - 1; i >= 0; i--)
		{
			if (forwardloops[i]->camera == camera)
			{
				//commands->removeAllCommands();
				// TODO
				delete forwardloops[i];
				forwardloops[i] = forwardloops[forwardloops.size() - 1];
				forwardloops.pop_back();
				break;
			}
		}
	}
	void LightManager::addDeferredLightLoop(CameraSceneNode *camera, render::CommandList *commands)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		// Add to light loop list
		LightLoopInfo *loop = new LightLoopInfo(camera, commands);
		deferredloops.push_back(loop);
		// Rebuild loop
		for (unsigned int i = 0; i < lights.size(); i++)
			addToDeferredLoop(lights[i], loop);
	}
	void LightManager::removeDeferredLightLoop(CameraSceneNode *camera, render::CommandList *commands)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < deferredloops.size(); i++)
		{
			if (deferredloops[i]->commands == commands)
			{
				// Remove all lights from this loop
				for (int j = lights.size() - 1; j >= 0; j--)
				{
					lights[j]->removeFromDeferredLightLoop(forwardloops[i], j);
				}
				// Remove loop
				delete deferredloops[i];
				deferredloops[i] = deferredloops[deferredloops.size() - 1];
				deferredloops.pop_back();
				break;
			}
		}
	}
	void LightManager::removeDeferredLightLoops(CameraSceneNode *camera)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (int i = deferredloops.size() - 1; i >= 0; i--)
		{
			if (deferredloops[i]->camera == camera)
			{
				//commands->removeAllCommands();
				// TODO
				delete deferredloops[i];
				deferredloops[i] = deferredloops[deferredloops.size() - 1];
				deferredloops.pop_back();
				break;
			}
		}
	}

	void LightManager::addToForwardLoop(LightSceneNode *light,
	                                    LightLoopInfo *loop)
	{
		// TODO: Erase these functions?
		light->addToForwardLightLoop(loop);
	}
	void LightManager::addToDeferredLoop(LightSceneNode *light,
	                                     LightLoopInfo *loop)
	{
		light->addToDeferredLightLoop(loop);
	}

	void LightManager::removeFromForwardLoops(unsigned int lightindex)
	{
		for (unsigned int i = 0; i < forwardloops.size(); i++)
		{
			lights[lightindex]->removeFromDeferredLightLoop(forwardloops[i], lightindex);
		}
	}
	void LightManager::removeFromDeferredLoops(unsigned int lightindex)
	{
		for (unsigned int i = 0; i < deferredloops.size(); i++)
		{
			lights[lightindex]->removeFromForwardLightLoop(forwardloops[i], lightindex);
		}
	}
}
}
