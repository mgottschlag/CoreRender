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

#ifndef _CORERENDER_SCENE_SCENE_HPP_INCLUDED_
#define _CORERENDER_SCENE_SCENE_HPP_INCLUDED_

#include "Camera.hpp"
#include "Light.hpp"

namespace cr
{
class GraphicsEngine;
namespace render
{
	class SceneFrameData;
	class FrameData;
	struct RenderQueue;
	struct TextureBinding;
}
namespace scene
{
	class Scene
	{
		public:
			Scene(res::ResourceManager *rmgr);
			~Scene();

			void destroy();

			void addCamera(Camera::Ptr camera);
			void removeCamera(Camera::Ptr camera);
			void addLight(Light::Ptr light);
			void removeLight(Light::Ptr light);

			render::SceneFrameData *beginFrame(render::FrameData *frame);
		private:
			unsigned int getRenderQueueCount(Camera::Ptr camera);
			void getForwardLightCount(Camera::Ptr camera,
			                          std::vector<Light::Ptr> &lights,
			                          unsigned int &lightcount,
			                          unsigned int &shadowcount);
			void getDeferredLightCount(Camera::Ptr camera,
			                           std::vector<Light::Ptr> &lights,
			                           unsigned int &lightcount,
			                           unsigned int &shadowcount);
			void clipLights(Camera::Ptr camera,
			                std::vector<Light::Ptr> &visible);
			unsigned int beginFrame(render::SceneFrameData *frame,
			                        render::RenderQueue *queue,
			                        Camera::Ptr camera);

			static void prepareTextures(render::SceneFrameData *frame,
			                            const std::vector<render::TextureBinding> &textures,
			                            render::TextureBinding *&prepared,
			                            core::MemoryPool *memory);

			void insertSetTarget(render::SceneFrameData *frame,
			                     render::RenderTarget::Ptr target,
			                     Camera::Ptr camera,
			                     core::MemoryPool *memory);

			tbb::mutex cameramutex;
			std::vector<Camera::Ptr> cameras;
			tbb::mutex lightmutex;
			std::vector<Light::Ptr> lights;

			res::ResourceManager *rmgr;

			render::Texture2D::Ptr shadowmap;
			render::RenderTarget::Ptr shadowtarget;
	};
}
}

#endif
