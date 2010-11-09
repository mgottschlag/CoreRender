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

#ifndef _CORERENDER_SCENE_ANIMATIONFILE_HPP_INCLUDED_
#define _CORERENDER_SCENE_ANIMATIONFILE_HPP_INCLUDED_

#include "../core/StructPacking.hpp"

namespace cr
{
namespace scene
{
	struct AnimationFile
	{
		static const unsigned int version = 0;
		static const unsigned int tag = (int)'C' + 256 * 'R' + 65536 * 'A';
		static const unsigned int maxnamesize = 64;

		CORERENDER_PACK_BEGIN()
		struct Header
		{
			unsigned int tag;
			unsigned int version;
			unsigned int channelcount;
			unsigned int framecount;
			float framespersecond;
		}
		CORERENDER_PACK_END();

		CORERENDER_PACK_BEGIN()
		struct Frame
		{
			float rotation[4];
			float position[3];
			float scale[3];
		}
		CORERENDER_PACK_END();

		CORERENDER_PACK_BEGIN()
		struct Channel
		{
			char name[maxnamesize];
			int constant;
		}
		CORERENDER_PACK_END();
	};
}
}

#endif
