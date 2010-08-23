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

#ifndef _CORERENDER_MATH_MATRIX4_HPP_INCLUDED_
#define _CORERENDER_MATH_MATRIX4_HPP_INCLUDED_

#include "Vector3.hpp"
#include "Matrix3.hpp"

namespace cr
{
namespace math
{
	class Matrix4
	{
		public:
			Matrix4()
			{
				// TODO: Delete this
				m[0][0] = 1;
				m[0][1] = 0;
				m[0][2] = 0;
				m[0][3] = 0;

				m[1][0] = 0;
				m[1][1] = 1;
				m[1][2] = 0;
				m[1][3] = 0;

				m[2][0] = 0;
				m[2][1] = 0;
				m[2][2] = 1;
				m[2][3] = 0;

				m[3][0] = 0;
				m[3][1] = 0;
				m[3][2] = 0;
				m[3][3] = 1;
			}
			Matrix4(const Matrix4 &other)
			{
				for (unsigned int i = 0; i < 16; i++)
				{
					x[i] = other.x[i];
				}
			}
			Matrix4(const Matrix3 &other)
			{
				m[0][0] = other.m[0][0];
				m[0][1] = other.m[0][1];
				m[0][2] = other.m[0][2];
				m[0][3] = 0;

				m[1][0] = other.m[1][0];
				m[1][1] = other.m[1][1];
				m[1][2] = other.m[1][2];
				m[1][3] = 0;

				m[2][0] = other.m[2][0];
				m[2][1] = other.m[2][1];
				m[2][2] = other.m[2][2];
				m[2][3] = 0;

				m[3][0] = 0;
				m[3][1] = 0;
				m[3][2] = 0;
				m[3][3] = 1;
			}
			Matrix4(float m00, float m01, float m02, float m03,
			        float m10, float m11, float m12, float m13,
			        float m20, float m21, float m22, float m23,
			        float m30, float m31, float m32, float m33)
			{
				m[0][0] = m00;
				m[0][1] = m01;
				m[0][2] = m02;
				m[0][3] = m03;

				m[1][0] = m10;
				m[1][1] = m11;
				m[1][2] = m12;
				m[1][3] = m13;

				m[2][0] = m20;
				m[2][1] = m21;
				m[2][2] = m22;
				m[2][3] = m23;

				m[3][0] = m30;
				m[3][1] = m31;
				m[3][2] = m32;
				m[3][3] = m33;
			}
			static Matrix4 TransMat(const Vector3F &translation)
			{
				float x = translation.x;
				float y = translation.y;
				float z = translation.z;
				return Matrix4(1, 0, 0, 0,
				               0, 1, 0, 0,
				               0, 0, 1, 0,
				               x, y, z, 1);
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

			float determinant()
			{
				// Laplace expansion along the first row
				float d = m[0][0] *
				          (m[1][1] * m[2][2] * m[3][3]
				         + m[1][2] * m[2][3] * m[3][1]
				         + m[1][3] * m[2][1] * m[3][2]
				         - m[1][3] * m[2][2] * m[3][1]
				         - m[1][2] * m[2][1] * m[3][3]
				         - m[1][1] * m[2][3] * m[3][2])
				        - m[1][0] *
				          (m[0][1] * m[2][2] * m[3][3]
				         + m[0][2] * m[2][3] * m[3][1]
				         + m[0][3] * m[2][1] * m[3][2]
				         - m[0][3] * m[2][2] * m[3][1]
				         - m[0][2] * m[2][1] * m[3][3]
				         - m[0][1] * m[2][3] * m[3][2])
				        + m[2][0] *
				          (m[0][1] * m[1][2] * m[3][3]
				         + m[0][2] * m[1][3] * m[3][1]
				         + m[0][3] * m[1][1] * m[3][2]
				         - m[0][3] * m[1][2] * m[3][1]
				         - m[0][2] * m[1][1] * m[3][3]
				         - m[0][1] * m[1][3] * m[3][2])
				        + m[3][0] *
				          (m[0][1] * m[1][2] * m[2][3]
				         + m[0][2] * m[1][3] * m[2][1]
				         + m[0][3] * m[1][1] * m[2][2]
				         - m[0][3] * m[1][2] * m[2][1]
				         - m[0][2] * m[1][1] * m[2][3]
				         - m[0][1] * m[1][3] * m[2][2]);
				return d;
			}

			Matrix4 inverse()
			{
				Matrix4 out;
				float d = determinant();
				if (d == 0)
					return out;
				d = 1 / d;

				out.m[0][0] = d * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) +
				                   m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) +
				                   m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
				out.m[0][1] = d * (m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
				                   m[2][2] * (m[0][3] * m[3][1] - m[0][1] * m[3][3]) +
				                   m[2][3] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]));
				out.m[0][2] = d * (m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]) +
				                   m[3][2] * (m[0][3] * m[1][1] - m[0][1] * m[1][3]) +
				                   m[3][3] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
				out.m[0][3] = d * (m[0][1] * (m[1][3] * m[2][2] - m[1][2] * m[2][3]) +
				                   m[0][2] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) +
				                   m[0][3] * (m[1][2] * m[2][1] - m[1][1] * m[2][2]));
				out.m[1][0] = d * (m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
				                   m[1][3] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) +
				                   m[1][0] * (m[2][3] * m[3][2] - m[2][2] * m[3][3]));
				out.m[1][1] = d * (m[2][2] * (m[0][0] * m[3][3] - m[0][3] * m[3][0]) +
				                   m[2][3] * (m[0][2] * m[3][0] - m[0][0] * m[3][2]) +
				                   m[2][0] * (m[0][3] * m[3][2] - m[0][2] * m[3][3]));
				out.m[1][2] = d * (m[3][2] * (m[0][0] * m[1][3] - m[0][3] * m[1][0]) +
				                   m[3][3] * (m[0][2] * m[1][0] - m[0][0] * m[1][2]) +
				                   m[3][0] * (m[0][3] * m[1][2] - m[0][2] * m[1][3]));
				out.m[1][3] = d * (m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) +
				                   m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
				                   m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));
				out.m[2][0] = d * (m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]) +
				                   m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
				                   m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]));
				out.m[2][1] = d * (m[2][3] * (m[0][0] * m[3][1] - m[0][1] * m[3][0]) +
				                   m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
				                   m[2][1] * (m[0][3] * m[3][0] - m[0][0] * m[3][3]));
				out.m[2][2] = d * (m[3][3] * (m[0][0] * m[1][1] - m[0][1] * m[1][0]) +
				                   m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]) +
				                   m[3][1] * (m[0][3] * m[1][0] - m[0][0] * m[1][3]));
				out.m[2][3] = d * (m[0][3] * (m[1][1] * m[2][0] - m[1][0] * m[2][1]) +
				                   m[0][0] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) +
				                   m[0][1] * (m[1][0] * m[2][3] - m[1][3] * m[2][0]));
				out.m[3][0] = d * (m[1][0] * (m[2][2] * m[3][1] - m[2][1] * m[3][2]) +
				                   m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
				                   m[1][2] * (m[2][1] * m[3][0] - m[2][0] * m[3][1]));
				out.m[3][1] = d * (m[2][0] * (m[0][2] * m[3][1] - m[0][1] * m[3][2]) +
				                   m[2][1] * (m[0][0] * m[3][2] - m[0][2] * m[3][0]) +
				                   m[2][2] * (m[0][1] * m[3][0] - m[0][0] * m[3][1]));
				out.m[3][2] = d * (m[3][0] * (m[0][2] * m[1][1] - m[0][1] * m[1][2]) +
				                   m[3][1] * (m[0][0] * m[1][2] - m[0][2] * m[1][0]) +
				                   m[3][2] * (m[0][1] * m[1][0] - m[0][0] * m[1][1]));
				out.m[3][3] = d * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) +
				                   m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) +
				                   m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
				return out;
			}
			Matrix4 transposed()
			{
				Matrix4 newmat;
				newmat.m[0][0] = m[0][0];
				newmat.m[1][0] = m[0][1];
				newmat.m[2][0] = m[0][2];
				newmat.m[3][0] = m[0][3];

				newmat.m[0][1] = m[1][0];
				newmat.m[1][1] = m[1][1];
				newmat.m[2][1] = m[1][2];
				newmat.m[3][1] = m[1][3];

				newmat.m[0][2] = m[2][0];
				newmat.m[1][2] = m[2][1];
				newmat.m[2][2] = m[2][2];
				newmat.m[3][2] = m[2][3];

				newmat.m[0][3] = m[3][0];
				newmat.m[1][3] = m[3][1];
				newmat.m[2][3] = m[3][2];
				newmat.m[3][3] = m[3][3];
				return newmat;
			}

			Matrix4 operator*(const Matrix4 &o)
			{
				return Matrix4(m[0][0] * o.m[0][0] + m[1][0] * o.m[0][1] + m[2][0] * o.m[0][2] + m[3][0] * o.m[0][3],
				               m[0][1] * o.m[0][0] + m[1][1] * o.m[0][1] + m[2][1] * o.m[0][2] + m[3][1] * o.m[0][3],
				               m[0][2] * o.m[0][0] + m[1][2] * o.m[0][1] + m[2][2] * o.m[0][2] + m[3][2] * o.m[0][3],
				               m[0][3] * o.m[0][0] + m[1][3] * o.m[0][1] + m[2][3] * o.m[0][2] + m[3][3] * o.m[0][3],

				               m[0][0] * o.m[1][0] + m[1][0] * o.m[1][1] + m[2][0] * o.m[1][2] + m[3][0] * o.m[1][3],
				               m[0][1] * o.m[1][0] + m[1][1] * o.m[1][1] + m[2][1] * o.m[1][2] + m[3][1] * o.m[1][3],
				               m[0][2] * o.m[1][0] + m[1][2] * o.m[1][1] + m[2][2] * o.m[1][2] + m[3][2] * o.m[1][3],
				               m[0][3] * o.m[1][0] + m[1][3] * o.m[1][1] + m[2][3] * o.m[1][2] + m[3][3] * o.m[1][3],

				               m[0][0] * o.m[2][0] + m[1][0] * o.m[2][1] + m[2][0] * o.m[2][2] + m[3][0] * o.m[2][3],
				               m[0][1] * o.m[2][0] + m[1][1] * o.m[2][1] + m[2][1] * o.m[2][2] + m[3][1] * o.m[2][3],
				               m[0][2] * o.m[2][0] + m[1][2] * o.m[2][1] + m[2][2] * o.m[2][2] + m[3][2] * o.m[2][3],
				               m[0][3] * o.m[2][0] + m[1][3] * o.m[2][1] + m[2][3] * o.m[2][2] + m[3][3] * o.m[2][3],

				               m[0][0] * o.m[3][0] + m[1][0] * o.m[3][1] + m[2][0] * o.m[3][2] + m[3][0] * o.m[3][3],
				               m[0][1] * o.m[3][0] + m[1][1] * o.m[3][1] + m[2][1] * o.m[3][2] + m[3][1] * o.m[3][3],
				               m[0][2] * o.m[3][0] + m[1][2] * o.m[3][1] + m[2][2] * o.m[3][2] + m[3][2] * o.m[3][3],
				               m[0][3] * o.m[3][0] + m[1][3] * o.m[3][1] + m[2][3] * o.m[3][2] + m[3][3] * o.m[3][3]);
			}

			union
			{
				/**
				 * Matrix data. First index is the row, the second the column.
				 */
				float m[4][4];
				/**
				 * Matrix data for linear access.
				 */
				float x[16];
			};
	};
}
}

#endif
