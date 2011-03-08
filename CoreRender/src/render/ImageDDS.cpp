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

#include "ImageDDS.hpp"

#include <cstring>
#include <algorithm>
#include <cassert>

namespace cr
{
namespace render
{
	static const unsigned int DDSD_CAPS        = 0x00000001;
	static const unsigned int DDSD_PIXELFORMAT = 0x00001000;
	static const unsigned int DDSD_MIPMAPCOUNT = 0x00020000;

	static const unsigned int DDPF_ALPHAPIXELS = 0x00000001;
	static const unsigned int DDPF_FOURCC      = 0x00000004;
	static const unsigned int DDPF_INDEXED     = 0x00000020;
	static const unsigned int DDPF_RGB         = 0x00000040;

	static const unsigned int DDSCAPS2_CUBEMAP           = 0x00000200;
	static const unsigned int DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
	static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
	static const unsigned int DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
	static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
	static const unsigned int DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
	static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
	static const unsigned int DDSCAPS2_CUBEMAP_COMPLETE  = 0x0000FC00;
	static const unsigned int DDSCAPS2_VOLUME            = 0x00200000;

	#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

	static const unsigned int FORMAT_DXT1 = FOURCC('D', 'X', 'T', '1');
	static const unsigned int FORMAT_DXT2 = FOURCC('D', 'X', 'T', '2');
	static const unsigned int FORMAT_DXT3 = FOURCC('D', 'X', 'T', '3');
	static const unsigned int FORMAT_DXT4 = FOURCC('D', 'X', 'T', '4');
	static const unsigned int FORMAT_DXT5 = FOURCC('D', 'X', 'T', '5');
	static const unsigned int FORMAT_A16B16G16R16F = 113;
	static const unsigned int FORMAT_A32B32G32R32F = 116;

	struct DDSHeader
	{
		unsigned int magic;
		unsigned int size;
		unsigned int flags;
		unsigned int height;
		unsigned int width;
		unsigned int pitchorlinearsize;
		unsigned int depth;
		unsigned int mipmapcount;
		unsigned int reserved1[11];
		struct
		{
			unsigned int size;
			unsigned int flags;
			unsigned int fourcc;
			unsigned int rgbbitcount;
			unsigned int rbitmask;
			unsigned int gbitmask;
			unsigned int bbitmask;
			unsigned int alphabitmask;
		} pixelformat;
		struct
		{
			unsigned int caps1;
			unsigned int caps2;
			unsigned int ddsx;
			unsigned int reserved;
		} caps;
		unsigned int reserved2;
	};

	ImageDDS::ImageDDS()
	{
	}
	ImageDDS::~ImageDDS()
	{
	}

