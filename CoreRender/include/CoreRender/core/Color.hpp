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
				value = 0;
			}
			Color(const Color &color)
			{
				value = color.value;
			}
			Color(unsigned int color)
			{
				value = color;
			}
			Color(unsigned char red,
			      unsigned char green,
			      unsigned char blue,
			      unsigned char alpha)
			{
				rgba.red = red;
				rgba.green = green;
				rgba.blue = blue;
				rgba.alpha = alpha;
			}

			void set(unsigned int value)
			{
				this->value = value;
			}
			unsigned int get() const
			{
				return value;
			}
			void setRed(unsigned char red)
			{
				rgba.red = red;
			}
			unsigned char getRed() const
			{
				return rgba.red;
			}
			void setBlue(unsigned char blue)
			{
				rgba.blue = blue;
			}
			unsigned char getBlue() const
			{
				return rgba.blue;
			}
			void setGreen(unsigned char green)
			{
				rgba.green = green;
			}
			unsigned char getGreen() const
			{
				return rgba.green;
			}
			void setAlpha(unsigned char alpha)
			{
				rgba.alpha = alpha;
			}
			unsigned char getAlpha() const
			{
				return rgba.alpha;
			}

			Color &operator=(const Color &color)
			{
				value = color.value;
				return *this;
			}
			bool operator==(const Color &color)
			{
				return value == color.value;
			}
		private:
			union
			{
				unsigned int value;
				struct
				{
					unsigned char red;
					unsigned char blue;
					unsigned char green;
					unsigned char alpha;
				} rgba;
			};
	};
}
}

#endif
