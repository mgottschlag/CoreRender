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

#ifndef _CORERENDER_MATH_MATRIX3_HPP_INCLUDED_
#define _CORERENDER_MATH_MATRIX3_HPP_INCLUDED_

#include "Vector3.hpp"

namespace cr
{
namespace math
{
	class Matrix3
	{
		public:
			Matrix3()
			{
			}
			Matrix3(const Matrix3 &other)
			{
				for (unsigned int i = 0; i < 9; i++)
				{
					m[i] = other.m[i];
				}
			}
			Matrix3(float m00, float m01, float m02,
			        float m10, float m11, float m12,
			        float m20, float m21, float m22)
			{
				Matrix3 &m = *this;
				m(0, 0) = m00;
				m(0, 1) = m01;
				m(0, 2) = m02;

				m(1, 0) = m10;
				m(1, 1) = m11;
				m(1, 2) = m12;

				m(2, 0) = m20;
				m(2, 1) = m21;
				m(2, 2) = m22;
			}
			static Matrix3 Identity()
			{
				return Matrix3(1, 0, 0,
				               0, 1, 0,
				               0, 0, 1);
			}

			Matrix3 operator*(const Matrix3 &o) const
			{
				const Matrix3 &m = *this;
				return Matrix3(m(0, 0) * o(0, 0) + m(1, 0) * o(0, 1) + m(2, 0) * o(0, 2),
				               m(0, 1) * o(0, 0) + m(1, 1) * o(0, 1) + m(2, 1) * o(0, 2),
				               m(0, 2) * o(0, 0) + m(1, 2) * o(0, 1) + m(2, 2) * o(0, 2),

				               m(0, 0) * o(1, 0) + m(1, 0) * o(1, 1) + m(2, 0) * o(1, 2),
				               m(0, 1) * o(1, 0) + m(1, 1) * o(1, 1) + m(2, 1) * o(1, 2),
				               m(0, 2) * o(1, 0) + m(1, 2) * o(1, 1) + m(2, 2) * o(1, 2),

				               m(0, 0) * o(2, 0) + m(1, 0) * o(2, 1) + m(2, 0) * o(2, 2),
				               m(0, 1) * o(2, 0) + m(1, 1) * o(2, 1) + m(2, 1) * o(2, 2),
				               m(0, 2) * o(2, 0) + m(1, 2) * o(2, 1) + m(2, 2) * o(2, 2));
			}

			float &operator()(int row, int column)
			{
				return m[row + column * 3];
			}
			const float &operator()(int row, int column) const
			{
				return m[row + column * 3];
			}

			/**
			 * Matrix data for linear access. The values are stored in a
			 * row-major format, so the first row is at indices 0-2.
			 */
			float m[9];
	};
}
}

#endif
