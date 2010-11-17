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
	AnimationBinding::AnimationBinding(Animation::Ptr animation, Mesh::Ptr mesh)
		: animation(animation), mesh(mesh)
	{
		forceUpdate();
	}
	AnimationBinding::~AnimationBinding()
	{
	}

	void AnimationBinding::update()
	{
		tbb::mutex::scoped_lock lock(mutex);
		if (!animation || !mesh)
			return;
		if (animchangecounter == animation->getChangeCounter()
			&& meshchangecounter == mesh->getChangeCounter())
			return;
		forceUpdate();
		animchangecounter = animation->getChangeCounter();
		meshchangecounter = mesh->getChangeCounter();
	}

	const std::vector<int> &AnimationBinding::getNodes()
	{
		return mapping;
	}

	void AnimationBinding::forceUpdate()
	{
		if (!animation || !mesh)
			return;
		unsigned int jointcount = animation->getChannelCount();
		std::vector<Mesh::Node> &meshnodes = mesh->getNodes();
		mapping.resize(jointcount);
		for (unsigned int i = 0; i < jointcount; i++)
		{
			std::string name = animation->getChannelNode(i);
			mapping[i] = -1;
			for (unsigned int j = 0; j < meshnodes.size(); j++)
			{
				if (meshnodes[j].name == name)
				{
					mapping[i] = j;
					break;
				}
			}
		}
	}
}
}
