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

#ifndef _CORERENDER_MATH_FRUSTUM_HPP_INCLUDED_
#define _CORERENDER_MATH_FRUSTUM_HPP_INCLUDED_

#include "BoundingBox.hpp"
#include "Plane.hpp"
#include "Matrix4.hpp"

namespace cr
{
namespace math
{
	class Frustum
	{
		public:
			Frustum()
			{
			}
			Frustum(const Matrix4 &projmat)
			{
				setProjectionMatrix(projmat);
			}

			void setProjectionMatrix(const Matrix4 &projmat)
			{
				this->projmat = projmat;
				Matrix4 projmatinv = projmat.inverse();
				// Corners of the frustum in world space
				corners[0] = projmatinv.transformPoint(Vector3F(-1, 1, -1));
				corners[1] = projmatinv.transformPoint(Vector3F(1, 1, -1));
				corners[2] = projmatinv.transformPoint(Vector3F(1, -1, -1));
				corners[3] = projmatinv.transformPoint(Vector3F(-1, -1, -1));
				corners[4] = projmatinv.transformPoint(Vector3F(-1, 1, 1));
				corners[5] = projmatinv.transformPoint(Vector3F(1, 1, 1));
				corners[6] = projmatinv.transformPoint(Vector3F(1, -1, 1));
				corners[7] = projmatinv.transformPoint(Vector3F(-1, -1, 1));
				// Near plane
				planes[0] = Plane(corners[0], corners[1], corners[2]);
				// Left plane
				planes[1] = Plane(corners[0], corners[3], corners[4]);
				// Top plane
				planes[2] = Plane(corners[0], corners[1], corners[4]);
				// Right plane
				planes[3] = Plane(corners[1], corners[2], corners[5]);
				// Bottom plane
				planes[4] = Plane(corners[2], corners[3], corners[6]);
				// Far plane
				planes[5] = Plane(corners[4], corners[5], corners[6]);
				// TODO: Fix normal vectors to point inside!
			}

			bool isInside(const Vector3F &point)
			{
				for (unsigned int i = 0; i < 6; i++)
				{
					if (planes[i].getDistance(point) < 0)
						return false;
				}
				return true;
			}
			/**
			 * Computes whether a box is partially or completely inside of the
			 * frustum.
			 * @note Warning: This function can return true even if the box
			 * actually is outside of the frustum, see
			 * http://www.lighthouse3d.com/opengl/viewfrustum/index.php?gatest3
			 * for an explanation of the algorithm used.
			 */
			bool isInside(const BoundingBox &box)
			{
				// TODO
			}

			Matrix4 projmat;
			Plane planes[6];
			Vector3F corners[6];
	};
}
}


#endif
