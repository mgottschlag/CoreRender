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

#include "CoreRender/scene/AnimatedMesh.hpp"

namespace cr
{
namespace scene
{
	AnimatedMesh::AnimatedMesh(Mesh::Ptr mesh)
	{
	}
	AnimatedMesh::~AnimatedMesh()
	{
	}

	void AnimatedMesh::setMesh(Mesh::Ptr mesh)
	{
	}
	Mesh::Ptr AnimatedMesh::getMesh()
	{
	}

	void AnimatedMesh::addAnimation(Animation::Ptr animation,
	                                float weight,
	                                bool additive)
	{
	}
	void AnimatedMesh::setAnimation(unsigned int index, float time)
	{
	}
	void AnimatedMesh::removeAnimation(unsigned int index)
	{
	}

	void AnimatedMesh::render(render::RenderQueue &queue,
	                          math::Matrix4 transmat)
	{
	}
	void AnimatedMesh::render(render::RenderQueue &queue,
	                          unsigned int instancecount,
	                          math::Matrix4 *transmat)
	{
	}
}
}
