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
		// Prepare batches
		jobs.resize(model->getMeshCount());
		for (unsigned int i = 0; i < model->getMeshCount(); i++)
		{
			jobs[i].vertices = model->getVertexBuffer();
			jobs[i].indices = model->getIndexBuffer();
			cr::render::Model::Mesh *mesh = model->getMesh(i);
			cr::render::Model::Batch *batch = model->getBatch(mesh->batch);
			jobs[i].material = mesh->material;
			jobs[i].layout = batch->layout;
			jobs[i].vertexcount = batch->vertexcount;
			jobs[i].startindex = batch->startindex;
			jobs[i].endindex = batch->startindex + batch->indexcount;
			jobs[i].vertexoffset = batch->vertexoffset;
			jobs[i].indextype = batch->indextype;
			jobs[i].basevertex = 0;
			jobs[i].uniforms = uniforms;
			// Set standard uniforms
			// TODO: This is a hack, slow, and ugly.
			math::Matrix4 oldtransmat = getTransMat();
			setTransMat(oldtransmat * mesh->node->getAbsTrans());
			uniforms["worldMat"] = getWorldMat();
			uniforms["worldNormalMat"] = getWorldNormalMat();
			setTransMat(oldtransmat);
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
