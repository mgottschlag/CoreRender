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

#include "CoreRender/scene/LightSceneNode.hpp"
#include <CoreRender/scene/Scene.hpp>

namespace cr
{
namespace scene
{
	LightSceneNode::LightSceneNode(LightManager *lights,
	                               render::Material::Ptr deferredmat,
	                               const std::string &lightcontext,
	                               const std::string &shadowcontext)
		: lights(lights), deferredmat(deferredmat), lightcontext(lightcontext),
		shadowcontext(shadowcontext), color(255, 255, 255, 255), radius(10.0f),
		active(true)
	{
		lights->addLight(this);
	}
	LightSceneNode::~LightSceneNode()
	{
		if (active)
			lights->removeLight(this);
	}

	void LightSceneNode::setColor(core::Color color)
	{
		this->color = color;
	}
	core::Color LightSceneNode::getColor()
	{
		return color;
	}
	void LightSceneNode::setRadius(float radius)
	{
		this->radius = radius;
	}
	float LightSceneNode::getRadius()
	{
		return radius;
	}

	void LightSceneNode::setActive(bool active)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		if (this->active == active)
			return;
		if (active)
			lights->addLight(this);
		else
			lights->removeLight(this);
		this->active = active;
	}
}
}
