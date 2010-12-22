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

#include "CoreRender/scene/AnimationBinding.hpp"

namespace cr
{
namespace scene
{
	AnimationBinding::AnimationBinding(cr::scene::Animation::Ptr animation, cr::scene::Model::Ptr model)
		: animation(animation), model(model)
	{
		forceUpdate();
	}
	AnimationBinding::~AnimationBinding()
	{
	}

	void AnimationBinding::update()
	{
		tbb::mutex::scoped_lock lock(mutex);
		if (!animation || !model)
			return;
		if (animchangecounter == animation->getChangeCounter()
			&& modelchangecounter == model->getChangeCounter())
			return;
		forceUpdate();
		animchangecounter = animation->getChangeCounter();
		modelchangecounter = model->getChangeCounter();
	}

	const std::vector<int> &AnimationBinding::getNodes()
	{
		return mapping;
	}

	void AnimationBinding::forceUpdate()
	{
		if (!animation || !model)
			return;
		unsigned int jointcount = animation->getChannelCount();
		std::vector<Model::Node> &modelnodes = model->getNodes();
		mapping.resize(jointcount);
		for (unsigned int i = 0; i < jointcount; i++)
		{
			std::string name = animation->getChannelNode(i);
			mapping[i] = -1;
			for (unsigned int j = 0; j < modelnodes.size(); j++)
			{
				if (modelnodes[j].name == name)
				{
					mapping[i] = j;
					break;
				}
			}
		}
	}
}
}
