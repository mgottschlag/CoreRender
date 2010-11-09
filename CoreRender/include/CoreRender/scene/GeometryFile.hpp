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

#ifndef _CORERENDER_SCENE_GEOMETRYFILE_HPP_INCLUDED_
#define _CORERENDER_SCENE_GEOMETRYFILE_HPP_INCLUDED_

#include "../core/StructPacking.hpp"

#include <vector>

namespace cr
{
namespace scene
{
	struct GeometryFile
	{
		static const unsigned int version = 0;
		static const unsigned int tag = (int)'C' + 256 * 'R' + 65536 * 'G';
		static const unsigned int maxtexcoords = 8;
		static const unsigned int maxcolors = 4;

		CORERENDER_PACK_BEGIN()
		struct Header
		{
			unsigned int tag;
			unsigned int version;
			unsigned int vertexdatasize;
			unsigned int indexdatasize;
			unsigned int batchcount;
		}
		CORERENDER_PACK_END();

		struct AttribFlags
		{
			enum List
			{
				HasPositions = 0x1,
				HasNormals = 0x2,
				HasTangents = 0x4,
				HasBitangents = 0x8,
				HasTexCoords = 0x10,
				HasJoints = 0x20,
				HasColors = 0x40
			};
		};
		CORERENDER_PACK_BEGIN()
		struct AttribInfo
		{
			unsigned int flags;
			unsigned char stride;
			unsigned char posoffset;
			unsigned char normaloffset;
			unsigned char tangentoffset;
			unsigned char bitangentoffset;
			unsigned char jointoffset;
			unsigned char jointweightoffset;
			unsigned char texcoordcount;
			unsigned char texcoordoffset[maxtexcoords];
			unsigned char texcoordsize[maxtexcoords];
			unsigned char coloroffset[maxcolors];
			unsigned char colorcount;
			unsigned char padding[3];
		}
		CORERENDER_PACK_END();
		CORERENDER_PACK_BEGIN()
		struct GeometryInfo
		{
			unsigned int vertexoffset;
			unsigned int vertexsize;
			unsigned int indexoffset;
			unsigned int indexsize;
			unsigned int indextype;
			unsigned int indexcount;
			unsigned int basevertex;

			unsigned int jointcount;
		}
		CORERENDER_PACK_END();

		struct Batch
		{
			AttribInfo attribs;
			GeometryInfo geom;
			std::vector<float> jointmatrices;
		};
	};
}
}

#endif
