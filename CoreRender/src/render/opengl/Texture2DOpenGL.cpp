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

#include "Texture2DOpenGL.hpp"
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
			default:
				// TODO: R/RG formats
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

	Texture2DOpenGL::Texture2DOpenGL(UploadManager &uploadmgr,
	                                 res::ResourceManager *rmgr,
	                                 const std::string &name)
		: Texture2D(uploadmgr, rmgr, name)
	{
	}
	Texture2DOpenGL::~Texture2DOpenGL()
	{
		// Delete OpenGL texture object
		if (handle != 0)
			glDeleteTextures(1, &handle);
	}

	void Texture2DOpenGL::upload(void *data)
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
		const RenderCaps &caps = *getUploadManager().getCaps();
		unsigned int internal = 0;
		unsigned int currentformat = 0;
		unsigned int component = 0;
		bool compressed = TextureFormat::isCompressed(uploaddata->format);
		bool intcompressed = TextureFormat::isCompressed(uploaddata->internalformat);
		// Compression only works under certain circumstances
		if (intcompressed)
		{
			if (compressed && uploaddata->format != uploaddata->internalformat)
			{
				core::Log::Ptr log = getManager()->getLog();
				log->error("Cannot convert between different compressed formats.");
				if (uploaddata->data)
					free(uploaddata->data);
				return;
			}
			if (!caps.getFlag(RenderCaps::Flag::TextureCompression))
			{
				if ((uploaddata->internalformat != TextureFormat::RGB_DXT1
				    && uploaddata->internalformat != TextureFormat::RGBA_DXT1)
				    || uploaddata->internalformat != uploaddata->format
				    || !caps.getFlag(RenderCaps::Flag::TextureDXT1))
				{
					core::Log::Ptr log = getManager()->getLog();
					log->error("Compressed texture not supported.");
					if (uploaddata->data)
						free(uploaddata->data);
					delete uploaddata;
					return;
				}
			}
		}
		else
		{
			if (compressed)
			{
				core::Log::Ptr log = getManager()->getLog();
				log->error("Cannot load compressed data into uncompressed texture.");
				if (uploaddata->data)
					free(uploaddata->data);
				delete uploaddata;
				return;
			}
		}
		// Check for float extension
		if (TextureFormat::isFloat(uploaddata->internalformat)
		    && !caps.getFlag(RenderCaps::Flag::TextureFloat))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Floating point texture not supported.");
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		// Depth-stencil
		if (uploaddata->internalformat == TextureFormat::Depth24Stencil8
		    && !caps.getFlag(RenderCaps::Flag::TextureDepthStencil))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Depth-stencil texture not supported.");
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		// TODO: Depth textures?
		// Translate internal format
		if (!translateInternalFormat(uploaddata->internalformat, internal))
		{
			core::Log::Ptr log = getManager()->getLog();
			log->error("Error translating internal texture format (%d).",
			           uploaddata->internalformat);
			if (uploaddata->data)
				free(uploaddata->data);
			delete uploaddata;
			return;
		}
		if (uploaddata->data)
		{
			if (!translateFormat(uploaddata->format, currentformat, component))
			{
				core::Log::Ptr log = getManager()->getLog();
				log->error("Unsupported source format.");
				if (uploaddata->data)
					free(uploaddata->data);
				delete uploaddata;
				return;
			}
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
				             currentformat,
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
}
}
}
