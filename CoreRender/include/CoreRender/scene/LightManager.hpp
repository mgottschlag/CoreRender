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

#ifndef _CORERENDER_SCENE_LIGHTMANAGER_HPP_INCLUDED_
#define _CORERENDER_SCENE_LIGHTMANAGER_HPP_INCLUDED_

#include "CameraSceneNode.hpp"

#include <tbb/spin_mutex.h>
#include <vector>

namespace cr
{
namespace scene
{
	class LightSceneNode;

	struct LightLoopInfo
	{
		LightLoopInfo(CameraSceneNode *camera, render::CommandList *commands)
			: camera(camera), commands(commands)
		{
		}
		CameraSceneNode *camera;
		render::CommandList *commands;
		std::vector<CameraConfig*> forwardconfigs;
		std::vector<CameraConfig*> shadowconfigs;
	};

	class LightManager
	{
		public:
			LightManager();
			~LightManager();

			void addLight(LightSceneNode *light);
			void removeLight(LightSceneNode *light);

			/*void addCamera(CameraSceneNode::Ptr camera);
			void removeCamera(CameraSceneNode::Ptr camera);*/

			void addForwardLightLoop(CameraSceneNode *camera, render::CommandList *commands);
			void removeForwardLightLoop(CameraSceneNode *camera, render::CommandList *commands);
			void removeForwardLightLoops(CameraSceneNode *camera);
			void addDeferredLightLoop(CameraSceneNode *camera, render::CommandList *commands);
			void removeDeferredLightLoop(CameraSceneNode *camera, render::CommandList *commands);
			void removeDeferredLightLoops(CameraSceneNode *camera);
		private:
			void addToDeferredLoop(LightSceneNode *light,
			                       LightLoopInfo *loop);
			void addToForwardLoop(LightSceneNode *light,
			                      LightLoopInfo *loop);
			void removeFromForwardLoops(unsigned int lightindex);
			void removeFromDeferredLoops(unsigned int lightindex);

			tbb::spin_mutex mutex;
			/*struct LightInfo
			{
				LightSceneNode *node;
				unsigned int id;
			};
			unsigned int lastid;*/
			std::vector<LightSceneNode*> lights;
			//std::vector<CameraSceneNode::Ptr> cameras;
			std::vector<LightLoopInfo*> forwardloops;
			std::vector<LightLoopInfo*> deferredloops;
	};
}
}

#endif
