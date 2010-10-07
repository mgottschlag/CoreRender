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
/* Parts taken from the irrlicht engine under the following license:

  Copyright (C) 2002-2009 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

*/

#ifndef _CORERENDER_MATH_MATRIX4_HPP_INCLUDED_
#define _CORERENDER_MATH_MATRIX4_HPP_INCLUDED_

#include "Vector4.hpp"
#include "Matrix3.hpp"

#include <iostream>

namespace cr
{
namespace math
{
	class Matrix4
	{
		public:
			Matrix4()
			{
			}
			Matrix4(const Matrix4 &other)
			{
				for (unsigned int i = 0; i < 16; i++)
				{
					m[i] = other.m[i];
				}
			}
			Matrix4(const Matrix3 &other)
			{
				Matrix4 &m = *this;
				m(0, 0) = other(0, 0);
				m(0, 1) = other(0, 1);
				m(0, 2) = other(0, 2);
				m(0, 3) = 0;

				m(1, 0) = other(1, 0);
				m(1, 1) = other(1, 1);
				m(1, 2) = other(1, 2);
				m(1, 3) = 0;

				m(2, 0) = other(2, 0);
				m(2, 1) = other(2, 1);
				m(2, 2) = other(2, 2);
				m(2, 3) = 0;

				m(3, 0) = 0;
				m(3, 1) = 0;
				m(3, 2) = 0;
				m(3, 3) = 1;
			}
			Matrix4(float m00, float m01, float m02, float m03,
			        float m10, float m11, float m12, float m13,
			        float m20, float m21, float m22, float m23,
			        float m30, float m31, float m32, float m33)
			{
				Matrix4 &m = *this;
				m(0, 0) = m00;
				m(0, 1) = m01;
				m(0, 2) = m02;
				m(0, 3) = m03;

				m(1, 0) = m10;
				m(1, 1) = m11;
				m(1, 2) = m12;
				m(1, 3) = m13;

				m(2, 0) = m20;
				m(2, 1) = m21;
				m(2, 2) = m22;
				m(2, 3) = m23;

				m(3, 0) = m30;
				m(3, 1) = m31;
				m(3, 2) = m32;
				m(3, 3) = m33;
			}
			static Matrix4 TransMat(const Vector3F &translation)
			{
				float x = translation.x;
				float y = translation.y;
				float z = translation.z;
				return Matrix4(1, 0, 0, x,
				               0, 1, 0, y,
				               0, 0, 1, z,
				               0, 0, 0, 1);
			}
			static Matrix4 ScaleMat(const Vector3F &scale)
			{
				float x = scale.x;
				float y = scale.y;
				float z = scale.z;
				return Matrix4(x, 0, 0, 0,
				               0, y, 0, 0,
				               0, 0, z, 0,
				               0, 0, 0, 1);
			}
			static Matrix4 Identity()
			{
				return Matrix4(1, 0, 0, 0,
				               0, 1, 0, 0,
				               0, 0, 1, 0,
				               0, 0, 0, 1);
			}
			static Matrix4 PerspectiveFOV(float fov,
			                              float aspectratio,
			                              float near,
			                              float far)
			{
				float radians = fov / 180.0 * 3.1415;
				float t = tan(radians / 2);
				return Perspective(2 * t * near * aspectratio,
				                   2 * t * near,
				                   near,
				                   far);
			}
			static Matrix4 Perspective(float width,
			                           float height,
			                           float near,
			                           float far)
			{
				return Perspective(-width / 2,
				                   width / 2,
				                   -height / 2,
				                   height / 2,
				                   near,
				                   far);
			}
			static Matrix4 Perspective(float left,
			                           float right,
			                           float bottom,
			                           float top,
			                           float near,
			                           float far)
			{
				float w = right - left;
				float h = top - bottom;
				float r = right;
				float l = left;
				float t = top;
				float b = bottom;
				float n = near;
				float f = far;
				return Matrix4(2*n/w, 0,     (r+l)/w,       0,
				               0,     2*n/h, (t+b)/h,       0,
				               0,     0,     -(f+n)/(f-n),  -2*f*n/(f-n),
				               0,     0,     -1,            0);
			}
			static Matrix4 Ortho(float width,
			                     float height,
			                     float near,
			                     float far)
			{
				return Ortho(-width / 2,
				             width / 2,
				             -height / 2,
				             height / 2,
				             near,
				             far);
			}
			static Matrix4 Ortho(float left,
			                     float right,
			                     float bottom,
			                     float top,
			                     float near,
			                     float far)
			{
				float w = right - left;
				float h = top - bottom;
				float r = right;
				float l = left;
				float t = top;
				float b = bottom;
				float n = near;
				float f = far;
				return Matrix4(2/w, 0,   0,        -(r+l)/w,
				               0,   2/h, 0,        -(t+b)/h,
				               0,   0,   -2/(f-n), -(f+n)/(f-n),
				               0,   0,   0,        1);
			}

