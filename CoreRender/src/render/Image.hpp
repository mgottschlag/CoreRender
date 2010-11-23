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

#ifndef _CORERENDER_RENDER_IMAGE_HPP_INCLUDED_
#define _CORERENDER_RENDER_IMAGE_HPP_INCLUDED_

#include "CoreRender/render/Texture.hpp"

#include <cstdlib>

namespace cr
{
namespace render
{
	class Image
	{
		public:
			Image()
				: width(0), height(0), depth(0), type(TextureType::Texture2D),
				format(TextureFormat::Invalid), deletedata(true), imagedata(0),
				imagesize(0)
			{
			}
			virtual ~Image()
			{
				if (imagedata && deletedata)
					free(imagedata);
			}

			virtual bool load(const char *filename,
			                  unsigned int datasize,
			                  unsigned char *data) = 0;

			unsigned int getWidth()
			{
				return width;
			}
			unsigned int getHeight()
			{
				return height;
			}
			unsigned int getDepth()
			{
				return depth;
			}
			TextureType::List getType()
			{
				return type;
			}
			TextureFormat::List getFormat()
			{
				return format;
			}
			void *getImageData(bool dropownership = false)
			{
				if (dropownership)
					deletedata = false;
				return imagedata;
			}
			unsigned int getImageSize()
			{
				return imagesize;
			}
		protected:
			unsigned int width;
			unsigned int height;
			unsigned int depth;
			TextureType::List type;
			TextureFormat::List format;
			bool deletedata;
			void *imagedata;
			unsigned int imagesize;
	};
}
}

#endif
