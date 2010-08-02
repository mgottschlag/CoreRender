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

#include "RenderCapsOpenGL.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderCapsOpenGL::RenderCapsOpenGL()
	{
	}
	RenderCapsOpenGL::~RenderCapsOpenGL()
	{
	}

	bool RenderCapsOpenGL::init()
	{
		if (GLEW_ARB_texture_float)
		{
			flags |= 1 << Flag::TextureFloat;
		}
		if (GLEW_EXT_texture_compression_s3tc)
		{
			flags |= 1 << Flag::TextureCompression;
			flags |= 1 << Flag::TextureDXT1;
		}
		else if (GLEW_EXT_texture_compression_dxt1)
		{
			flags |= 1 << Flag::TextureDXT1;
		}
		if (GLEW_EXT_packed_depth_stencil)
		{
			flags |= 1 << Flag::TextureDepthStencil;
		}
		if (GLEW_ARB_point_sprite)
		{
			flags |= 1 << Flag::PointSprite;
		}
		flags |= 1 << Flag::OcclusionQuery;
		/*if (GLEW_ARB_timer_query)
		{
			flags |= 1 << Flag::TimerQuery;
		}*/
		if (GLEW_EXT_timer_query)
		{
			flags |= 1 << Flag::TimerQuery;
		}
		if (GLEW_ARB_half_float_vertex)
		{
			flags |= 1 << Flag::VertexHalfFloat;
		}
		if (GLEW_ARB_texture_rg)
		{
			flags |= 1 << Flag::TextureRG;
		}
		if (GLEW_ARB_geometry_shader4)
		{
			flags |= 1 << Flag::GeometryShader;
		}
		// TODO: Tesselation shader?
		return true;
	}
}
}
}
