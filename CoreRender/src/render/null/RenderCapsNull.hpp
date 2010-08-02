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

#ifndef _CORERENDER_RENDER_NULL_RENDERCAPSNULL_HPP_INCLUDED_
#define _CORERENDER_RENDER_NULL_RENDERCAPSNULL_HPP_INCLUDED_

#include "CoreRender/render/RenderCaps.hpp"

namespace cr
{
namespace render
{
namespace null
{
	class RenderCapsNull : public RenderCaps
	{
		public:
			RenderCapsNull()
			{
				// Support a reasonable amount of features
				flags |= 1 << Flag::TextureFloat;
				flags |= 1 << Flag::TextureDepthStencil;
				flags |= 1 << Flag::TextureCompression;
				flags |= 1 << Flag::TextureDXT1;
				flags |= 1 << Flag::VertexHalfFloat;
				flags |= 1 << Flag::PointSprite;
				flags |= 1 << Flag::TextureRG;
				maxtexsize1d = 4096;
				maxtexsize2d[0] = 4096;
				maxtexsize2d[1] = 4096;
				maxtexsize3d[0] = 128;
				maxtexsize3d[1] = 128;
				maxtexsize3d[2] = 128;
				maxtexsizecube[0] = 4096;
				maxtexsizecube[1] = 4096;
				maxpointsize = 256.0f;
				minpointsize = 0.1f;
			}
			virtual ~RenderCapsNull()
			{
			}
		private:
	};
}
}
}

#endif
