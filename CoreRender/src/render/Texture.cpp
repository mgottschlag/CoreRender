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

#include "CoreRender/render/Texture.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/RenderCaps.hpp"
#include "CoreRender/render/Image.hpp"

#include <cstring>
#include <cstdlib>

namespace cr
{
namespace render
{
	bool TextureFormat::supported(const RenderCaps &caps,
	                              TextureFormat::List internalformat,
	                              bool uploadingdata,
	                              TextureFormat::List uploadformat)
	{
		bool internalcompressed = TextureFormat::isCompressed(internalformat);
		bool compressed = TextureFormat::isCompressed(uploadformat);
		// Compressed textures need OpenGL extensions and have certain
		// restrictions
		if (internalcompressed)
		{
			// Not uploading anything to a compressed texture does not make much
			// sense as we do not support rendering to a compressed texture
			if (!uploadingdata)
				return false;
			// Converting between different compressed formats is not supported
			if (compressed && (internalformat != uploadformat))
				return false;
			if (!caps.getFlag(RenderCaps::Flag::TextureCompression))
			{
				// We might support at least a subset of the texture compression
				// extension, e.g. on mobile devices of open source drivers
				if (caps.getFlag(RenderCaps::Flag::TextureDXT1))
				{
					// TextureDXT1 only supports some compressed formats
					if (internalformat != TextureFormat::RGB_DXT1
					 && internalformat != TextureFormat::RGBA_DXT1)
						return false;
					// TextureDXT1 does not support uploading uncompressed data
					if (internalformat != uploadformat)
						return false;
				}
				else
					return false;
			}
		}
		// We can not load compressed data into uncompressed textures
		if (uploadingdata && compressed && !internalcompressed)
			return false;
		// Depth/stencil textures need an OpenGL extension
		if (!caps.getFlag(RenderCaps::Flag::TextureDepthStencil)
		 && internalformat == Depth24Stencil8)
			return false;
		// Float textures need an OpenGL extension
		bool isfloat = isFloat(internalformat);
		if (isfloat && !caps.getFlag(RenderCaps::Flag::TextureFloat))
			return false;
		// TODO: Additional constraints on depth and float textures
		return true;
	}

	TextureFormat::List TextureFormat::fromString(const std::string &str)
	{
		if (str == "RGBA8")
			return RGBA8;
		if (str == "RGBA16F")
			return RGBA16F;
		if (str == "RGBA32F")
			return RGBA32F;
		if (str == "R8")
			return R8;
		if (str == "R16")
			return R16;
		if (str == "R32")
			return R32;
		if (str == "R16F")
			return R16F;
		if (str == "R32F")
			return R32F;
		if (str == "RG8")
			return RG8;
		if (str == "RG16")
			return RG16;
		if (str == "RG32")
			return RG32;
		if (str == "RG16F")
			return RG16F;
		if (str == "RG32F")
			return RG32F;
		if (str == "RGB_DXT1")
			return RGB_DXT1;
		if (str == "RGBA_DXT1")
			return RGBA_DXT1;
		if (str == "RGBA_DXT3")
			return RGBA_DXT3;
		if (str == "RGBA_DXT5")
			return RGBA_DXT5;
		if (str == "Depth24Stencil8")
			return Depth24Stencil8;
		if (str == "Depth16")
			return Depth16;
		if (str == "Depth24")
			return Depth24;
		return Invalid;
	}

	unsigned int TextureFormat::getSize(TextureFormat::List format,
	                                    unsigned int width,
	                                    unsigned int height,
	                                    unsigned int depth)
	{
		switch (format)
		{
			case TextureFormat::RGBA_DXT1:
			case TextureFormat::RGBA_DXT3:
			case TextureFormat::RGBA_DXT5:
				if (width < 4)
					width = 4;
				if (height < 4)
					height = 4;
				break;
			default:
				break;
		}
		unsigned int texels = width * height * depth;
		switch (format)
		{
			case TextureFormat::Invalid:
				return 0;
			case TextureFormat::RGBA8:
				return texels * 4;
			case TextureFormat::RGBA16F:
				return texels * 8;
			case TextureFormat::RGBA32F:
				return texels * 16;
			case TextureFormat::RGBA_DXT1:
				return texels / 2;
			case TextureFormat::RGBA_DXT3:
			case TextureFormat::RGBA_DXT5:
				return texels;
			case TextureFormat::Depth24Stencil8:
				return texels * 4;
			case TextureFormat::Depth16:
				return texels * 2;
			case TextureFormat::Depth24:
				return texels * 3;
			default:
				// TODO
				return 0;
		}
	}

	Texture::Texture(UploadManager &uploadmgr,
	                 res::ResourceManager *rmgr,
	                 const std::string &name)
		: RenderResource(uploadmgr, rmgr, name), handle(0), discarddata(false)
	{
		currentdata.width = 0;
		currentdata.height = 0;
		currentdata.depth = 0;
		currentdata.type = TextureType::Texture2D;
		currentdata.internalformat = TextureFormat::Invalid;
		currentdata.format = TextureFormat::Invalid;
		currentdata.data = 0;
		currentdata.datasize = 0;
		currentdata.mipmaps = true;
		currentdata.filtering = TextureFiltering::Linear;
		currentdata.depthcompare = false;
	}
	Texture::~Texture()
	{
		if (currentdata.data)
			free(currentdata.data);
	}

