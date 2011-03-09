/*
Copyright (C) 2011, Mathias Gottschlag

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

#include "CoreRender/render/Image.hpp"
#include "ImageLoaderDDS.hpp"
#include "ImageLoaderSTB.hpp"

#include <cstdlib>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdparty/stb_image_write.h"

namespace cr
{
namespace render
{
	static std::string tolower(std::string s)
	{
		for (unsigned int i = 0; i < s.size(); i++)
			s[i] = std::tolower(s[i]);
		return s;
	}

	Image::Image()
		: width(0), height(0), depth(0), type(TextureType::Texture2D),
		format(TextureFormat::Invalid), deletedata(true), imagedata(0),
		imagesize(0), hasmipmaps(false)
	{
	}
	Image::~Image()
	{
		if (imagedata && deletedata)
			std::free(imagedata);
	}

	Image *Image::create2D(unsigned int width,
	                       unsigned int height,
	                       TextureFormat::List format,
	                       void *data,
	                       bool copy)
	{
		Image *image = new Image;
		unsigned int imagesize = TextureFormat::getSize(format,
		                                                width,
		                                                height,
		                                                1);
		if (copy)
		{
			void *datacopy = std::malloc(imagesize);
			std::memcpy(datacopy, data, imagesize);
			image->set(width,
			           height,
			           0,
			           TextureType::Texture2D,
			           format,
			           true,
			           datacopy,
			           imagesize,
			           false);
		}
		else
		{
			image->set(width,
			           height,
			           0,
			           TextureType::Texture2D,
			           format,
			           true,
			           data,
			           imagesize,
			           false);
		}
		return image;
	}

	void Image::set(unsigned int width,
	                unsigned int height,
	                unsigned int depth,
	                TextureType::List type,
	                TextureFormat::List format,
	                bool deletedata,
	                void *imagedata,
	                unsigned int imagesize,
	                bool hasmipmaps)
	{
		if (this->deletedata && this->imagedata)
			std::free(this->imagedata);
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->type = type;
		this->format = format;
		this->deletedata = deletedata;
		this->imagedata = imagedata;
		this->imagesize = imagesize;
		this->hasmipmaps = hasmipmaps;
	}

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
		std::string extension = tolower(filename.substr(filename.rfind(".")));
		Image *image = new Image;
		if (extension == ".dds")
		{
			if (!ImageLoaderDDS::load(image, filename.c_str(), datasize, data))
			{
				delete image;
				return 0;
			}
		}
		else
		{
			if (!ImageLoaderSTB::load(image, filename.c_str(), datasize, data))
			{
				delete image;
				return 0;
			}
		}
		return image;
	}

	bool Image::save(core::FileSystem::Ptr fs, const std::string &filename)
	{
		if (type != TextureType::Texture2D)
			return false;
		unsigned int components = 0;
		if (format == TextureFormat::R8)
			components = 1;
		else if (format == TextureFormat::RG8)
			components = 2;
		else if (format == TextureFormat::RGBA8)
			components = 4;
		if (components == 0)
			return false;
		std::string extension = tolower(filename.substr(filename.rfind(".")));
		if (extension == ".png")
		{
			core::File::Ptr file = fs->open(filename,
			                                core::FileAccess::Write,
			                                true);
			if (!file)
				return false;
			unsigned int stride = TextureFormat::getSize(format, width, 1, 1);
			if (!stbi_write_png(file,
			                    width,
			                    height,
			                    components,
			                    imagedata,
			                    stride))
				return false;
		}
		else if (extension == ".tga")
		{
			core::File::Ptr file = fs->open(filename,
			                                core::FileAccess::Write,
			                                true);
			if (!file)
				return false;
			if (!stbi_write_tga(file,
			                    width,
			                    height,
			                    components,
			                    imagedata))
				return false;
		}
		else if (extension == ".bmp")
		{
			core::File::Ptr file = fs->open(filename,
			                                core::FileAccess::Write,
			                                true);
			if (!file)
				return false;
			if (!stbi_write_bmp(file,
			                    width,
			                    height,
			                    components,
			                    imagedata))
				return false;
		}
		else
		{
			return false;
		}
		return true;
	}
}
}
