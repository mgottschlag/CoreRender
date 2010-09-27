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

#include "CoreRender/render/ModelRenderable.hpp"
#include "CoreRender/render/RenderJob.hpp"

#include <cstdio>

namespace cr
{
namespace render
{
	ModelRenderable::ModelRenderable()
	{
		uniforms.add("worldMat");
		uniforms.add("worldNormalMat");
	}
	ModelRenderable::~ModelRenderable()
	{
	}

	void ModelRenderable::setModel(Model::Ptr model)
	{
		this->model = model;
	}
	Model::Ptr ModelRenderable::getModel()
	{
		return model;
	}

	unsigned int ModelRenderable::addAnimStage(Animation::Ptr anim,
	                                           float weight,
	                                           bool additive,
	                                           std::string startnode)
	{
		AnimStage newstage;
		newstage.anim = anim;
		newstage.weight = weight;
		newstage.additive = additive;
		newstage.startnode = startnode;
		newstage.time = 0.0f;
		animstages.push_back(newstage);
		return animstages.size() - 1;
	}
	ModelRenderable::AnimStage *ModelRenderable::getAnimStage(unsigned int index)
	{
		if (index >= animstages.size())
			return 0;
		return &animstages[index];
	}
	void ModelRenderable::removeAnimStage(unsigned int index)
	{
		if (index >= animstages.size())
			return;
		animstages.erase(animstages.begin() + index);
	}
	unsigned int ModelRenderable::getAnimStateCount()
	{
		return animstages.size();
	}

	unsigned int ModelRenderable::beginRendering()
	{
		if (!model)
			return 0;
		// Get list with model nodes (needed for joints)
		// TODO: Cache these
		Model::AnimationNodeMap nodes;
		model->getNodeList(nodes);
		// Prepare skeletal animation
		for (Model::AnimationNodeMap::iterator it = nodes.begin();
		     it != nodes.end(); it++)
		{
			for (unsigned int i = 0; i < animstages.size(); i++)
			{
				Animation::Ptr anim = animstages[i].anim;
				// TODO: Proper mixing
				Animation::Channel *channel = anim->getChannel(it->first);
				if (!channel)
					continue;
				Animation::Frame frame = anim->getFrame(channel, animstages[i].time);
				math::Matrix4 animmatrix = math::Matrix4::TransMat(frame.position)
					* frame.rotation.toMatrix()
					* math::Matrix4::ScaleMat(frame.scale);
				it->second->transformation = animmatrix;
				it->second->transinverse = animmatrix.inverse();
			}
		}
		// Update absolute transformation
		for (Model::AnimationNodeMap::iterator it = nodes.begin();
		     it != nodes.end(); it++)
		{
			it->second->computeAbsTrans();
		}
		// Prepare batches
		jobs.resize(model->getMeshCount());
		for (unsigned int i = 0; i < model->getMeshCount(); i++)
		{
			Model::Mesh *mesh = model->getMesh(i);
			Model::Batch *batch = model->getBatch(mesh->batch);
			// Get node this mesh is attached to
			Model::AnimationNode *node = nodes.find(mesh->node->getName())->second;
			// Create job
			RenderJob &job = jobs[i];
			job.vertices = model->getVertexBuffer();
			job.indices = model->getIndexBuffer();
			job.material = mesh->material;
			job.layout = batch->layout;
			job.vertexcount = batch->vertexcount;
			job.startindex = batch->startindex;
			job.endindex = batch->startindex + batch->indexcount;
			job.vertexoffset = batch->vertexoffset;
			job.indextype = batch->indextype;
			job.basevertex = 0;
			job.uniforms = uniforms;
			// Apply animations
			Model::AnimationNodeMap::iterator it;
			for (unsigned int i = 0; i < batch->joints.size(); i++)
			{
				// Get uniform name
				char uniformname[20];
				snprintf(uniformname, 20, "skinMat[%u]", i);
				// Find joint node
				Model::Joint &joint = batch->joints[i];
				std::string jointname = joint.name;
				it = nodes.find(jointname);
				if (it == nodes.end())
				{
					// TODO: Log warning
					job.uniforms.add(uniformname) = math::Matrix4::Identity();
					continue;
				}
				Model::AnimationNode *jointnode = it->second;
				// Compute and set joint matrix
				math::Matrix4 matrix = joint.jointmat;
				// TODO: Slow.
				job.uniforms.add(uniformname) = node->abstransinverse * jointnode->abstrans * joint.jointmat;
			}
			// Set standard uniforms
			// TODO: We only have to do this if we do not use skinning
			math::Matrix4 transmat = getTransMat();
			job.defaultuniforms.push_back(DefaultUniform(DefaultUniformName::TransMatrix, transmat));
		}
		// Clear node list again
		for (Model::AnimationNodeMap::iterator it = nodes.begin();
		     it != nodes.end(); it++)
		{
			delete it->second;
		}
		return model->getMeshCount();
	}
	RenderJob *ModelRenderable::getJob(unsigned int index)
	{
		return &jobs[index];
	}
	void ModelRenderable::endRendering()
	{
		jobs.clear();
	}
}
}
