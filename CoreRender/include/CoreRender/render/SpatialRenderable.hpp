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

#ifndef _CORERENDER_RENDER_SPATIALRENDERABLE_HPP_INCLUDED_
#define _CORERENDER_RENDER_SPATIALRENDERABLE_HPP_INCLUDED_

#include "Renderable.hpp"
#include "../math/Matrix4.hpp"
#include "../math/Matrix3.hpp"

namespace cr
{
namespace render
{
	class SpatialRenderable : public Renderable
	{
		public:
			SpatialRenderable()
				: transmat(math::Matrix4::Identity()),
				projmat(math::Matrix4::Identity()),
				dirty(false)
			{
			}
			virtual ~SpatialRenderable()
			{
			}

			void setTransMat(const math::Matrix4 &transmat)
			{
				this->transmat = transmat;
				dirty = true;
			}
			void setProjMat(const math::Matrix4 &projmat)
			{
				this->projmat = projmat;
				dirty = true;
			}
			const math::Matrix4 &getTransMat()
			{
				if (dirty)
					updateMatrices();
				return transmat;
			}
			const math::Matrix4 &getProjMat()
			{
				if (dirty)
					updateMatrices();
				return projmat;
			}
			const math::Matrix4 &getWorldMat()
			{
				if (dirty)
					updateMatrices();
				return worldmat;
			}
			const math::Matrix4 &getWorldNormalMat()
			{
				if (dirty)
					updateMatrices();
				return worldnormalmat;
			}
		private:
			void updateMatrices()
			{
				dirty = false;
				worldmat = projmat * transmat;
				worldnormalmat = transmat.inverse().transposed();
			}
			math::Matrix4 transmat;
			math::Matrix4 projmat;
			math::Matrix4 worldmat;
			math::Matrix4 worldnormalmat;
			bool dirty;
	};
}
}

#endif
