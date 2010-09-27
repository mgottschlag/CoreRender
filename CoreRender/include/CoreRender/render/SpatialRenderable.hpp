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
	/**
	 * Renderable object which has a 3D position/rotation. This is the base
	 * class for most renderable objects in a scene.
	 *
	 * This class contains some matrices which then are used for standard
	 * uniforms in shaders, which are "worldMat" for the world matrix
	 * (projection matrix * transformation matrix) and "worldNormalMat" to
	 * transform normals.
	 */
	class SpatialRenderable : public Renderable
	{
		public:
			/**
			 * Constructor.
			 */
			SpatialRenderable()
				: transmat(math::Matrix4::Identity())
			{
			}
			/**
			 * Destructor.
			 */
			virtual ~SpatialRenderable()
			{
			}

			/**
			 * Sets the transformation matrix for this renderable object.
			 * @param transmat New transformation matrix.
			 */
			void setTransMat(const math::Matrix4 &transmat)
			{
				this->transmat = transmat;
			}
			/**
			 * Returns the transformation matrix.
			 * @return Transformation matrix.
			 */
			const math::Matrix4 &getTransMat()
			{
				return transmat;
			}
		private:
			math::Matrix4 transmat;
	};
}
}

#endif
