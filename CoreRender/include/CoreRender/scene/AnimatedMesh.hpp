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

#ifndef _CORERENDER_SCENE_ANIMATEDMESH_HPP_INCLUDED_
#define _CORERENDER_SCENE_ANIMATEDMESH_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"
#include "Mesh.hpp"
#include "Animation.hpp"

namespace cr
{
namespace scene
{
	class AnimatedMesh : public core::ReferenceCounted
	{
		public:
			AnimatedMesh(Mesh::Ptr mesh = 0);
			virtual ~AnimatedMesh();

			void setMesh(Mesh::Ptr mesh);
			Mesh::Ptr getMesh();

			void addAnimation(Animation::Ptr animation,
			                  float weight = 1.0,
			                  bool additive = false);
			void setAnimation(unsigned int index, float time);
			void removeAnimation(unsigned int index);

			void render(render::RenderQueue &queue,
			            math::Matrix4 transmat);
			void render(render::RenderQueue &queue,
			            unsigned int instancecount,
			            math::Matrix4 *transmat);

			typedef core::SharedPointer<AnimatedMesh> Ptr;
		private:
	};
}
}

#endif
