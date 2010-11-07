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

#ifndef _CORERENDER_SCENE_CAMERA_HPP_INCLUDED_
#define _CORERENDER_SCENE_CAMERA_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"
#include "../render/Pipeline.hpp"
#include "../render/RenderTarget.hpp"
#include "../math/Matrix4.hpp"

namespace cr
{
namespace scene
{
	class Camera : public core::ReferenceCounted
	{
		public:
			Camera(render::Pipeline::Ptr pipeline = 0);
			virtual ~Camera();

			void setPipeline(render::Pipeline::Ptr pipeline)
			{
				this->pipeline = pipeline;
			}
			render::Pipeline::Ptr getPipeline()
			{
				return pipeline;
			}

			void setTarget(render::RenderTarget::Ptr target)
			{
				this->target = target;
			}
			render::RenderTarget::Ptr getTarget()
			{
				return target;
			}

			void setProjMat(math::Matrix4 projmat)
			{
				this->projmat = projmat;
			}
			math::Matrix4 getProjMat()
			{
				return projmat;
			}
			void setViewMat(math::Matrix4 viewmat)
			{
				this->viewmat = viewmat;
			}
			math::Matrix4 getViewMat()
			{
				return projmat;
			}

			void setViewport(unsigned int x,
			                 unsigned int y,
			                 unsigned int width,
			                 unsigned int height)
			{
				viewport[0] = x;
				viewport[1] = y;
				viewport[2] = width;
				viewport[3] = height;
			}
			unsigned int *getViewport()
			{
				return viewport;
			}

			typedef core::SharedPointer<Camera> Ptr;
		private:
			render::Pipeline::Ptr pipeline;
			render::RenderTarget::Ptr target;

			unsigned int viewport[4];
			math::Matrix4 projmat;
			math::Matrix4 viewmat;
	};
}
}

#endif
