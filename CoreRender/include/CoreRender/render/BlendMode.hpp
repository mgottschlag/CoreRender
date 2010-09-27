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

#ifndef _CORERENDER_RENDER_BLENDMODE_HPP_INCLUDED_
#define _CORERENDER_RENDER_BLENDMODE_HPP_INCLUDED_

namespace cr
{
namespace render
{
	/**
	 * Defines the way geometry is blended with the render target.
	 */
	struct BlendMode
	{
		enum List
		{
			/**
			 * No blending, only takes the color from the current rendering
			 * operation.
			 */
			Replace,
			/**
			 * Blends between the output of the rendering operation and the
			 * render target based on the alpha value of the shader output.
			 */
			Blend,
			/**
			 * Adds the output of the shader onto the value of the pixel in the
			 * render target.
			 */
			Add,
			/**
			 * Like Add, but additionally multiplies the color which is added
			 * with the alpha value.
			 */
			AddBlended,
			/**
			 * Multiplies the color of the render target with the output of the
			 * shader.
			 */
			Multiply,
			/**
			 * Takes the minimum of each color channel from either the render
			 * target or the output of the shader.
			 */
			Minimum,
			/**
			 * Takes the maximum of each color channel from either the render
			 * target or the output of the shader.
			 */
			Maximum
		};
	};
}
}

#endif
