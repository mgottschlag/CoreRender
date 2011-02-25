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

#ifndef _CORERENDER_SCENE_ANIMATEDMODEL_HPP_INCLUDED_
#define _CORERENDER_SCENE_ANIMATEDMODEL_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"
#include "Model.hpp"
#include "Animation.hpp"
#include "AnimationBinding.hpp"

namespace cr
{
namespace scene
{
	class AnimatedModel : public core::ReferenceCounted
	{
		public:
			AnimatedModel(Model::Ptr model = 0);
			virtual ~AnimatedModel();

			void setModel(Model::Ptr model);
			Model::Ptr getModel();

			struct AnimationStage
			{
				AnimationStage()
					: binding(0, 0)
				{
				}
				Animation::Ptr animation;
				float weight;
				bool additive;
				float time;
				AnimationBinding binding;
			};

			struct NodeAnimationInfo
			{
				bool updated;
				math::Vec3f trans;
				math::Vec3f scale;
				math::Quaternion rot;
			};

			unsigned int addAnimation(Animation::Ptr animation,
			                          float weight = 1.0,
			                          bool additive = false);
			void setAnimation(unsigned int index, float time);
			void removeAnimation(unsigned int index);
			unsigned int getAnimationCount();

			void render(render::RenderQueue &queue,
			            math::Mat4f transmat);
			void render(render::RenderQueue &queue,
			            unsigned int instancecount,
			            math::Mat4f *transmat);

			typedef core::SharedPointer<AnimatedModel> Ptr;
		private:
			float applyStage(unsigned int stageindex,
			                 NodeAnimationInfo *nodes,
			                 float weightsum);
			void applyAnimation(std::vector<Model::Node> &nodes);

			Model::Ptr model;
			std::vector<AnimationStage> animstages;
	};
}
}

#endif
