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

#ifndef _CORERENDER_RENDER_MODELRENDERABLE_HPP_INCLUDED_
#define _CORERENDER_RENDER_MODELRENDERABLE_HPP_INCLUDED_

#include "SpatialRenderable.hpp"
#include "Model.hpp"
#include "Animation.hpp"

namespace cr
{
namespace render
{
	class ModelRenderable : public SpatialRenderable
	{
		public:
			ModelRenderable();
			virtual ~ModelRenderable();

			void setModel(Model::Ptr model);
			Model::Ptr getModel();

			struct AnimStage
			{
				Animation::Ptr anim;
				float weight;
				bool additive;
				std::string startnode;
			};

			unsigned int addAnimStage(Animation::Ptr anim,
			                          float weight,
			                          bool additive = false,
			                          std::string startnode = "");
			AnimStage *getAnimStage(unsigned int index);
			void removeAnimStage(unsigned int index);
			unsigned int getAnimStateCount();

			virtual unsigned int beginRendering();
			virtual RenderJob *getJob(unsigned int index);
			virtual void endRendering();

			UniformData &getUniformData()
			{
				return uniforms;
			}
		private:
			Model::Ptr model;
			std::vector<AnimStage> animstages;
			UniformData uniforms;
			std::vector<cr::render::RenderJob> jobs;
	};
}
}

#endif
