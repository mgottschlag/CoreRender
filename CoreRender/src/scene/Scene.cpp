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
#include "CoreRender/render/GraphicsEngine.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/PipelineDefinition.hpp"

namespace cr
{
namespace scene
{
	Scene::Scene(render::GraphicsEngine *graphics)
		: graphics(graphics)
	{
		rootnode = new SceneNode;
	}
	Scene::~Scene()
	{
		activecameras.clear();
	}

	SceneNode::Ptr Scene::addGroupNode(SceneNode::Ptr parent)
	{
		if (!parent)
			parent = getRootNode();
		SceneNode::Ptr newnode = new SceneNode;
		newnode->setParent(parent);
		return newnode;
	}
	ModelSceneNode::Ptr Scene::addModelNode(render::Model::Ptr model,
	                                        SceneNode::Ptr parent)
	{
		if (!parent)
			parent = getRootNode();
		ModelSceneNode::Ptr newnode = new ModelSceneNode(model);
		newnode->setParent(parent);
		return newnode;
	}
	ModelSceneNode::Ptr Scene::addModelNode(const std::string &model,
	                                        SceneNode::Ptr parent)
	{
		res::ResourceManager *rmgr = graphics->getResourceManager();
		render::Model::Ptr res = rmgr->getOrLoad<render::Model>("Model",
		                                                        model);
		// TODO: Wait somewhere else?
		res->waitForLoading(true);
		return addModelNode(res, parent);
	}
	CameraSceneNode::Ptr Scene::addCameraNode(render::Pipeline::Ptr pipeline,
	                                          SceneNode::Ptr parent)
	{
		if (!parent)
			parent = getRootNode();
		CameraSceneNode::Ptr newnode = new CameraSceneNode(pipeline);
		newnode->setParent(parent);
		return newnode;
	}
	CameraSceneNode::Ptr Scene::addCameraNode(const std::string &pipeline,
	                                          SceneNode::Ptr parent)
	{
		res::ResourceManager *rmgr = graphics->getResourceManager();
		render::PipelineDefinition::Ptr res;
		res = rmgr->getOrLoad<render::PipelineDefinition>("PipelineDefinition",
		                                                  pipeline);
		res->waitForLoading(true);
		// TODO: Screen size
		return addCameraNode(res->createPipeline(512, 512), parent);
	}
	CameraSceneNode::Ptr Scene::addCameraNodeOrtho(render::Pipeline::Ptr pipeline,
	                                               float left,
	                                               float right,
	                                               float bottom,
	                                               float top,
	                                               float near,
	                                               float far,
	                                               SceneNode::Ptr parent)
	{
		CameraSceneNode::Ptr camera = addCameraNode(pipeline, parent);
		camera->setOrtho(left, right, bottom, top, near, far);
		return camera;
	}
	CameraSceneNode::Ptr Scene::addCameraNodeOrtho(const std::string &pipeline,
	                                               float left,
	                                               float right,
	                                               float bottom,
	                                               float top,
	                                               float near,
	                                               float far,
	                                               SceneNode::Ptr parent)
	{
		CameraSceneNode::Ptr camera = addCameraNode(pipeline, parent);
		camera->setOrtho(left, right, bottom, top, near, far);
		return camera;
	}
	CameraSceneNode::Ptr Scene::addCameraNodePerspective(render::Pipeline::Ptr pipeline,
	                                                     float fov,
	                                                     float aspect,
	                                                     float near,
	                                                     float far,
	                                                     SceneNode::Ptr parent)
	{
		CameraSceneNode::Ptr camera = addCameraNode(pipeline, parent);
		if (aspect == 0.0f)
			aspect = 1.0f;
		camera->setPerspectiveFOV(fov, aspect, near, far);
		return camera;
	}
	CameraSceneNode::Ptr Scene::addCameraNodePerspective(const std::string &pipeline,
	                                                     float fov,
	                                                     float aspect,
	                                                     float near,
	                                                     float far,
	                                                     SceneNode::Ptr parent)
	{
		CameraSceneNode::Ptr camera = addCameraNode(pipeline, parent);
		if (aspect == 0.0f)
			aspect = 1.0f;
		camera->setPerspectiveFOV(fov, aspect, near, far);
		return camera;
	}
	SpotLightSceneNode::Ptr Scene::addSpotLightNode(SceneNode::Ptr parent)
	{
	}
	PointLightSceneNode::Ptr Scene::addPointLightNode(SceneNode::Ptr parent)
	{
	}

	void Scene::activateCamera(CameraSceneNode::Ptr camera,
	                           unsigned int rendertime)
	{
		// TODO: Sorting based on rendertime
		activecameras.push_back(camera);
		graphics->addPipeline(camera->getPipeline());
	}
	void Scene::deactivateCamera(CameraSceneNode::Ptr camera)
	{
		for (unsigned int i = 0; i < activecameras.size(); i++)
		{
			if (activecameras[i] == camera)
			{
				graphics->removePipeline(camera->getPipeline());
				activecameras.erase(activecameras.begin() + i);
				break;
			}
		}
	}

	void Scene::render()
	{
		// Update all scene nodes
		rootnode->update(false);
		// Render all cameras
		for (unsigned int i = 0; i < activecameras.size(); i++)
		{
			rootnode->render(activecameras[i]->getPipeline()->getDefaultSequence());
		}
	}
}
}
