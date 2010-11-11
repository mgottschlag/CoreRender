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

#ifndef _CORERENDER_RENDER_ANIMATIONBINDING_HPP_INCLUDED_
#define _CORERENDER_RENDER_ANIMATIONBINDING_HPP_INCLUDED_

#include "Animation.hpp"
#include "Mesh.hpp"

namespace cr
{
namespace scene
{
	/**
	 * Class which stores the relation between animation joints and mesh nodes.
	 */
	class AnimationBinding
	{
		public:
			/**
			 * Constructor.
			 * @param animation Animation from which joints are taken from.
			 * @param mesh Mesh from which nodes are taken from.
			 */
			AnimationBinding(Animation::Ptr animation, Mesh::Ptr mesh);
			/**
			 * Destructor.
			 */
			~AnimationBinding();

			/**
			 * Updates the animation binding. Has to be called after either the
			 * animation or the mesh changed. Is called once in the constructor.
			 * Can be called more often as it will not search for nodes again if
			 * neither the animation nor the mesh changed.
			 */
			void update();

			/**
			 * Returns an array containing node indices. The array index then
			 * relates to the joint index, so getNodes()[1] gets the node for
			 * the second joint. If an element is -1, no node was found.
			 * @return Mapping of joint indices to mesh node indices.
			 */
			const std::vector<int> &getNodes();
		private:
			void forceUpdate();

			Animation::Ptr animation;
			Mesh::Ptr mesh;
			tbb::mutex mutex;
			std::vector<int> mapping;
			unsigned int animchangecounter;
			unsigned int meshchangecounter;
	};
}
}

#endif
