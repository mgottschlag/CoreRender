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

#ifndef _CORERENDER_MATH_PLANE_HPP_INCLUDED_
#define _CORERENDER_MATH_PLANE_HPP_INCLUDED_

#include "Vector3.hpp"

namespace cr
{
namespace math
{
	class Plane
	{
		public:
			Plane()
				: normal(0, 1, 0), d(0)
			{
			}
			Plane(const Vector3F &normal, float d)
				: normal(normal), d(d)
			{
			}
			Plane(float a, float b, float c, float d)
				: normal(a, b, c), d(d)
			{
			}
			Plane(const Vector3F &p1, const Vector3F &p2, const Vector3F &p3)
			{
				Vector3F v1 = p2 - p1;
				Vector3F v2 = p3 - p1;
				normal = v1.cross(v2);
				d = p1.dot(normal);
			}

			void normalize()
			{
				float length = normal.getLength();
				d /= length;
				normal /= length;
			}

			/**
			 * Returns the distance of the point relative to the plane. The unit
			 * of the returned value is the length of the normal vector of the
			 * plane, so use this with a vector with the length 1 to get the
			 * "real" distance. You can use this with a different vector length
			 * to find out on which side of the plane the point is.
			 * @param point Point to compute the distance to.
			 * @return Distance of the point to the plane.
			 */
			float getDistance(const Vector3F &point)
			{
				float d2 = point.dot(normal);
				return d2 - d;
			}

			Vector3F normal;
			float d;
	};
}
}

#endif