	bool Texture::set1D(unsigned int width,
	                    TextureFormat::List internalformat,
	                    TextureFormat::List format,
	                    void *data,
	                    bool copy)
	{
		unsigned int datasize = TextureFormat::getSize(format, width, 1);
		return set(TextureType::Texture1D,
		           width,
		           0,
		           0,
		           internalformat,
		           format,
		           datasize,
		           data,
		           copy);
	}
	bool Texture::update1D(unsigned int x,
	                       unsigned int width,
	                       void *data)
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		if (currentdata.data == 0 || currentdata.type != TextureType::Texture1D)
			return false;
		if (x + width > currentdata.width)
			return false;
		TextureFormat::List format = currentdata.format;
		unsigned int size = TextureFormat::getSize(format, width, 1);
		unsigned int dstoffset = TextureFormat::getSize(format, x, 1);
		char *src = (char*)data;
		char *dst = (char*)currentdata.data + dstoffset;
		memcpy(dst, src, size);
		registerUpload();
		return true;
	}

	bool Texture::set2D(unsigned int width,
	                    unsigned int height,
	                    TextureFormat::List internalformat,
	                    TextureFormat::List format,
	                    void *data,
	                    bool copy)
	{
		unsigned int datasize = TextureFormat::getSize(format, width, height);
		return set(TextureType::Texture2D,
		           width,
		           height,
		           0,
		           internalformat,
		           format,
		           datasize,
		           data,
		           copy);
	}
	bool Texture::update2D(unsigned int x,
	                       unsigned int y,
	                       unsigned int width,
	                       unsigned int height,
	                       void *data)
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		if (currentdata.data == 0 || currentdata.type != TextureType::Texture2D)
			return false;
		if (x + width > currentdata.width || y + height > currentdata.height)
			return false;
		TextureFormat::List format = currentdata.format;
		unsigned int srcstride = TextureFormat::getSize(format, width, 1);
		unsigned int dststride = TextureFormat::getSize(format,
		                                                currentdata.width, 1);
		unsigned int dstoffset = TextureFormat::getSize(format, x, 1)
		                       + dststride * y;
		char *src = (char*)data;
		char *dst = (char*)currentdata.data + dstoffset;
		if (srcstride == dststride)
		{
			memcpy(dst, src, srcstride * height);
		}
		else
		{
			for (unsigned int i = 0; i < height; i++)
			{
				memcpy(dst, src, srcstride);
				src += srcstride;
				dst += dststride;
			}
		}
		registerUpload();
		return true;
	}

	bool Texture::setCube(unsigned int width,
	                      unsigned int height,
	                      TextureFormat::List internalformat,
	                      TextureFormat::List format,
	                      void *data,
	                      bool copy)
	{
		unsigned int datasize = TextureFormat::getSize(format,
		                                               width, height) * 6;
		return set(TextureType::TextureCube,
		           width,
		           height,
		           0,
		           internalformat,
		           format,
		           datasize,
		           data,
		           copy);
	}
	bool Texture::updateCube(CubeSide::List side,
	                      unsigned int x,
	                      unsigned int y,
	                      unsigned int width,
	                      unsigned int height,
	                      void *data)
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		if (currentdata.data == 0
			|| currentdata.type != TextureType::TextureCube)
			return false;
		if (x + width > currentdata.width || y + height > currentdata.height)
			return false;
		TextureFormat::List format = currentdata.format;
		unsigned int srcstride = TextureFormat::getSize(format, width, 1);
		unsigned int dststride = TextureFormat::getSize(format,
		                                                currentdata.width, 1);
		unsigned int dstoffset = TextureFormat::getSize(format, x, 1)
		                       + dststride * y;
		dstoffset += (int)side * currentdata.height * dststride;
		char *src = (char*)data;
		char *dst = (char*)currentdata.data + dstoffset;
		if (srcstride == dststride)
		{
			memcpy(dst, src, srcstride * height);
		}
		else
		{
			for (unsigned int i = 0; i < height; i++)
			{
				memcpy(dst, src, srcstride);
				src += srcstride;
				dst += dststride;
			}
		}
		registerUpload();
		return true;
	}

	bool Texture::set3D(unsigned int width,
	                    unsigned int height,
	                    unsigned int depth,
	                    TextureFormat::List internalformat,
	                    TextureFormat::List format,
	                    void *data,
	                    bool copy)
	{
		unsigned int datasize = TextureFormat::getSize(format,
		                                               width, height, depth);
		return set(TextureType::Texture3D,
		           width,
		           height,
		           depth,
		           internalformat,
		           format,
		           datasize,
		           data,
		           copy);
	}
	bool Texture::update3D(unsigned int x,
	                       unsigned int y,
	                       unsigned int z,
	                       unsigned int width,
	                       unsigned int height,
	                       unsigned int depth,
	                       void *data)
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		if (currentdata.data == 0
			|| currentdata.type != TextureType::TextureCube)
			return false;
		if (x + width > currentdata.width || y + height > currentdata.height)
			return false;
		TextureFormat::List format = currentdata.format;
		unsigned int srcstride = TextureFormat::getSize(format, width, 1);
		unsigned int dststride = TextureFormat::getSize(format,
		                                                currentdata.width, 1);
		unsigned int dstlayersize = dststride * currentdata.height;
		unsigned int dstoffset = TextureFormat::getSize(format, x, 1)
		                       + dststride * y + z * dstlayersize;
		char *src = (char*)data;
		char *dst = (char*)currentdata.data + dstoffset;
		if (srcstride == dststride)
		{
			for (unsigned int i = 0; i < depth; i++)
			{
				memcpy(dst, src, srcstride * height);
				src += srcstride * height;
				dstoffset += dstlayersize;
				dst = (char*)currentdata.data + dstoffset;
			}
		}
		else
		{
			for (unsigned int i = 0; i < depth; i++)
			{
				for (unsigned int j = 0; j < height; j++)
				{
					memcpy(dst, src, srcstride);
					src += srcstride;
					dst += dststride;
				}
				dstoffset += dstlayersize;
				dst = (char*)currentdata.data + dstoffset;
			}
		}
		registerUpload();
		return true;
	}

	void Texture::setMipmapsEnabled(bool mipmaps)
	{
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			currentdata.mipmaps = mipmaps;
		}
		registerUpload();
	}
	bool Texture::getMipmapsEnabled()
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		return currentdata.mipmaps;
	}

	void Texture::setDepthCompare(bool depthcompare)
	{
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			currentdata.depthcompare = depthcompare;
		}
		registerUpload();
	}
	bool Texture::getDepthCompare()
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		return currentdata.depthcompare;
	}

	void Texture::setFiltering(TextureFiltering::List filtering)
	{
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			currentdata.filtering = filtering;
		}
		registerUpload();
	}
	TextureFiltering::List Texture::getFiltering()
	{
		tbb::spin_mutex::scoped_lock lock(imagemutex);
		return currentdata.filtering;
	}

	void Texture::discardImageData()
	{
		// We have to check whether the image data is still needed here before
		// we delete it, and maybe only delete it later in getUploadData().
		if (!isUploading())
		{
			discarddata = true;
		}
		else
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			if (currentdata.data)
			{
				free(currentdata.data);
				currentdata.data = 0;
			}
		}
	}

	bool Texture::load()
	{
		std::string path = getPath();
		// Open file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		// Open image
		Image *image = Image::load(fs, path);
		if (!image)
		{
			getManager()->getLog()->error("Could not open image \"%s\".",
			                              path.c_str());
			finishLoading(false);
			return false;
		}
		set(image->getType(),
		    image->getWidth(),
		    image->getHeight(),
		    image->getDepth(),
		    image->getFormat(),
		    image->getFormat(),
		    image->getImageSize(),
		    image->getImageData(true),
		    false,
		    !image->hasMipmaps());
		delete image;
		finishLoading(true);
		return true;
	}
	bool Texture::unload()
	{
		discardImageData();
		return true;
	}

	void *Texture::getUploadData()
	{
		TextureData *uploaddata = new TextureData(currentdata);
		if (currentdata.data)
		{
			uploaddata->data = malloc(currentdata.datasize);
			memcpy(uploaddata->data, currentdata.data, currentdata.datasize);
			if (discarddata == true)
			{
				free(currentdata.data);
				currentdata.data = 0;
				discarddata = false;
			}
		}
		return uploaddata;
	}

	bool Texture::set(TextureType::List type,
	                  unsigned int width,
	                  unsigned int height,
	                  unsigned int depth,
	                  TextureFormat::List internalformat,
	                  TextureFormat::List format,
	                  unsigned int datasize,
	                  void *data,
	                  bool copy,
	                  bool createmipmaps)
	{
		// Allocate image data
		void *datacopy;
		if (copy && data)
		{
			datacopy = malloc(datasize);
			memcpy(datacopy, data, datasize);
		}
		else
		{
			datacopy = data;
		}
		void *prevdata = 0;
		// Copy new texture information
		{
			tbb::spin_mutex::scoped_lock lock(imagemutex);
			prevdata = currentdata.data;
			currentdata.type = type;
			currentdata.width = width;
			currentdata.height = height;
			currentdata.depth = depth;
			currentdata.internalformat = internalformat;
			currentdata.format = format;
			currentdata.data = datacopy;
			currentdata.datasize = datasize;
			currentdata.createmipmaps = createmipmaps;
		}
		// Delete old data
		if (prevdata)
			free(prevdata);
		// Register for uploading
		registerUpload();
		return true;
	}
}
}
