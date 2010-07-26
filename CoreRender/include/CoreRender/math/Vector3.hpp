/*
Copyright (C) 2009, Mathias Gottschlag

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

#ifndef _CORERENDER_MATH_VECTOR3_HPP_INCLUDED_
#define _CORERENDER_MATH_VECTOR3_HPP_INCLUDED_

#include <cmath>
#include <string>
#include <cstdlib>

namespace cr
{
namespace math
{
	/**
	 * Class for three-dimensional vectors, used for positions/rotation.
	 */
	template<typename T> class Vector3
	{
		public:
			/**
			 * Constructor.
			 */
			Vector3() : x(0), y(0), z(0)
			{
			}
			/**
			 * Constructor.
			 */
			template <class T2> Vector3(T2 x, T2 y, T2 z) : x(x), y(y), z(z)
			{
			}
			/**
			 * Constructor.
			 */
			template <class T2> Vector3(const Vector3<T2> &v)
				: x(v.x), y(v.y), z(v.z)
			{
			}
			/**
			 * Constructor.
			 */
			Vector3(const char *s)
			{
				set(s);
			}
			/**
			 * Constructor.
			 */
			Vector3(const std::string &s)
			{
				set(s);
			}

			/**
			 * Returns the squared length of the vector. Much less
			 * computationally expensive than getLength(). Good for comparing
			 * vector lengths.
			 */
			float getSquaredLength()
			{
				return x * x + y * y + z * z;
			}
			/**
			 * Returns the length of the vector.
			 */
			float getLength()
			{
				return sqrt(getSquaredLength());
			}

			/**
			 * Rotates the vector around the z axis.
			 * @param angle Angle in degrees.
			 */
			void rotateXY(float angle)
			{
				angle *= 3.1415f / 180.0f;
				float s = sin(angle);
				float c = cos(angle);
				*this = Vector3<T>(c * x - s * y, s * x + c * y, z);
			}
			/**
			 * Rotates the vector around the y axis.
			 * @param angle Angle in degrees.
			 */
			void rotateXZ(float angle)
			{
				angle *= 3.1415f / 180.0f;
				float s = sin(angle);
				float c = cos(angle);
				*this = Vector3<T>(-c * x + s * z, y, s * x + c * z);
			}
			/**
			 * Rotates the vector around the x axis.
			 * @param angle Angle in degrees.
			 */
			void rotateYZ(float angle)
			{
				angle *= 3.1415f / 180.0f;
				float s = sin(angle);
				float c = cos(angle);
				*this = Vector3<T>(x, c * y - s * z, s * y + c * z);
			}
			/**
			 * Rotates the vector using the given angles. First z rotation is
			 * applied, then x and then z.
			 */
			void rotate(const Vector3<float> &angles)
			{
				rotateXY(angles.z);
				rotateYZ(angles.x);
				rotateXZ(angles.y);
			}

			/**
			 * Dot product.
			 */
			float dot(const Vector3<T> other)
			{
				return x * other.x + y * other.y + z * other.z;
			}
			/**
			 * Cross product.
			 */
			Vector3<T> cross(const Vector3<T> other)
			{
				return Vector3<T>(y * other.z - z * other.y,
				                  z * other.x - x * other.z,
				                  x * other.y - y * other.x);
			}

			/**
			 * Returns the rotation of this vector relative to (0/0/1).
			 */
			Vector3<float> getAngle()
			{
				Vector3<float> angle;
				angle.y = atan2(x, z) * 180.0f / 3.1415f;
				angle.x = atan2(sqrt(x * x + z * z), y) * 180.0f / 3.1415f - 90;
				if (angle.y < 0)
					angle.y += 360;
				if (angle.y >= 360)
					angle.y -= 360;
				if (angle.x < 0)
					angle.x += 360;
				if (angle.x >= 360)
					angle.x -= 360;
				return angle;
			}

			/**
			 * Sets the vector to the linear interpolation between a and b.
			 * \param a First vector
			 * \param b Second vector
			 * \param d Interpolation factor. If it is 0, then the vector will
			 * be set to a, if it is 1, the vector will be b.
			 */
			Vector3<T> &interpolate(const Vector3<T> &a, const Vector3<T> &b,
				float d)
			{
				Vector3<T> dv = b - a;
				*this = a + dv * d;
				return *this;
			}

			/**
			 * Reads the vector from a string in the form "x/y/z".
			 */
			void set(const std::string &s)
			{
				size_t separator = s.find("/");
				size_t separator2;
				if (separator != std::string::npos)
					separator2 = s.find("/", separator + 1);
				if ((separator == std::string::npos)
					|| (separator2 == std::string::npos))
				{
					x = 0;
					y = 0;
					z = 0;
				}
				else
				{
					x = (T)atof(s.c_str());
					const char *ys = s.c_str() + separator + 1;
					y = (T)atof(ys);
					const char *zs = s.c_str() + separator2 + 1;
					z = (T)atof(zs);
				}
			}

			template<typename T2> Vector3<T> operator*(T2 s) const
			{
				return Vector3<T>((T)(x * s), (T)(y * s), (T)(z * s));
			}
			template<typename T2> Vector3<T> &operator*=(T2 s)
			{
				x = x * s;
				y = y * s;
				z = z * s;
				return *this;
			}
			template<typename T2> Vector3<T> operator/(T2 s) const
			{
				return Vector3<T>((T)(x / s), (T)(y / s), (T)(z / s));
			}
			template<typename T2> Vector3<T> &operator/=(T2 s)
			{
				x = x / s;
				y = y / s;
				z = z / s;
				return *this;
			}
			template<typename T2> Vector3<T> operator+(const Vector3<T2> &v) const
			{
				return Vector3<T>((T)(x + v.x), (T)(y + v.y), (T)(z + v.z));
			}
			template<typename T2> Vector3<T> operator-(const Vector3<T2> &v) const
			{
				return Vector3<T>((T)(x - v.x), (T)(y - v.y), (T)(z - v.z));
			}
			template<typename T2> Vector3<T> &operator=(const Vector3<T2> &v)
			{
				x = (T)v.x;
				y = (T)v.y;
				z = (T)v.z;
				return *this;
			}
			Vector3<T> &operator=(const std::string &s)
			{
				set(s);
				return *this;
			}
			Vector3<T> &operator=(const char *s)
			{
				set(s);
				return *this;
			}
			template<typename T2> Vector3<T> &operator+=(const Vector3<T2> &v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}
			template<typename T2> Vector3<T> &operator-=(const Vector3<T2> &v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}
			template<typename T2> bool operator==(const Vector3<T2> &v) const
			{
				return x == v.x && y == v.y && z == v.z;
			}
			template<typename T2> bool operator!=(const Vector3<T2> &v) const
			{
				return x != v.x || y != v.y || z != v.z;
			}

			T x;
			T y;
			T z;
	};

	typedef Vector3<int> Vector3I;
	typedef Vector3<float> Vector3F;
	typedef Vector3<double> Vector3D;
}
}

#endif
