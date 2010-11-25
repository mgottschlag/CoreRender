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

#include "ImageSTB.hpp"

#include "../3rdparty/stb_image.h"

#include <cstring>

namespace cr
{
namespace render
{
	ImageSTB::ImageSTB()
	{
	}
	ImageSTB::~ImageSTB()
	{
	}

	bool ImageSTB::load(const char *filename,
	                    unsigned int datasize,
	                    unsigned char *data)
	{
		bool ishdr = stbi_is_hdr_from_memory(data, datasize);
		// Load the image from the buffer provided by the caller
		int width;
		int height;
		void *image;
		if (ishdr)
		{
			image = stbi_loadf_from_memory(data,
			                                      datasize,
			                                      &width,
			                                      &height,
			                                      0,
			                                      4);
		}
		else
		{
			image = stbi_load_from_memory(data,
			                                       datasize,
			                                       &width,
			                                       &height,
			                                       0,
			                                       4);
		}
		// If we do not get any image data, this can mean either that the
		// image is corrupt or that it is in a format we do not support here
		if (!image)
			return false;
		this->width = width;
		this->height = height;
		this->depth = 0;
		type = TextureType::Texture2D;
		// TODO: Smaller formats, e.g. RGB, to save memory
		if (ishdr)
		{
			format = TextureFormat::RGBA32F;
			imagesize = width * height * 4 * sizeof(float);
		}
		else
		{
			format = TextureFormat::RGBA8;
			imagesize = width * height * 4;
		}
		imagedata = malloc(imagesize);
		memcpy(imagedata, image, imagesize);
		deletedata = true;
		stbi_image_free(image);
		return true;
	}
}
}
