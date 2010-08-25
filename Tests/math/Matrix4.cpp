
#include "CoreRender/math/Matrix4.hpp"

#include <iostream>

using namespace cr;
using namespace math;

int main(int argc, char **argv)
{
	unsigned int errors = 0;
	{
		// Matrix4 * Vector4F
		Matrix4 m(0, 0, 0, 1,
		          1, 0, 0, 0,
		          0, 1, 0, 0,
		          0, 0, 1, 0);
		Vector4F v(1, 2, 3, 4);
		v = m * v;
		if (v != Vector4F(4, 1, 2, 3))
		{
			std::cout << "Matrix4 * Vector4F:" << std::endl;
			std::cout << "(" << v.x << ")   (0 0 0 1)   (1)" << std::endl;
			std::cout << "(" << v.x << ")   (1 0 0 0)   (2)" << std::endl;
			std::cout << "(" << v.z << ") = (0 1 0 0) * (3)" << std::endl;
			std::cout << "(" << v.w << ")   (0 0 1 0)   (4)" << std::endl;
			errors++;
		}
	}
	{
		// Matrix4 * Matrix4
		Matrix4 m1(1, 2, 3, 4,
		           5, 6, 7, 8,
		           9, 1, 2, 3,
		           4, 5, 6, 7);
		Matrix4 m2(3, 1, 4, 2,
		           9, 8, 6, 7,
		           1, 3, 5, 7,
		           2, 4, 6, 8);
		Matrix4 result(32, 42, 55, 69,
		               92, 106, 139, 165,
		               44, 35,  70, 63,
		               77, 90, 118, 141);
		if (result != m1 * m2)
		{
			std::cout << "Matrix4 * Matrix4" << std::endl;
			errors++;
		}
	}
	{
		// Identity
		Matrix4 m = Matrix4::Identity();
		Vector4F v(1, 2, 3, 4);
		Vector4F v2 = m * v;
		if (v2 != v)
		{
			std::cout << "Identity" << std::endl;
			errors++;
		}
	}
	{
		// Determinant
		Matrix4 m(1, 2, 3, 4,
		          5, 6, 7, 8,
		          1, 2, 0, 7,
		          9, 1, 3, 1);
		if (m.determinant() != 312)
		{
			std::cout << "determinant(): " << m.determinant()
				<< "(correct: 312)" << std::endl;
			errors++;
		}
	}
	{
		// Transposed
		Matrix4 m1(1, 2, 3, 4,
		           5, 6, 7, 8,
		           9, 1, 2, 3,
		           4, 5, 6, 7);
		Matrix4 m2(1, 5, 9, 4,
		           2, 6, 1, 5,
		           3, 7, 2, 6,
		           4, 8, 3, 7);
		if (m1.transposed() != m2)
		{
			std::cout << "transposed()" << std::endl;
			errors++;
		}
	}
	{
		// Translation (w = 1)
		Matrix4 m = Matrix4::TransMat(Vector3F(1, 2, 3));
		Vector4F v(1, 2, 3, 1);
		Vector4F res = m * v;
		if (res != Vector4F(2, 4, 6, 1))
		{
			std::cout << "Translation (w = 1):" << std::endl;
			std::cout << "(" << res.x << ")   (1)" << std::endl;
			std::cout << "(" << res.y << ")   (2)                 (1)" << std::endl;
			std::cout << "(" << res.z << ") = (3) translated with (2)" << std::endl;
			std::cout << "(" << res.w << ")   (1)                 (3)" << std::endl;
			errors++;
		}
	}
	{
		// Translation (w = 1)
		Matrix4 m = Matrix4::TransMat(Vector3F(1, 2, 3));
		Vector4F v(3, 1, 3, 2);
		Vector4F res = m * v;
		if (res != Vector4F(5, 5, 9, 2))
		{
			std::cout << "Translation (w = 1):" << std::endl;
			std::cout << "(" << res.x << ")   (3)" << std::endl;
			std::cout << "(" << res.y << ")   (1)                 (1)" << std::endl;
			std::cout << "(" << res.z << ") = (3) translated with (2)" << std::endl;
			std::cout << "(" << res.w << ")   (2)                 (3)" << std::endl;
			errors++;
		}
	}
	{
		// Scaling
		Matrix4 m = Matrix4::ScaleMat(Vector3F(1, 2, 3));
		Vector4F v(3, 1, 3, 2);
		Vector4F res = m * v;
		if (res != Vector4F(3, 2, 9, 2))
		{
			std::cout << "Scaling:" << std::endl;
			std::cout << "(" << res.x << ")   (3)" << std::endl;
			std::cout << "(" << res.y << ")   (1)             (1)" << std::endl;
			std::cout << "(" << res.z << ") = (3) scaled with (2)" << std::endl;
			std::cout << "(" << res.w << ")   (2)             (3)" << std::endl;
			errors++;
		}
	}
	{
		// Inverse
		Matrix4 m1(0, 0, 0, 1,
		           2, 0, 1, 0,
		           0, 1, 1, 0,
		           1, 0, 0, 3);
		Matrix4 m2(-3, 0, 0, 1,
		           -6, -1, 1, 2,
		           6, 1, 0, -2,
		           1, 0, 0, 0);
		if (m1.inverse() != m2)
		{
			std::cout << "Inverse" << std::endl;
			errors++;
		}
	}
	std::cout << errors << " errors." << std::endl;
	return errors;
}
