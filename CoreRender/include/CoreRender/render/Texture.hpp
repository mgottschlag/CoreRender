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

#ifndef _CORERENDER_RENDER_TEXTURE_HPP_INCLUDED_
#define _CORERENDER_RENDER_TEXTURE_HPP_INCLUDED_

#include "RenderResource.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace render
{
	class RenderCaps;

	/**
	 * Color format of the texture data in RAM/VRAM.
	 */
	struct TextureFormat
	{
		enum List
		{
			Invalid,
			RGBA8,
			RGBA16F,
			RGBA32F,
			R8,
			R16,
			R32,
			R16F,
			R32F,
			RG8,
			RG16,
			RG32,
			RG16F,
			RG32F,
			RGB_DXT1,
			RGBA_DXT1,
			RGBA_DXT3,
			RGBA_DXT5,
			Depth24Stencil8,
			Depth16,
			Depth24
		};

		/**
		 * Checks whether the format is supported with the available render
		 * capabilities.
		 * @param caps Render capabilities.
		 * @param internalformat Format on the GPU.
		 * @param format Specifies the format in memory. If this is set to
		 * Invalid, then format==internalformat is assumed, if not, the function
		 * also checks whether the conversion from format to internalformat is
		 * supported.
		 * @return True if the combination of formats is supported.
		 */
		static bool supported(RenderCaps *caps,
		                      List internalformat,
		                      List format = Invalid);
		/**
		 * Returns the size in bytes for a certain number of texels with a
		 * format.
		 * @param format Texture format.
		 * @param texels Number of texels.
		 * @return Size of the texture data in bytes.
		 */
		static unsigned int getSize(TextureFormat::List format,
		                            unsigned int texels);
		/**
		 * Returns whether a format is compressed.
		 * @return True if the format is compressed.
		 */
		static bool isCompressed(TextureFormat::List format)
		{
			switch (format)
			{
				case RGB_DXT1:
				case RGBA_DXT1:
				case RGBA_DXT3:
				case RGBA_DXT5:
					return true;
				default:
					return false;
			}
		}
		/**
		 * Returns true if the format is a floating point format.
		 * @return True if the format contains floating point data.
		 */
		static bool isFloat(TextureFormat::List format)
		{
			switch (format)
			{
				case RGBA16F:
				case RGBA32F:
				case R16F:
				case R32F:
				case RG16F:
				case RG32F:
					return true;
				default:
					return false;
			}
		}
		/**
		 * Returns true if the format is a floating point format.
		 * @return True if the format contains floating point data.
		 */
		static bool isDepth(TextureFormat::List format)
		{
			switch (format)
			{
				case Depth24Stencil8:
				case Depth24:
				case Depth16:
					return true;
				default:
					return false;
			}
		}

		/**
		 * Converts a string to a texture format enum value. The string has to
		 * contain exactly the name of the enum member so that for example
		 * "RGBA16F" results in RGBA16F being returned.
		 * @param str Texture format name.
		 * @return Texture format.
		 */
		static TextureFormat::List fromString(const std::string &str);
	};
	/**
	 * Type of the texture.
	 */
	struct TextureType
	{
		enum List
		{
			/**
			 * 1D texture, can be used e.g. for lookup tables in shaders.
			 */
			Texture1D,
			/**
			 * Standard 2D texture.
			 */
			Texture2D,
			/**
			 * 3D texture.
			 */
			Texture3D,
			/**
			 * Cube map.
			 */
			TextureCube
		};
	};

	/**
	 * Texture resource class. This class is subclassed by 2D, 3D and cube
	 * textures and never used directly.
	 */
	class Texture : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param renderer Renderer to be used with this texture.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 * @param type Type of the texture.
			 */
			Texture(UploadManager &uploadmgr,
			        res::ResourceManager *rmgr,
			        const std::string &name,
			        TextureType::List type);
			/**
			 * Destructor.
			 */
			virtual ~Texture();

			/**
			 * Returns the handle to the texture. Usually this is the OpenGL
			 * texture handle.
			 * @return Handle to the texture.
			 */
			int getHandle()
			{
				return handle;
			}
			/**
			 * Returns the texture type (2D, 3D, cube map).
			 * @return Texture type.
			 */
			TextureType::List getTextureType()
			{
				return type;
			}

			typedef core::SharedPointer<Texture> Ptr;
		protected:
			unsigned int handle;
			TextureType::List type;
	};
}
}

#endif
