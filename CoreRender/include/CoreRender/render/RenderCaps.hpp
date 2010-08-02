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

#ifndef _CORERENDER_RENDER_RENDERCAPS_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERCAPS_HPP_INCLUDED_

namespace cr
{
namespace render
{
	/**
	 * This class is filled with values by creating an instance of a derived
	 * class and then setting the instance of this class to the content of the
	 * derived class via RenderCaps::operator=().
	 */
	class RenderCaps
	{
		public:
			struct Param
			{
				enum List
				{
					MaxTextureWidth1D = 0,
					MaxTextureWidth2D,
					MaxTextureHeight2D,
					MaxTextureWidth3D,
					MaxTextureHeight3D,
					MaxTextureDepth3D,
					MaxTextureWidthCube,
					MaxTextureHeightCube,
					MinPointSize,
					MaxPointSize,
					Count
				};
			};
			struct Flag
			{
				enum List
				{
					GeometryShader = 0,
					TesselationShader,
					OcclusionQuery,
					TimerQuery,
					TextureFloat,
					TextureDepthStencil,
					TextureCompression,
					TextureDXT1,
					TextureRG,
					VertexHalfFloat,
					PointSprite,
					Count
				};
			};

			RenderCaps()
				: flags(0), maxtexsize1d(0), maxpointsize(0.0f),
				minpointsize(0.0f)
			{
				maxtexsize2d[0] = 0;
				maxtexsize2d[1] = 0;
				maxtexsize3d[0] = 0;
				maxtexsize3d[1] = 0;
				maxtexsize3d[2] = 0;
				maxtexsizecube[0] = 0;
				maxtexsizecube[1] = 0;
			}
			virtual ~RenderCaps()
			{
			}

			bool getFlag(Flag::List flag) const
			{
				return (flags & (1 << (int)flag)) != 0;
			}
			float getParamF(Param::List param) const
			{
				switch (param)
				{
					case Param::MaxTextureWidth1D:
						return (float)maxtexsize1d;
					case Param::MaxTextureWidth2D:
						return (float)maxtexsize2d[0];
					case Param::MaxTextureHeight2D:
						return (float)maxtexsize2d[1];
					case Param::MaxTextureWidth3D:
						return (float)maxtexsize3d[0];
					case Param::MaxTextureHeight3D:
						return (float)maxtexsize3d[1];
					case Param::MaxTextureDepth3D:
						return (float)maxtexsize3d[2];
					case Param::MaxTextureWidthCube:
						return (float)maxtexsizecube[0];
					case Param::MaxTextureHeightCube:
						return (float)maxtexsizecube[1];
					case Param::MinPointSize:
						return minpointsize;
					case Param::MaxPointSize:
						return maxpointsize;
					default:
						return 0.0f;
				}
			}
			int getParamI(Param::List param) const
			{
				switch (param)
				{
					case Param::MaxTextureWidth1D:
						return maxtexsize1d;
					case Param::MaxTextureWidth2D:
						return maxtexsize2d[0];
					case Param::MaxTextureHeight2D:
						return maxtexsize2d[1];
					case Param::MaxTextureWidth3D:
						return maxtexsize3d[0];
					case Param::MaxTextureHeight3D:
						return maxtexsize3d[1];
					case Param::MaxTextureDepth3D:
						return maxtexsize3d[2];
					case Param::MaxTextureWidthCube:
						return maxtexsizecube[0];
					case Param::MaxTextureHeightCube:
						return maxtexsizecube[1];
					case Param::MinPointSize:
						return (int)minpointsize;
					case Param::MaxPointSize:
						return (int)maxpointsize;
					default:
						return 0;
				}
			}

			RenderCaps &operator=(const RenderCaps &caps)
			{
				flags = caps.flags;
				maxtexsize1d = caps.maxtexsize1d;
				maxtexsize2d[0] = caps.maxtexsize2d[0];
				maxtexsize2d[1] = caps.maxtexsize2d[1];
				maxtexsize3d[0] = caps.maxtexsize3d[0];
				maxtexsize3d[1] = caps.maxtexsize3d[1];
				maxtexsize3d[2] = caps.maxtexsize3d[2];
				maxtexsizecube[0] = caps.maxtexsizecube[0];
				maxtexsizecube[1] = caps.maxtexsizecube[1];
				maxpointsize = caps.maxpointsize;
				minpointsize = caps.minpointsize;
				return *this;
			}
		protected:
			unsigned int flags;
			
			unsigned int maxtexsize1d;
			unsigned int maxtexsize2d[2];
			unsigned int maxtexsize3d[3];
			unsigned int maxtexsizecube[2];
			float maxpointsize;
			float minpointsize;
	};
}
}

#endif
