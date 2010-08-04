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

#include "CoreRender/render/Texture2D.hpp"
#include "CoreRender/render/Renderer.hpp"

#include <cstring>
#include <cstdlib>

namespace cr
{
namespace render
{
	Texture2D::Texture2D(Renderer *renderer,
	                 res::ResourceManager *rmgr,
	                 const std::string &name)
		: Texture(renderer, rmgr, name, TextureType::Texture2D), width(0),
		height(0), internalformat(TextureFormat::Invalid),
		format(TextureFormat::Invalid), data(0)
	{
	}
	Texture2D::~Texture2D()
	{
		if (data)
			free(data);
	}

	bool Texture2D::set(unsigned int width,
	                    unsigned int height,
	                    TextureFormat::List internalformat,
	                    TextureFormat::List format,
	                    void *data,
	                    bool copy)
	{
		// Allocate image data
		void *datacopy;
		if (copy)
		{
			unsigned int datasize = TextureFormat::getSize(format,
			                                               width * height);
			datacopy = malloc(datasize);
			memcpy(datacopy, data, datasize);
		}
		else
		{
			datacopy = data;
		}
		void *prevdata = 0;
		// Fill in info
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			prevdata = this->data;
			this->width = width;
			this->height = height;
			this->internalformat = internalformat;
			this->format = format;
			this->data = datacopy;
		}
		// Delete old data
		if (prevdata)
			free(prevdata);
		// Register for uploading
		registerUpload();
		return true;
	}
	bool Texture2D::update(TextureFormat::List format,
	                       void *data,
	                       bool copy)
	{
		// Allocate image data
		void *datacopy;
		if (copy)
		{
			unsigned int datasize = TextureFormat::getSize(format,
			                                               width * height);
			datacopy = malloc(datasize);
			memcpy(datacopy, data, datasize);
		}
		else
		{
			datacopy = data;
		}
		void *prevdata = 0;
		// Fill in info
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			prevdata = this->data;
			this->format = format;
			this->data = datacopy;
		}
		// Delete old data
		if (prevdata)
			free(prevdata);
		// Register for uploading
		registerUpload();
		return true;
	}
	bool Texture2D::update(unsigned int x,
	                       unsigned int y,
	                       unsigned int width,
	                       unsigned int height,
	                       TextureFormat::List format,
	                       void *data)
	{
		// TODO
		return false;
	}
	void Texture2D::discardImageData()
	{
		discarddata = true;
		// TODO: Actually delete the data
	}

	bool Texture2D::load()
	{
		// TODO
		finishLoading(false);
		return false;
	}
	bool Texture2D::unload()
	{
		discardImageData();
		return true;
	}
}
}
