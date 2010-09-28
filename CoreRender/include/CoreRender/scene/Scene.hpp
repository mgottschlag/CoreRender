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

#include "SceneNode.hpp"
#include "ModelSceneNode.hpp"
#include "CameraSceneNode.hpp"
#include "SpotLightSceneNode.hpp"
#include "PointLightSceneNode.hpp"
#include "ModelSceneNode.hpp"

namespace cr
{
namespace render
{
	class GraphicsEngine;
}
namespace scene
{
	class Scene
	{
		public:
			Scene(render::GraphicsEngine *graphics);
			~Scene();

			SceneNode::Ptr addGroupNode(SceneNode::Ptr parent = 0);
			ModelSceneNode::Ptr addModelNode(render::Model::Ptr model,
			                                 SceneNode::Ptr parent = 0);
			ModelSceneNode::Ptr addModelNode(const std::string &model,
			                                 SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNode(render::Pipeline::Ptr pipeline,
			                                   SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNode(const std::string &pipeline,
			                                   SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNodeOrtho(render::Pipeline::Ptr pipeline,
			                                        float left = -1.0f,
			                                        float right = 1.0f,
			                                        float bottom = -1.0f,
			                                        float top = 1.0f,
			                                        float near = -1.0f,
			                                        float far = 1.0f,
			                                        SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNodeOrtho(const std::string &pipeline,
			                                        float left = -1.0f,
			                                        float right = 1.0f,
			                                        float bottom = -1.0f,
			                                        float top = 1.0f,
			                                        float near = -1.0f,
			                                        float far = 1.0f,
			                                        SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNodePerspective(render::Pipeline::Ptr pipeline,
			                                              float fov = 90.0f,
			                                              float aspect = 0.0f,
			                                              float near = 1.0f,
			                                              float far = 1000.0f,
			                                              SceneNode::Ptr parent = 0);
			CameraSceneNode::Ptr addCameraNodePerspective(const std::string &pipeline,
			                                              float fov = 90.0f,
			                                              float aspect = 0.0f,
			                                              float near = 1.0f,
			                                              float far = 1000.0f,
			                                              SceneNode::Ptr parent = 0);
			SpotLightSceneNode::Ptr addSpotLightNode(SceneNode::Ptr parent = 0);
			PointLightSceneNode::Ptr addPointLightNode(SceneNode::Ptr parent = 0);

			SceneNode::Ptr getRootNode()
			{
				return rootnode;
			}

			void activateCamera(CameraSceneNode::Ptr camera,
			                    unsigned int rendertime);
			void deactivateCamera(CameraSceneNode::Ptr camera);

			void render();
		private:
			Scene(const Scene &scene)
			{
			}
			Scene &operator=(const Scene &scene)
			{
				return *this;
			}

			render::GraphicsEngine *graphics;

			std::vector<CameraSceneNode::Ptr> activecameras;

			SceneNode::Ptr rootnode;
	};
}
}

#endif
