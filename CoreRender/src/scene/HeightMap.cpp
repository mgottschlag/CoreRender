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

#include "CoreRender/scene/HeightMap.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace cr
{
namespace scene
{
	struct MapHeader
	{
		unsigned short format;
		unsigned short width;
		unsigned short height;
	};

	HeightMap::HeightMap()
		: data(0), width(0), height(0)
	{
	}
	HeightMap::~HeightMap()
	{
		if (data)
			delete[] data;
	}

	bool HeightMap::load(core::FileSystem::Ptr fs, const std::string &filename)
	{
		if (data)
		{
			delete[] data;
			data = 0;
			width = 0;
			height = 0;
		}
		core::File::Ptr file = fs->open(filename, core::FileAccess::Read, false);
		if (!file)
			return false;
		MapHeader header;
		if (file->read(sizeof(header), &header) != sizeof(header))
			return false;
		data = new float[header.width * header.height];
		if (!data)
			return false;
		width = header.width;
		height = header.height;
		int datasize = width * height * sizeof(float);
		if (file->read(datasize, data) != datasize)
			return false;
		return true;
	}

	void HeightMap::set(unsigned int width,
	                    unsigned int height,
	                    float *data,
	                    bool copy)
	{
		if (this->data)
		{
			delete[] this->data;
		}
		if (copy && data)
		{
			this->data = new float[width * height];
			std::memcpy(this->data, data, width * height * sizeof(float));
		}
		else
			this->data = data;
		this->width = width;
		this->height = height;
	}

	float HeightMap::getMin(unsigned int x,
	                        unsigned int y,
	                        unsigned int width,
	                        unsigned int height)
	{
		assert(width > 0 && height > 0);
		unsigned int index = x + y * this->width;
		float min = data[index];
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				index = x + j + (y + i) * this->width;
				if (data[index] < min)
					min = data[index];
			}
		}
		return min;
	}
	float HeightMap::getMax(unsigned int x,
	                        unsigned int y,
	                        unsigned int width,
	                        unsigned int height)
	{
		assert(width > 0 && height > 0);
		unsigned int index = x + y * this->width;
		float max = data[index];
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				index = x + j + (y + i) * this->width;
				if (data[index] > max)
					max = data[index];
			}
		}
		return max;
	}

	float HeightMap::getHeight(float x, float y)
	{
		// Map x/y into the range 0..width/height
		// TODO: We later might want other modes like mirroring here
		x *= width - 1;
		y *= height - 1;
		if (x > width - 1)
			x = width - 1;
		if (y > height - 1)
			y = height - 1;
		if (x < 0.0f)
			x = 0.0f;
		if (y < 0.0f)
			y = 0.0f;
		// Get the coordinates of the pixels we have to interpolate between
		int lowx = std::floor(x);
		int lowy = std::floor(y);
		int highx = std::min(width - 1, (unsigned int)std::ceil(x));
		int highy = std::min(height - 1, (unsigned int)std::ceil(y));
		// Interpolation factor
		float dx = x - lowx;
		float dy = y - lowy;
		// Interpolate
		float height1 = getEntry(lowx, lowy) * (1.0f - dx) + getEntry(highx, lowy) * dx;
		float height2 = getEntry(lowx, highy) * (1.0f - dx) + getEntry(highx, highy) * dx;
		return height1 * (1.0f - dy) + height2 * dy;
	}
}
}
