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

#ifndef _CORERENDER_RENDER_SHADERVARIABLETYPE_HPP_INCLUDED_
#define _CORERENDER_RENDER_SHADERVARIABLETYPE_HPP_INCLUDED_

namespace cr
{
namespace render
{
	struct ShaderVariableType
	{
		enum List
		{
			Invalid,
			Float,
			Float2,
			Float3,
			Float4,
			Float4x4,
			Float3x3,
			Float4x3,
			Float3x4
			// TODO: 2x2, 3x2, 4x2, 2x3, 2x4
		};

		static unsigned int getSize(ShaderVariableType::List type)
		{
			switch (type)
			{
				case Invalid:
					return 0;
				case Float:
					return 1;
				case Float2:
					return 2;
				case Float3:
					return 3;
				case Float4:
					return 4;
				case Float4x4:
					return 16;
				case Float3x3:
					return 9;
				case Float4x3:
				case Float3x4:
					return 12;
				default:
					return 0;
			}
		}
	};
}
}

#endif