	bool ImageDDS::create(const std::string &filename,
	                      unsigned int datasize,
	                      unsigned char *data)
	{
		assert(sizeof(DDSHeader) == 128);
		if (datasize < sizeof(DDSHeader))
			return false;
		// Parse DDS header
		DDSHeader *header = (DDSHeader*)data;
		if (header->magic != FOURCC('D', 'D', 'S', ' '))
			return false;
		if (header->size != 124)
			return false;
		if ((header->flags & DDSD_CAPS) == 0)
			return false;
		if ((header->flags & DDSD_PIXELFORMAT) == 0)
			return false;
		width = header->width;
		height = header->height;
		depth = 1;
		unsigned int slices = 1;
		// TODO: Mipmaps
		unsigned int mipmapcount = header->mipmapcount;
		if ((header->flags & DDSD_MIPMAPCOUNT) == 0)
			mipmapcount = 1;
		hasmipmaps = mipmapcount > 1;
		// Get texture type
		if (header->caps.caps2 & DDSCAPS2_CUBEMAP)
		{
			if ((header->caps.caps2 & DDSCAPS2_CUBEMAP_COMPLETE) != DDSCAPS2_CUBEMAP_COMPLETE)
				return false;
			type = TextureType::TextureCube;
			// The cube map sides are just stored as consecutive slices in the
			// file
			slices = 6;
		}
		else if (header->caps.caps2 & DDSCAPS2_VOLUME)
		{
			type = TextureType::Texture3D;
			depth = header->depth;
		}
		else
		{
			type = TextureType::Texture2D;
		}
		// Get pixel format
		bool componentsswapped = false;
		bool createalpha = false;
		if (header->pixelformat.flags & DDPF_FOURCC)
		{
			if (header->pixelformat.fourcc == FORMAT_DXT1)
				format = TextureFormat::RGBA_DXT1;
			else if (header->pixelformat.fourcc == FORMAT_DXT3)
				format = TextureFormat::RGBA_DXT3;
			else if (header->pixelformat.fourcc == FORMAT_DXT5)
				format = TextureFormat::RGBA_DXT5;
			else if (header->pixelformat.fourcc == FORMAT_A16B16G16R16F)
			{
				componentsswapped = true;
				format = TextureFormat::RGBA16F;
			}
			else if (header->pixelformat.fourcc == FORMAT_A32B32G32R32F)
			{
				componentsswapped = true;
				format = TextureFormat::RGBA32F;
			}
			else
				return false;
		}
		else if (header->pixelformat.flags & DDPF_RGB)
		{

			if (header->pixelformat.rbitmask == 0x000000FF
			 && header->pixelformat.gbitmask == 0x0000FF00
			 && header->pixelformat.bbitmask == 0x00FF0000)
			{
				componentsswapped = true;
			}
			else if (!(header->pixelformat.rbitmask == 0x00FF0000
			        && header->pixelformat.gbitmask == 0x0000FF00
			        && header->pixelformat.bbitmask == 0x000000FF))
			{
				return false;
			}
			if (header->pixelformat.flags & DDPF_ALPHAPIXELS)
				format = TextureFormat::RGBA8;
			else
			{
				format = TextureFormat::RGBA8;
				// We only support RGBA, so we have to insert alpha values
				createalpha = true;
				return false;
			}
		}
		else
		{
			// We do not support these other pixel formats
			return false;
		}
		// Calculate texture data size
		imagesize = 0;
		unsigned int currentwidth = width;
		unsigned int currentheight = height;
		unsigned int currentdepth = depth;
		unsigned int pixelcount = 0;
		for (unsigned int i = 0; i < mipmapcount; i++)
		{
			imagesize += TextureFormat::getSize(format,
			                                    currentwidth,
			                                    currentheight,
			                                    currentdepth);
			pixelcount = currentwidth * currentheight * currentdepth;
			currentwidth = std::max<unsigned int>(currentwidth / 2, 1u);
			currentheight = std::max<unsigned int>(currentheight / 2, 1u);
			currentdepth = std::max<unsigned int>(currentdepth / 2, 1u);
		}
		// We only support loading mipmaps from the file if all mipmaps down to
		// 1*1*1 are available
		if (hasmipmaps && pixelcount != 1)
			return false;
		imagesize *= slices;
		if (datasize < sizeof(DDSHeader) + imagesize)
			return false;
		// Get texture data
		imagedata = malloc(imagesize);
		if (!createalpha)
		{
			void *srcdata = (char*)data + sizeof(DDSHeader);
			memcpy(imagedata, srcdata, imagesize);
			if (componentsswapped && format == TextureFormat::RGBA8)
			{
				unsigned int *pixels = (unsigned int*)imagedata;
				for (unsigned int i = 0; i < imagesize / 4; i++)
				{
					pixels[i] = ((pixels[i] & 0xFF000000) >> 24)
					          | ((pixels[i] & 0x00FF0000) >> 8)
					          | ((pixels[i] & 0x0000FF00) << 8)
					          | ((pixels[i] & 0x000000FF) << 24);
				}
			}
			else if (componentsswapped && format == TextureFormat::RGBA16F)
			{
				unsigned short *pixels = (unsigned short*)imagedata;
				for (unsigned int i = 0; i < imagesize / 8; i++)
				{
					unsigned short tmp = pixels[i * 4];
					pixels[i * 4] = pixels[i * 4 + 3];
					pixels[i * 4 + 3] = tmp;
					tmp = pixels[i * 4 + 1];
					pixels[i * 4 + 1] = pixels[i * 4 + 2];
					pixels[i * 4 +2] = tmp;
				}
			}
			else if (componentsswapped && format == TextureFormat::RGBA32F)
			{
				float *pixels = (float*)imagedata;
				for (unsigned int i = 0; i < imagesize / 16; i++)
				{
					float tmp = pixels[i * 4];
					pixels[i * 4] = pixels[i * 4 + 3];
					pixels[i * 4 + 3] = tmp;
					tmp = pixels[i * 4 + 1];
					pixels[i * 4 + 1] = pixels[i * 4 + 2];
					pixels[i * 4 +2] = tmp;
				}
			}
		}
		else
		{
			// The only case we handle with no alpha values is 24-bit RGB, so
			// just convert the pixels to 32-bit RGBA
			char *srcdata = (char*)data + sizeof(DDSHeader);
			unsigned int *dstdata = (unsigned int*)imagedata;
			if (componentsswapped)
			{
				for (unsigned int i = 0; i < imagesize / 4; i++)
				{
					dstdata[i] = (unsigned int)srcdata[2]
					           + ((unsigned int)srcdata[1] << 8)
					           + ((unsigned int)srcdata[0] << 16);
				}
			}
			else
			{
				for (unsigned int i = 0; i < imagesize / 4; i++)
				{
					dstdata[i] = (unsigned int)srcdata[0]
					           + ((unsigned int)srcdata[1] << 8)
					           + ((unsigned int)srcdata[2] << 16);
				}
			}
		}
		return true;
	}
}
}
