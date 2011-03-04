/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CORERENDER_RENDER_PRIMITIVETYPE_HPP_INCLUDED_
#define _CORERENDER_RENDER_PRIMITIVETYPE_HPP_INCLUDED_

namespace cr
{
namespace render
{
	/**
	 * Defines how primitives are formed from vertex data.
	 */
	struct PrimitiveType
	{
		enum List
		{
			/**
			 * Triangles, formed by use of the index buffer.
			 */
			Triangle,
			/**
			 * List of triangles, without using indices.
			 */
			TriangleList,
			/**
			 * Triangle strip.
			 */
			TriangleStrip,
			/**
			 * Triangle fan.
			 */
			TriangleFan,
			/**
			 * Quads, formed by use of the index buffer.
			 */
			Quad,
			/**
			 * List of quads, without using indices.
			 */
			QuadList,
			/**
			 * Quad strip.
			 */
			QuadStrip,
			/**
			 * Point sprites.
			 */
			PointSprite
		};
	};
}
}

#endif
