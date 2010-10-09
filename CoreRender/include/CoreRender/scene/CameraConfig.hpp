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

#ifndef _CORERENDER_SCENE_CAMERACONFIG_HPP_INCLUDED_
#define _CORERENDER_SCENE_CAMERACONFIG_HPP_INCLUDED_

#include "CoreRender/math/Matrix4.hpp"
#include "CoreRender/math/BoundingBox.hpp"
#include "CoreRender/math/Frustum.hpp"

#include <vector>
#include <tbb/spin_mutex.h>

namespace cr
{
namespace render
{
	class BatchListCommand;
}
namespace scene
{
	class CameraConfig
	{
		public:
			CameraConfig();
			~CameraConfig();

			void setProjection(const math::Matrix4 &projmat);
			math::Matrix4 getProjection();

			void setBoxCulling(bool active, const math::BoundingBox &box);
			math::BoundingBox getBoxCulling();
			bool getBoxCullingActive();

			void addBatchList(render::BatchListCommand *batchlist);
			void removeBatchList(render::BatchListCommand *batchlist);
			void removeBatchList(unsigned int index);
			render::BatchListCommand *getBatchList(unsigned int index);
			unsigned int getBatchListCount();

			bool cull(const math::BoundingBox &box);
		private:
			void updateUniforms();

			math::Matrix4 projmat;
			math::Frustum frustum;
			math::BoundingBox boundingbox;
			bool useboundingbox;

			std::vector<render::BatchListCommand*> batchlists;

			tbb::spin_mutex mutex;
	};
}
}

#endif
