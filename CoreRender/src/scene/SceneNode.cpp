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

#include "CoreRender/scene/SceneNode.hpp"

namespace cr
{
namespace scene
{
	SceneNode::SceneNode()
		: parent(0), transmatdirty(true), position(0.0, 0.0, 0.0),
		rotation(math::Vector3F(0.0, 0.0, 0.0)), scale(1.0, 1.0, 1.0),
		visible(true)
	{
	}
	SceneNode::~SceneNode()
	{
		while (children.size() > 0)
			children[0]->setParent(0);
	}

	void SceneNode::setPosition(const math::Vector3F &position)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		this->position = position;
		transmatdirty = true;
	}
	void SceneNode::setRotation(const math::Quaternion &rotation)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		this->rotation = rotation;
		transmatdirty = true;
	}
	void SceneNode::setRotation(const math::Vector3F &rotation)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		this->rotation = rotation;
		transmatdirty = true;
	}
	void SceneNode::setScale(const math::Vector3F &scale)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		this->scale = scale;
		transmatdirty = true;
	}
	math::Vector3F SceneNode::getPosition()
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		return position;
	}
	math::Quaternion SceneNode::getRotation()
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		return rotation;
	}
	math::Vector3F SceneNode::getScale()
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		return scale;
	}

	void SceneNode::setParent(SceneNode::Ptr parent)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		//tbb::spin_mutex::scoped_lock lock2(this->parent);
		if (this->parent)
			this->parent->removeChild(this);
		this->parent = parent.get();
		if (parent)
			parent->addChild(this);
		transmatdirty = true;
	}
	SceneNode::Ptr SceneNode::getParent()
	{
		return parent;
	}

	void SceneNode::setVisible(bool visible)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		if (visible != this->visible)
		{
			this->visible = visible;
			// We need to force a matrix update when a scene node is visible again
			transmatdirty = true;
		}
	}
	bool SceneNode::isVisible()
	{
		return visible;
	}

	void SceneNode::remove()
	{
		setParent(0);
	}

	void SceneNode::update(bool abstranschanged)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		if (!visible)
			return;
		// Update transformation matrix
		if (transmatdirty)
		{
			transmat = math::Matrix4::TransMat(position)
			         * rotation.toMatrix()
			         * math::Matrix4::ScaleMat(scale);
			transmatdirty = false;
			abstranschanged = true;
		}
		// Update absolute transformation matrix
		if (abstranschanged && parent)
			abstrans = parent->getAbsTransMat() * transmat;
		else if (abstranschanged)
			abstrans = transmat;
		onUpdate(abstranschanged);
		// Update children
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->update(abstranschanged);
	}
	void SceneNode::render(render::PipelineSequence *sequence)
	{
		if (!visible)
			return;
		// Submit this node
		submit(sequence);
		// Render children
		// TODO: Do we want to render multiple pipelines in parallel?
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->render(sequence);
	}

	void SceneNode::removeChild(SceneNode::Ptr child)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (children[i] == child)
			{
				children[i] = children[children.size() - 1];
				children.pop_back();
				break;
			}
		}
	}
	void SceneNode::addChild(SceneNode::Ptr child)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		children.push_back(child);
	}
}
}
