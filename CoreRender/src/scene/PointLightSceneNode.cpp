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

#include "CoreRender/scene/PointLightSceneNode.hpp"
#include "CoreRender/scene/LightManager.hpp"

namespace cr
{
namespace scene
{
	PointLightSceneNode::PointLightSceneNode(LightManager *lights,
	                                         render::Material::Ptr deferredmat,
	                                         const std::string &lightcontext,
	                                         const std::string &shadowcontext)
		: LightSceneNode(lights, deferredmat, lightcontext, shadowcontext)
	{
	}
	PointLightSceneNode::~PointLightSceneNode()
	{
	}

	void PointLightSceneNode::addToForwardLightLoop(LightLoopInfo *loop)
	{
		// Add light commands
		render::BatchListCommand *forwardcmd = new render::BatchListCommand;
		forwardcmd->setContext(getLightContext());
		loop->commands->addCommand(forwardcmd);
		setUniforms(forwardcmd);
		// TODO: Shadows
		// Add forward camera config
		CameraConfig *forwardconfig = new CameraConfig;
		forwardconfig->addBatchList(forwardcmd);
		loop->camera->addSecondaryConfig(forwardconfig);
		forwardconfigs.push_back(forwardconfig);
		// TODO: Set bounding box
		// Add shadow camera config
		// TODO
	}
	void PointLightSceneNode::removeFromForwardLightLoop(LightLoopInfo *loop,
	                                                     unsigned int index)
	{
		// Remove light commands
		loop->commands->removeCommand(index);
		// Remove forward camera config
		// We do not need locking here as long as there is only one LightManager
		for (unsigned int i = 0; i < forwardconfigs.size(); i++)
		{
			if (forwardconfigs[i] == loop->forwardconfigs[index])
			{
				forwardconfigs[i] = forwardconfigs[forwardconfigs.size() - 1];
				forwardconfigs.pop_back();
				break;
			}
		}
		loop->camera->removeSecondaryConfig(loop->forwardconfigs[index]);
		delete loop->forwardconfigs[index];
		loop->forwardconfigs.erase(loop->forwardconfigs.begin() + index);
		// Remove shadow camera config
		// TODO
	}
	void PointLightSceneNode::addToDeferredLightLoop(LightLoopInfo *loop)
	{
		// TODO
	}
	void PointLightSceneNode::removeFromDeferredLightLoop(LightLoopInfo *loop,
	                                                      unsigned int index)
	{
		// TODO
	}

	void PointLightSceneNode::onUpdate(bool abstranschanged)
	{
		if (abstranschanged)
		{
			// Reset all uniforms
			for (unsigned int i = 0; i < forwardconfigs.size(); i++)
			{
				// This assumes that every forward rendering config only has one
				// light loop attached
				setUniforms(forwardconfigs[i]->getBatchList(0));
			}
		}
	}

	void PointLightSceneNode::setUniforms(render::BatchListCommand *batchlist)
	{
		math::Vector3F abspos = getAbsTransMat().transformPoint(position);
		batchlist->getUniformData().add("lightPos") = abspos;
		batchlist->getUniformData().add("lightRadius") = getRadius();
		math::Vector3F colorvec;
		colorvec.x = (float)getColor().getRed() / 255.0f;
		colorvec.y = (float)getColor().getGreen() / 255.0f;
		colorvec.z = (float)getColor().getBlue() / 255.0f;
		batchlist->getUniformData().add("lightColor") = colorvec;
	}
}
}