			float determinant() const
			{
				const Matrix4 &m = *this;
				return (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
				       (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
				       (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
				       (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
				       (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
				       (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));
			}

			Matrix4 inverse() const
			{
				Matrix4 out;
				float d = determinant();
				if (d == 0)
					return out;
				d = 1 / d;

				const Matrix4 &m = *this;
				out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
				                 m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
				                 m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
				out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
				                 m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
				                 m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
				out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
				                 m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
				                 m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
				out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
				                 m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
				                 m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
				out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
				                 m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
				                 m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
				out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
				                 m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
				                 m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
				out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
				                 m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
				                 m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
				out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
				                 m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
				                 m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
				out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
				                 m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
				                 m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
				out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
				                 m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
				                 m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
				out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
				                 m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
				                 m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
				out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
				                 m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
				                 m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
				out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
				                 m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
				                 m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
				out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
				                 m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
				                 m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
				out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
				                 m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
				                 m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
				out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
				                 m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
				                 m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));
				return out;
			}
			Matrix4 transposed() const
			{
				const Matrix4 &m = *this;
				Matrix4 newmat;
				newmat(0, 0) = m(0, 0);
				newmat(1, 0) = m(0, 1);
				newmat(2, 0) = m(0, 2);
				newmat(3, 0) = m(0, 3);

				newmat(0, 1) = m(1, 0);
				newmat(1, 1) = m(1, 1);
				newmat(2, 1) = m(1, 2);
				newmat(3, 1) = m(1, 3);

				newmat(0, 2) = m(2, 0);
				newmat(1, 2) = m(2, 1);
				newmat(2, 2) = m(2, 2);
				newmat(3, 2) = m(2, 3);

				newmat(0, 3) = m(3, 0);
				newmat(1, 3) = m(3, 1);
				newmat(2, 3) = m(3, 2);
				newmat(3, 3) = m(3, 3);
				return newmat;
			}

			Vector3F transformPoint(const Vector3F &point) const
			{
				Vector4F transformed = *this * Vector4F(point.x,
				                                        point.y,
				                                        point.z,
				                                        1.0f);
				return Vector3F(transformed.x / transformed.w,
				                transformed.y / transformed.w,
				                transformed.z / transformed.w);
			}

			Vector4F operator*(const Vector4F &v) const
			{
				Vector4F out;
				const Matrix4 &m = *this;
				out.x = v.x * m(0, 0) + v.y * m(0, 1) + v.z * m(0, 2) + v.w * m(0, 3);
				out.y = v.x * m(1, 0) + v.y * m(1, 1) + v.z * m(1, 2) + v.w * m(1, 3);
				out.z = v.x * m(2, 0) + v.y * m(2, 1) + v.z * m(2, 2) + v.w * m(2, 3);
				out.w = v.x * m(3, 0) + v.y * m(3, 1) + v.z * m(3, 2) + v.w * m(3, 3);
				return out;
			}

			Matrix4 operator*(const Matrix4 &o) const
			{
				Matrix4 out;
				const Matrix4 &m = *this;
				out(0, 0) = m(0, 0) * o(0, 0) + m(0, 1) * o(1, 0) + m(0, 2) * o(2, 0) + m(0, 3) * o(3, 0);
				out(0, 1) = m(0, 0) * o(0, 1) + m(0, 1) * o(1, 1) + m(0, 2) * o(2, 1) + m(0, 3) * o(3, 1);
				out(0, 2) = m(0, 0) * o(0, 2) + m(0, 1) * o(1, 2) + m(0, 2) * o(2, 2) + m(0, 3) * o(3, 2);
				out(0, 3) = m(0, 0) * o(0, 3) + m(0, 1) * o(1, 3) + m(0, 2) * o(2, 3) + m(0, 3) * o(3, 3);

				out(1, 0) = m(1, 0) * o(0, 0) + m(1, 1) * o(1, 0) + m(1, 2) * o(2, 0) + m(1, 3) * o(3, 0);
				out(1, 1) = m(1, 0) * o(0, 1) + m(1, 1) * o(1, 1) + m(1, 2) * o(2, 1) + m(1, 3) * o(3, 1);
				out(1, 2) = m(1, 0) * o(0, 2) + m(1, 1) * o(1, 2) + m(1, 2) * o(2, 2) + m(1, 3) * o(3, 2);
				out(1, 3) = m(1, 0) * o(0, 3) + m(1, 1) * o(1, 3) + m(1, 2) * o(2, 3) + m(1, 3) * o(3, 3);

				out(2, 0) = m(2, 0) * o(0, 0) + m(2, 1) * o(1, 0) + m(2, 2) * o(2, 0) + m(2, 3) * o(3, 0);
				out(2, 1) = m(2, 0) * o(0, 1) + m(2, 1) * o(1, 1) + m(2, 2) * o(2, 1) + m(2, 3) * o(3, 1);
				out(2, 2) = m(2, 0) * o(0, 2) + m(2, 1) * o(1, 2) + m(2, 2) * o(2, 2) + m(2, 3) * o(3, 2);
				out(2, 3) = m(2, 0) * o(0, 3) + m(2, 1) * o(1, 3) + m(2, 2) * o(2, 3) + m(2, 3) * o(3, 3);

				out(3, 0) = m(3, 0) * o(0, 0) + m(3, 1) * o(1, 0) + m(3, 2) * o(2, 0) + m(3, 3) * o(3, 0);
				out(3, 1) = m(3, 0) * o(0, 1) + m(3, 1) * o(1, 1) + m(3, 2) * o(2, 1) + m(3, 3) * o(3, 1);
				out(3, 2) = m(3, 0) * o(0, 2) + m(3, 1) * o(1, 2) + m(3, 2) * o(2, 2) + m(3, 3) * o(3, 2);
				out(3, 3) = m(3, 0) * o(0, 3) + m(3, 1) * o(1, 3) + m(3, 2) * o(2, 3) + m(3, 3) * o(3, 3);

				return out;
			}

			float &operator()(int row, int column)
			{
				return m[row + column * 4];
			}
			const float &operator()(int row, int column) const
			{
				return m[row + column * 4];
			}

			bool operator==(const Matrix4 &other) const
			{
				for (unsigned int i = 0; i < 16; i++)
				{
					if (fabs(m[i] - other.m[i]) > 0.0001f)
						return false;
				}
				return true;
			}
			bool operator!=(const Matrix4 &other) const
			{
				return !(*this == other);
			}

			/**
			 * Matrix data for linear access. The values are stored in a
			 * column-major format, so the first column is at indices 0-3.
			 */
			float m[16];
	};
}
}

#endif
