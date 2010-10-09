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

#include "CoreRender/scene/CameraSceneNode.hpp"

namespace cr
{
namespace scene
{
	CameraSceneNode::CameraSceneNode(render::Pipeline::Ptr pipeline)
		: pipeline(pipeline), projmatdirty(true),
		projmat(math::Matrix4::Identity())
	{
		// Build primary configs
		std::vector<render::BatchListCommand*> batchlists;
		pipeline->getBatchLists(batchlists);
		for (unsigned int i = 0; i < batchlists.size(); i++)
			primaryconfig.addBatchList(batchlists[i]);
	}
	CameraSceneNode::~CameraSceneNode()
	{
		// Delete secondary configs
		// TODO: This should not be done here
		/*for (unsigned int i = 0; i < primaryconfigs.size(); i++)
			delete primaryconfigs[i];*/
	}

	void CameraSceneNode::setOrtho(float left,
	                               float right,
	                               float bottom,
	                               float top,
	                               float near,
	                               float far)
	{
		projmat = math::Matrix4::Ortho(left, right, bottom, top, near, far);
		projmatdirty = true;
	}
	void CameraSceneNode::setPerspective(float left,
	                                     float right,
	                                     float bottom,
	                                     float top,
	                                     float near,
	                                     float far)
	{
		projmat = math::Matrix4::Perspective(left, right, bottom, top, near, far);
		projmatdirty = true;
	}
	void CameraSceneNode::setPerspectiveFOV(float fov,
	                                        float aspectratio,
	                                        float near,
	                                        float far)
	{
		projmat = math::Matrix4::PerspectiveFOV(fov, aspectratio, near, far);
		projmatdirty = true;
	}

	void CameraSceneNode::addSecondaryConfig(CameraConfig *config)
	{
		tbb::mutex::scoped_lock lock(configmutex);
		secondaryconfigs.push_back(config);
		// Set matrices
		config->setProjection(projmat * getAbsTransMat().inverse());
	}
	void CameraSceneNode::removeSecondaryConfig(CameraConfig *config)
	{
		tbb::mutex::scoped_lock lock(configmutex);
		for (unsigned int i = 0; i < secondaryconfigs.size(); i++)
		{
			if (secondaryconfigs[i] == config)
			{
				secondaryconfigs[i] = secondaryconfigs[secondaryconfigs.size() - 1];
				secondaryconfigs.pop_back();
				break;
			}
		}
	}
	const std::vector<CameraConfig*> &CameraSceneNode::getSecondaryConfigs()
	{
		return secondaryconfigs;
	}

	render::Pipeline::Ptr CameraSceneNode::getPipeline()
	{
		return pipeline;
	}

	void CameraSceneNode::onUpdate(bool abstranschanged)
	{
		if (abstranschanged || projmatdirty)
		{
			// Set primary config
			math::Matrix4 cameramat = projmat * getAbsTransMat().inverse();
			primaryconfig.setProjection(cameramat);
			// Set secondary configs
			for (unsigned int i = 0; i < secondaryconfigs.size(); i++)
				secondaryconfigs[i]->setProjection(cameramat);
			// Reset dirty flag
			projmatdirty = false;
		}
	}
}
}
