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
				// TODO
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
				// TODO
				return false;
			case TextureFormat::Depth24:
				// TODO
				return false;
			default:
				// TODO: R/RG formats
				return false;
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
		// Create the texture object if necessary
		if (handle == 0)
		{
			glGenTextures(1, &handle);
			glBindTexture(GL_TEXTURE_2D, handle);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D,
			                GL_TEXTURE_MIN_FILTER,
			                GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_2D,
			                GL_TEXTURE_MAG_FILTER,
			                GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, true);
			// TODO: Error checking
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
		// Translate formats to a form OpenGL understands
		unsigned int internal = 0;
		unsigned int format = 0;
		unsigned int component = 0;
		bool compressed = TextureFormat::isCompressed(uploaddata->format);
		bool intcompressed = TextureFormat::isCompressed(uploaddata->internalformat);
		// Translate internal format
		if (!translateInternalFormat(uploaddata->internalformat, internal)
		    || (uploaddata->data
		        && !translateFormat(uploaddata->format, format, component)))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Error translating texture format (%d, %d).",
			           uploaddata->internalformat, uploaddata->format);
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		// Upload texture data
		glBindTexture(GL_TEXTURE_2D, handle);
		if (uploaddata->data)
		{
			// TODO
			if (compressed)
			{
				unsigned int size = TextureFormat::getSize(uploaddata->format,
				                                           uploaddata->width * uploaddata->height);
				glCompressedTexImage2D(GL_TEXTURE_2D,
				                       0,
				                       internal,
				                       uploaddata->width,
				                       uploaddata->height,
				                       0,
				                       size,
				                       uploaddata->data);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D,
				             0,
				             internal,
				             uploaddata->width,
				             uploaddata->height,
				             0,
				             format,
				             component,
				             uploaddata->data);
			}
			free(uploaddata->data);
		}
		else
		{
			if (intcompressed)
			{
				// TODO: Does this make any sense?
				glCompressedTexImage2D(GL_TEXTURE_2D,
				                       0,
				                       internal,
				                       uploaddata->width,
				                       uploaddata->height,
				                       0,
				                       0,
				                       0);
			}
			else if (TextureFormat::isDepth(uploaddata->internalformat))
			{
				// TODO: Hack
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexImage2D(GL_TEXTURE_2D,
				             0,
				             internal,
				             uploaddata->width,
				             uploaddata->height,
				             0,
				             GL_DEPTH_COMPONENT,
				             GL_UNSIGNED_BYTE,
				             0);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D,
				             0,
				             internal,
				             uploaddata->width,
				             uploaddata->height,
				             0,
				             GL_RGBA,
				             GL_UNSIGNED_BYTE,
				             0);
			}
		}
		delete uploaddata;
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
}
}
}
