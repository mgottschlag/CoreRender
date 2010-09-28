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

#include "CoreRender/scene/ModelSceneNode.hpp"
#include "CoreRender/render/ModelRenderable.hpp"
#include "CoreRender/render/PipelineSequence.hpp"

namespace cr
{
namespace scene
{
	ModelSceneNode::ModelSceneNode(render::Model::Ptr model)
	{
		this->model = new render::ModelRenderable;
		this->model->setModel(model);
	}
	ModelSceneNode::~ModelSceneNode()
	{
		delete model;
	}

	render::Model::Ptr ModelSceneNode::getModel()
	{
		return model->getModel();
	}

	void ModelSceneNode::submit(render::PipelineSequence *sequence)
	{
		sequence->submit(model);
	}
	void ModelSceneNode::onUpdate(bool abstranschanged)
	{
		if (abstranschanged)
			model->setTransMat(getAbsTransMat());
	}
}
}
