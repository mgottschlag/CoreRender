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

#ifndef _CORERENDER_RENDER_DEFAULTUNIFORM_HPP_INCLUDED_
#define _CORERENDER_RENDER_DEFAULTUNIFORM_HPP_INCLUDED_

#include "../math/Matrix4.hpp"

namespace cr
{
namespace render
{
	struct DefaultUniformName
	{
		enum List
		{
			TransMatrix,
			TransMatrixInv,
			WorldMatrix,
			WorldMatrixInv,
			ViewMatrix,
			ViewMatrixInv,
			ProjMatrix,
			ProjMatrixInv,
			WorldNormalMatrix,
			WorldNormalMatrixInv,
			ViewPosition,
			Count
		};

		static const char *getName(List uniform)
		{
			switch (uniform)
			{
				case TransMatrix:
					return "transMat";
				case TransMatrixInv:
					return "transMatInv";
				case WorldMatrix:
					return "worldMat";
				case WorldMatrixInv:
					return "worldMatInv";
				case ViewMatrix:
					return "viewMat";
				case ViewMatrixInv:
					return "viewMatInv";
				case ProjMatrix:
					return "projMat";
				case ProjMatrixInv:
					return "projMatInv";
				case WorldNormalMatrix:
					return "worldNormalMat";
				case WorldNormalMatrixInv:
					return "worldNormalMatInv";
				case ViewPosition:
					return "viewPos";
				default:
					return "";
			}
		}
	};

	struct DefaultUniform
	{
		DefaultUniform(DefaultUniformName::List name, const math::Matrix4 &m)
			: name(name)
		{
			for (unsigned int i = 0; i < 16; i++)
				data[i] = m.m[i];
		}
		DefaultUniform(DefaultUniformName::List name, const math::Vector3F &v)
			: name(name)
		{
			data[0] = v.x;
			data[1] = v.y;
			data[2] = v.z;
		}
		DefaultUniformName::List name;
		float data[16];
	};

	struct DefaultUniformLocations
	{
		DefaultUniformLocations()
		{
			for (unsigned int i = 0; i < DefaultUniformName::Count; i++)
				location[i] = -1;
		}
		int location[DefaultUniformName::Count];
	};

	struct DefaultUniformValues
	{
		float uniforms[DefaultUniformName::Count][16];
	};
}
}

#endif
