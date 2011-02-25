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

#include <GameMath.hpp>

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
				: minCorner(0, 0, 0), maxCorner(0, 0, 0)
			{
			}
			/**
			 * Constructor.
			 * @param minCorner Point with the smallest values for all components
			 * which is still in the box.
			 * @param minCorner Point with the largest values for all components which
			 * is still in the box.
			 */
			BoundingBox(const Vec3f &minCorner, const Vec3f &maxCorner)
				: minCorner(minCorner), maxCorner(maxCorner)
			{
			}
			/**
			 * Constructor. Creates an empty box.
			 * @param center Center of the bounding box.
			 */
			BoundingBox(const Vec3f &center)
				: minCorner(center), maxCorner(center)
			{
			}

			Vec3f getCorner(unsigned int index)
			{
				switch (index)
				{
					case 0:
						return Vec3f(minCorner.x, minCorner.y, minCorner.z);
					case 1:
						return Vec3f(maxCorner.x, minCorner.y, minCorner.z);
					case 2:
						return Vec3f(maxCorner.x, maxCorner.y, minCorner.z);
					case 3:
						return Vec3f(minCorner.x, maxCorner.y, minCorner.z);
					case 4:
						return Vec3f(minCorner.x, minCorner.y, maxCorner.z);
					case 5:
						return Vec3f(maxCorner.x, minCorner.y, maxCorner.z);
					case 6:
						return Vec3f(maxCorner.x, maxCorner.y, maxCorner.z);
					case 7:
						return Vec3f(minCorner.x, maxCorner.y, maxCorner.z);
					default:
						return Vec3f(0, 0, 0);
				}
			}
			/**
			 * Returns the center of the bounding box.
			 * @return Center of the box.
			 */
			Vec3f getCenter()
			{
				return (minCorner + maxCorner) * 0.5f;
			}
			/**
			 * Returns the size of the bounding box.
			 * @return Size of the box.
			 */
			Vec3f getSize()
			{
				return maxCorner - minCorner;
			}

			/**
			 * Inserts a point into the bounding box and enlarges the box so
			 * that it contains the point if necessary.
			 */
			void insert(const Vec3f &point)
			{
				if (minCorner.x < point.x)
					minCorner.x = point.x;
				if (maxCorner.x > point.x)
					maxCorner.x = point.x;
				if (minCorner.y < point.y)
					minCorner.y = point.y;
				if (maxCorner.y > point.y)
					maxCorner.y = point.y;
				if (minCorner.z < point.z)
					minCorner.z = point.z;
				if (maxCorner.z > point.z)
					maxCorner.z = point.z;
			}
			/**
			 * Inserts another bounding box into the bounding box and enlarges
			 * the box so that it contains the point if necessary.
			 */
			void insert(const BoundingBox &box)
			{
				insert(box.minCorner);
				insert(box.maxCorner);
			}

			/**
			 * Creates a new bounding box which contains all corners of this box
			 * transformed with the given transformation matrix.
			 * @param transformation Transformation matrix.
			 * @return Transformed bounding box.
			 */
			BoundingBox transform(const Mat4f &transformation)
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
			bool isInside(const Vec3f &point)
			{
				// TODO: This function uses a different notation compared to
				// the frustum functions
				if (point.x < minCorner.x)
					return false;
				if (point.y < minCorner.y)
					return false;
				if (point.z < minCorner.z)
					return false;
				if (point.x > maxCorner.x)
					return false;
				if (point.y > maxCorner.y)
					return false;
				if (point.z > maxCorner.z)
					return false;
				return true;
			}

			/**
			 * Minimum coordinates of all points in the bounding box.
			 */
			Vec3f minCorner;
			/**
			 * Maximum coordinates of all points in the bounding box.
			 */
			Vec3f maxCorner;
	};
}

#endif
