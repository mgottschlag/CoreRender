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

#ifndef _CORERENDER_MATH_BOUNDINGBOX_HPP_INCLUDED_
#define _CORERENDER_MATH_BOUNDINGBOX_HPP_INCLUDED_

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix4.hpp"

namespace cr
{
namespace math
{
	/**
	 * Axis-aligned bounding box.
	 */
	class BoundingBox
	{
		public:
			/**
			 * Constructor. Creates an empty box with the center at the origin
			 * of the coordinate system.
			 */
			BoundingBox()
				: min(0, 0, 0), max(0, 0, 0)
			{
			}
			/**
			 * Constructor.
			 * @param min Point with the smallest values for all components
			 * which is still in the box.
			 * @param min Point with the largest values for all components which
			 * is still in the box.
			 */
			BoundingBox(const Vector3F &min, const Vector3F &max)
				: min(min), max(max)
			{
			}
			/**
			 * Constructor. Creates an empty box.
			 * @param center Center of the bounding box.
			 */
			BoundingBox(const Vector3F &center)
				: min(center), max(center)
			{
			}

			Vector3F getCorner(unsigned int index)
			{
				switch (index)
				{
					case 0:
						return Vector3F(min.x, min.y, min.z);
					case 1:
						return Vector3F(max.x, min.y, min.z);
					case 2:
						return Vector3F(max.x, max.y, min.z);
					case 3:
						return Vector3F(min.x, max.y, min.z);
					case 4:
						return Vector3F(min.x, min.y, max.z);
					case 5:
						return Vector3F(max.x, min.y, max.z);
					case 6:
						return Vector3F(max.x, max.y, max.z);
					case 7:
						return Vector3F(min.x, max.y, max.z);
					default:
						return Vector3F(0, 0, 0);
				}
			}
			/**
			 * Returns the center of the bounding box.
			 * @return Center of the box.
			 */
			Vector3F getCenter()
			{
				return (min + max) * 0.5f;
			}
			/**
			 * Returns the size of the bounding box.
			 * @return Size of the box.
			 */
			Vector3F getSize()
			{
				return max - min;
			}

			/**
			 * Inserts a point into the bounding box and enlarges the box so
			 * that it contains the point if necessary.
			 */
			void insert(const Vector3F &point)
			{
				if (min.x < point.x)
					min.x = point.x;
				if (max.x > point.x)
					max.x = point.x;
				if (min.y < point.y)
					min.y = point.y;
				if (max.y > point.y)
					max.y = point.y;
				if (min.z < point.z)
					min.z = point.z;
				if (max.z > point.z)
					max.z = point.z;
			}
			/**
			 * Inserts another bounding box into the bounding box and enlarges
			 * the box so that it contains the point if necessary.
			 */
			void insert(const BoundingBox &box)
			{
				insert(box.min);
				insert(box.max);
			}

			/**
			 * Creates a new bounding box which contains all corners of this box
			 * transformed with the given transformation matrix.
			 * @param transformation Transformation matrix.
			 * @return Transformed bounding box.
			 */
			BoundingBox transform(const Matrix4 &transformation)
			{
				// Construct empty box from first corner
				// If we do not initialize the box like this, (0/0/0) would
				// always be in the box
				BoundingBox box(transformation.transformPoint(getCorner(0)));
				// Insert other corners
				for (unsigned int i = 1; i < 8; i++)
					box.insert(transformation.transformPoint(getCorner(i)));
				return box;
			}

			/**
			 * Returns whether a point is inside the bounding box.
			 * @param point Point to check.
			 * @return True if the point is in the box.
			 */
			bool isInside(const Vector3F &point)
			{
				// TODO: This function uses a different notation compared to
				// the frustum functions
				if (point.x < min.x)
					return false;
				if (point.y < min.y)
					return false;
				if (point.z < min.z)
					return false;
				if (point.x > max.x)
					return false;
				if (point.y > max.y)
					return false;
				if (point.z > max.z)
					return false;
				return true;
			}

			/**
			 * Minimum coordinates of all points in the bounding box.
			 */
			Vector3F min;
			/**
			 * Maximum coordinates of all points in the bounding box.
			 */
			Vector3F max;
	};
}
}

#endif
