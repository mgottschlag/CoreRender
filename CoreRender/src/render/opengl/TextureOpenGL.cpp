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

#include "TextureOpenGL.hpp"
#include "../VideoDriver.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/UploadManager.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	static bool translateInternalFormat(TextureFormat::List format,
	                                    unsigned int &internal)
	{
		switch (format)
		{
			case TextureFormat::RGBA8:
				internal = GL_RGBA8;
				return true;
			case TextureFormat::RGBA16F:
				internal = GL_RGBA16F_ARB;
				return true;
			case TextureFormat::RGBA32F:
				internal = GL_RGBA32F_ARB;
				return true;
			case TextureFormat::RGB_DXT1:
				internal = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				return true;
			case TextureFormat::RGBA_DXT1:
				internal = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				return true;
			case TextureFormat::RGBA_DXT3:
				internal = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return true;
			case TextureFormat::RGBA_DXT5:
				internal = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				return true;
			case TextureFormat::Depth24Stencil8:
				internal = GL_DEPTH24_STENCIL8_EXT;
				return true;
			case TextureFormat::Depth16:
				internal = GL_DEPTH_COMPONENT16;
				return true;
			case TextureFormat::Depth24:
				internal = GL_DEPTH_COMPONENT24;
				return true;
			case TextureFormat::R8:
				internal = GL_R8;
				return true;
			case TextureFormat::R16:
				internal = GL_R16;
				return true;
			case TextureFormat::R32:
				internal = GL_R32I;
				return true;
			case TextureFormat::R16F:
				internal = GL_R16F;
				return true;
			case TextureFormat::R32F:
				internal = GL_R32F;
				return true;
			case TextureFormat::RG8:
				internal = GL_RG8;
				return true;
			case TextureFormat::RG16:
				internal = GL_RG16;
				return true;
			case TextureFormat::RG32:
				internal = GL_RG32I;
				return true;
			case TextureFormat::RG16F:
				internal = GL_RG16F;
				return true;
			case TextureFormat::RG32F:
				internal = GL_RG32F;
				return true;
			default:
				return false;
		}
	}

	static bool translateFormat(TextureFormat::List format,
	                            unsigned int &openglfmt,
	                            unsigned int &component)
	{
		switch (format)
		{
			case TextureFormat::RGBA8:
				openglfmt = GL_RGBA;
				component = GL_UNSIGNED_BYTE;
				return true;
			case TextureFormat::RGBA16F:
				openglfmt = GL_RGBA;
				component = GL_HALF_FLOAT_ARB;
				return false;
			case TextureFormat::RGBA32F:
				openglfmt = GL_RGBA;
				component = GL_FLOAT;
				return true;
			case TextureFormat::Depth24Stencil8:
				openglfmt = GL_DEPTH_STENCIL_EXT;
				component = GL_UNSIGNED_INT_24_8_EXT;
				return true;
			case TextureFormat::Depth16:
				openglfmt = GL_DEPTH_COMPONENT;
				component = GL_FLOAT;
				return true;
			case TextureFormat::Depth24:
				openglfmt = GL_DEPTH_COMPONENT;
				component = GL_FLOAT;
				return true;
			case TextureFormat::RGB_DXT1:
			case TextureFormat::RGBA_DXT1:
			case TextureFormat::RGBA_DXT3:
			case TextureFormat::RGBA_DXT5:
				// We never upload these via glTexImage*, so just pass some
				// random format here
				openglfmt = GL_RGBA;
				component = GL_UNSIGNED_BYTE;
				return true;
			default:
				// TODO: R/RG formats
				return false;
		}
	}

	static unsigned int translateTextureType(TextureType::List type)
	{
		switch (type)
		{
			case TextureType::Texture1D:
				return GL_TEXTURE_1D;
			case TextureType::Texture2D:
				return GL_TEXTURE_2D;
			case TextureType::Texture3D:
				return GL_TEXTURE_3D;
			case TextureType::TextureCube:
				return GL_TEXTURE_CUBE_MAP;
		}
	}

	TextureOpenGL::TextureOpenGL(UploadManager &uploadmgr,
	                             res::ResourceManager *rmgr,
	                             const std::string &name)
		: Texture(uploadmgr, rmgr, name)
	{
	}
	TextureOpenGL::~TextureOpenGL()
	{
		// Delete OpenGL texture object
		if (handle != 0)
			glDeleteTextures(1, &handle);
	}

	void TextureOpenGL::upload(void *data)
	{
		TextureData *uploaddata = (TextureData*)data;
		unsigned int opengltype = translateTextureType(uploaddata->type);
		// Create the texture object if necessary
		if (handle == 0)
			glGenTextures(1, &handle);
		// Set texture filtering mode
		glBindTexture(opengltype, handle);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(opengltype, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(opengltype, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (uploaddata->filtering == TextureFiltering::Linear)
		{
			if (uploaddata->mipmaps)
				glTexParameterf(opengltype,
				                GL_TEXTURE_MIN_FILTER,
				                GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameterf(opengltype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(opengltype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameterf(opengltype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(opengltype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		// Check whether we actually support the format
		if (!checkFormatSupport(uploaddata->internalformat,
		                        uploaddata->format,
		                        uploaddata->data != 0))
		{
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		// We still need valid format information even if we do not want to
		// upload any data to the texture, so just take a compatible format
		if (!uploaddata->data)
			uploaddata->format = uploaddata->internalformat;
		// Translate formats to a form OpenGL understands
		unsigned int internal = 0;
		unsigned int format = 0;
		unsigned int component = 0;
		bool compressed = TextureFormat::isCompressed(uploaddata->format);
		bool isdepth = TextureFormat::isDepth(uploaddata->internalformat);
		// Translate internal format
		if (!translateInternalFormat(uploaddata->internalformat, internal)
		    || !translateFormat(uploaddata->format, format, component))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Error translating texture format (%d, %d).",
			           uploaddata->internalformat, uploaddata->format);
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		// Depth textures have to be set up differently in OpenGL if they are
		// used as shadow maps for depth comparisons (sampler2DShadow) to work
		// correctly
		if (isdepth && uploaddata->depthcompare)
		{
			glTexParameteri(opengltype, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(opengltype, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
		else if (isdepth)
		{
			glTexParameteri(opengltype, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		}
		// Upload texture data
		unsigned int width = uploaddata->width;
		unsigned int height = uploaddata->height;
		unsigned int depth = uploaddata->depth;
		unsigned int datasize = uploaddata->datasize;
		switch (uploaddata->type)
		{
			case TextureType::Texture1D:
			case TextureType::Texture2D:
			case TextureType::Texture3D:
				upload(opengltype, width, height, depth, uploaddata->format,
					internal, format, component, uploaddata->data, datasize,
					compressed, !uploaddata->createmipmaps);
				break;
			case TextureType::TextureCube:
				char *currentface = (char*)uploaddata->data;
				currentface += upload(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				currentface += upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				currentface += upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				currentface += upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				currentface += upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				currentface += upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
					width, height, 1, uploaddata->format, internal,
					format, component, currentface, datasize, compressed,
					!uploaddata->createmipmaps);
				break;
		}
		if (uploaddata->data)
			free(uploaddata->data);
		delete uploaddata;
		// Generate mipmaps
		hasmipmaps = uploaddata->mipmaps;
		if (hasmipmaps && uploaddata->createmipmaps)
		{
			glEnable(opengltype);
			glGenerateMipmapEXT(opengltype);
			glDisable(opengltype);
		}
		// Error checking
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Uploading texture: %s", gluErrorString(error));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool TextureOpenGL::checkFormatSupport(TextureFormat::List internalformat,
	                                       TextureFormat::List uploadformat,
	                                       bool uploadingdata)
	{
		const RenderCaps &caps = *getUploadManager().getCaps();
		if (!TextureFormat::supported(caps,
		                              internalformat,
		                              uploadingdata,
		                              uploadformat))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("%s: Texture formats not supported.", getName().c_str());
			return false;
		}
		return true;
	}

	void TextureOpenGL::uploadCompressed(unsigned int target,
	                                     unsigned int mipmap,
	                                     unsigned int width,
	                                     unsigned int height,
	                                     unsigned int depth,
	                                     unsigned int datasize,
	                                     unsigned int internalformat,
	                                     void *data)
	{
		if (target == GL_TEXTURE_1D)
		{
			glCompressedTexImage1D(GL_TEXTURE_1D, mipmap, internalformat, width,
				0, datasize, data);
		}
		else if (target == GL_TEXTURE_3D)
		{
			glCompressedTexImage3D(GL_TEXTURE_3D, mipmap, internalformat, width,
				height, depth, 0, datasize, data);
		}
		else
		{
			glCompressedTexImage2D(target, mipmap, internalformat, width,
				height, 0, datasize, data);
		}
	}
	unsigned int TextureOpenGL::uploadMipmapsCompressed(unsigned int target,
	                                                    unsigned int width,
	                                                    unsigned int height,
	                                                    unsigned int depth,
	                                                    TextureFormat::List format,
	                                                    unsigned int internalformat,
	                                                    void *data)
	{
		unsigned int currentwidth = width;
		unsigned int currentheight = height;
		unsigned int currentdepth = depth;
		unsigned int pixelcount = 0;
		char *currentmipmap = (char*)data;
		unsigned int mipmapindex = 0;
		unsigned int sizeread = 0;
		while (pixelcount != 1)
		{
			pixelcount = currentwidth * currentheight * currentdepth;
			unsigned int datasize = TextureFormat::getSize(format,
			                                               currentwidth,
			                                               currentheight,
			                                               currentdepth);
			uploadCompressed(target, mipmapindex, currentwidth, currentheight,
				currentdepth, datasize, internalformat, currentmipmap);
			sizeread += datasize;
			mipmapindex++;
			currentmipmap = currentmipmap + datasize;
			currentwidth = std::max(currentwidth / 2, 1u);
			currentheight = std::max(currentheight / 2, 1u);
			currentdepth = std::max(currentdepth / 2, 1u);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapindex - 1);
		return sizeread;
	}
	void TextureOpenGL::upload(unsigned int target,
	                           unsigned int mipmap,
	                           unsigned int width,
	                           unsigned int height,
	                           unsigned int depth,
	                           unsigned int internalformat,
	                           unsigned int uploadformat,
	                           unsigned int component,
	                           void *data)
	{
		if (target == GL_TEXTURE_1D)
			glTexImage1D(target, 0, internalformat, width, 0, uploadformat,
				component, data);
		else if (target == GL_TEXTURE_3D)
			glTexImage3D(target, 0, internalformat, width, height, 0,
				uploadformat, depth, component, data);
		else
			glTexImage2D(target, mipmap, internalformat, width, height, 0,
				uploadformat, component, data);
	}
	unsigned int TextureOpenGL::uploadMipmaps(unsigned int target,
	                                          unsigned int width,
	                                          unsigned int height,
	                                          unsigned int depth,
	                                          TextureFormat::List format,
	                                          unsigned int internalformat,
	                                          unsigned int uploadformat,
	                                          unsigned int component,
	                                          void *data)
	{
		unsigned int currentwidth = width;
		unsigned int currentheight = height;
		unsigned int currentdepth = depth;
		unsigned int pixelcount = 0;
		char *currentmipmap = (char*)data;
		unsigned int mipmapindex = 0;
		unsigned int sizeread = 0;
		while (pixelcount != 1)
		{
			pixelcount = currentwidth * currentheight * currentdepth;
			unsigned int datasize = TextureFormat::getSize(format,
			                                               currentwidth,
			                                               currentheight,
			                                               currentdepth);
			upload(target, mipmapindex, currentwidth, currentheight,
				currentdepth, internalformat, uploadformat, component,
				currentmipmap);
			sizeread += datasize;
			mipmapindex++;
			currentmipmap = currentmipmap + datasize;
			currentwidth = std::max(currentwidth / 2, 1u);
			currentheight = std::max(currentheight / 2, 1u);
			currentdepth = std::max(currentdepth / 2, 1u);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapindex - 1);
		return sizeread;
	}

	unsigned int TextureOpenGL::upload(unsigned int target,
	                                   unsigned int width,
	                                   unsigned int height,
	                                   unsigned int depth,
	                                   TextureFormat::List format,
	                                   unsigned int internalformat,
	                                   unsigned int uploadformat,
	                                   unsigned int component,
	                                   void *data,
	                                   unsigned int datasize,
	                                   bool compressed,
	                                   bool mipmaps)
	{
		if (compressed)
		{
			if (mipmaps)
			{
				return uploadMipmapsCompressed(target, width, height, depth,
					format, internalformat, data);
			}
			else
			{
				uploadCompressed(target, 0, width, height, depth, datasize,
					internalformat, data);
				return TextureFormat::getSize(format, width, height, depth);
			}
		}
		else
		{
			if (mipmaps)
			{
				return uploadMipmaps(target, width, height, depth, format,
					internalformat, uploadformat, component, data);
			}
			else
			{
				upload(target, 0, width, height, depth, internalformat,
					uploadformat, component, data);
				return TextureFormat::getSize(format, width, height, depth);
			}
		}
	}
}
}
}
