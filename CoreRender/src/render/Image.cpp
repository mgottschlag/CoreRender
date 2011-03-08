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

#include "CoreRender/render/Image.hpp"
#include "ImageDDS.hpp"
#include "ImageSTB.hpp"

namespace cr
{
namespace render
{
	Image *Image::load(core::FileSystem::Ptr fs, const std::string &filename)
	{
		// Open file
		core::File::Ptr file = fs->open(filename, core::FileAccess::Read);
		if (!file)
			return 0;
		// Read the file content
		unsigned int filesize = file->getSize();
		unsigned char *buffer = new unsigned char[filesize];
		if (file->read(filesize, buffer) != (int)filesize)
		{
			delete[] buffer;
			return 0;
		}
		// Load the image
		Image *image = load(filename, filesize, buffer);
		delete buffer;
		return image;
	}

	Image *Image::load(const std::string &filename,
	                   unsigned int datasize,
	                   unsigned char *data)
	{
		// We have to check the extension of the file first as we have multiple
		// image loaders for different formats
		std::string extension = filename.substr(filename.rfind("."));
		Image *image = 0;
		if (extension == ".dds")
			image = new ImageDDS;
		else
			image = new ImageSTB;
		if (!image->load(filename.c_str(), datasize, data))
		{
			delete image;
			return 0;
		}
		return image;
	}
}
}
