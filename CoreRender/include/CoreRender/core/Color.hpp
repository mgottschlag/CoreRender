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

#ifndef _CORERENDER_CORE_COLOR_HPP_INCLUDED_
#define _CORERENDER_CORE_COLOR_HPP_INCLUDED_

namespace cr
{
namespace core
{
	class Color
	{
		public:
			Color()
			{
				red = 0;
				green = 0;
				blue = 0;
				alpha = 0;
			}
			Color(const Color &color)
			{
				red = color.red;
				green = color.green;
				blue = color.blue;
				alpha = color.alpha;
			}
			Color(float red,
			      float green,
			      float blue,
			      float alpha)
			{
				this->red = red;
				this->green = green;
				this->blue = blue;
				this->alpha = alpha;
			}

			void setRed(float red)
			{
				this->red = red;
			}
			float getRed() const
			{
				return red;
			}
			void setBlue(float blue)
			{
				this->blue = blue;
			}
			float getBlue() const
			{
				return blue;
			}
			void setGreen(float green)
			{
				this->green = green;
			}
			float getGreen() const
			{
				return green;
			}
			void setAlpha(float alpha)
			{
				this->alpha = alpha;
			}
			float getAlpha() const
			{
				return alpha;
			}

			Color &operator=(const Color &color)
			{
				red = color.red;
				green = color.green;
				blue = color.blue;
				alpha = color.alpha;
				return *this;
			}
			bool operator==(const Color &color)
			{
				// TODO: Rounding errors
				if (red != color.red)
					return false;
				if (green != color.green)
					return false;
				if (blue != color.blue)
					return false;
				if (alpha != color.alpha)
					return false;
				return true;
			}
		private:
			float red;
			float green;
			float blue;
			float alpha;
	};
}
}

#endif
