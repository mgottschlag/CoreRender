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
		 * @param uploadformat Specifies the format in memory.
		 * @param uploadingdata This specifies whether the texture is going to
		 * be filled with data from RAM on creation. If this is false,
		 * uploadformat is ignored.
		 * @return True if the combination of formats is supported.
		 */
		static bool supported(const RenderCaps &caps,
		                      List internalformat,
		                      bool uploadingdata = false,
		                      List uploadformat = RGBA8);
		/**
		 * Returns the size in bytes for a certain number of texels with a
		 * format.
		 * @param format Texture format.
		 * @param texels Number of texels.
		 * @return Size of the texture data in bytes.
		 */
		static unsigned int getSize(TextureFormat::List format,
		                            unsigned int width,
		                            unsigned int height,
		                            unsigned int depth = 1);
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
	 * Specifies the way pixels are read from the texture in shaders.
	 */
	struct TextureFiltering
	{
		enum List
		{
			/**
			 * Texture coordinates are rounded and the nearest pixel is fetched.
			 */
			Nearest,
			/**
			 * Interpolation between the nearest pixels takes place. If mipmaps
			 * are available, they will be used. This is the default value for
			 * new textures.
			 */
			Linear
		};
	};

	/**
	 * Side of a cube map texture.
	 */
	struct CubeSide
	{
		enum List
		{
			PositiveX,
			NegativeX,
			PositiveY,
			NegativeY,
			PositiveZ,
			NegativeZ
		};
	};

	/**
	 * Texture resource class. This class can hold a texture of any type.
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
			        const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Texture();

			/**
			 * Creates a new 1D texture, erasing all previous content.
			 * @param width New width of the texture.
			 * @param height New height of the texture.
			 * @param internalformat Color format on the GPU.
			 * @param format Input color format in RAM. This has to be set if
			 * the texture shall be initialized with data.
			 * @param data New texture data. Can be 0 for an uninitialized
			 * texture.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool set1D(unsigned int width,
			           TextureFormat::List internalformat,
			           TextureFormat::List format = TextureFormat::Invalid,
			           void *data = 0,
			           bool copy = true);
			/**
			 * Updates only some parts of the texture. This does not work after
			 * discardImageData() has been called or if the texture was not
			 * initialized with data from RAM before.
			 * @param x X coordinate of the rectangle to be updated.
			 * @param width Width of the rectangle to be updated.
			 * @param data New texture data.
			 * @return Returns false if the update failed, usually because of
			 * invalid input parameters.
			 */
			bool update1D(unsigned int x,
			              unsigned int width,
			              void *data);

			/**
			 * Creates a new 2D texture, erasing all previous content.
			 * @param width New width of the texture.
			 * @param height New height of the texture.
			 * @param internalformat Color format on the GPU.
			 * @param format Input color format in RAM. This has to be set if
			 * the texture shall be initialized with data.
			 * @param data New texture data. Can be 0 for an uninitialized
			 * texture.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool set2D(unsigned int width,
			           unsigned int height,
			           TextureFormat::List internalformat,
			           TextureFormat::List format = TextureFormat::Invalid,
			           void *data = 0,
			           bool copy = true);
			/**
			 * Updates only some parts of the texture. This does not work after
			 * discardImageData() has been called or if the texture was not
			 * initialized with data from RAM before.
			 * @param x X coordinate of the rectangle to be updated.
			 * @param y Y coordinate of the rectangle to be updated.
			 * @param width Width of the rectangle to be updated.
			 * @param height Height of the rectangle to be updated.
			 * @param data New texture data.
			 * @return Returns false if the update failed, usually because of
			 * invalid input parameters.
			 */
			bool update2D(unsigned int x,
			              unsigned int y,
			              unsigned int width,
			              unsigned int height,
			              void *data);

			/**
			 * Creates a new cube map, erasing all previous content.
			 * @param width New width of the texture.
			 * @param height New height of the texture.
			 * @param internalformat Color format on the GPU.
			 * @param format Input color format in RAM. This has to be set if
			 * the texture shall be initialized with data.
			 * @param data New texture data. Can be 0 for an uninitialized
			 * texture.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool setCube(unsigned int width,
			             unsigned int height,
			             TextureFormat::List internalformat,
			             TextureFormat::List format = TextureFormat::Invalid,
			             void *data = 0,
			             bool copy = true);
			/**
			 * Updates only some parts of the texture. This does not work after
			 * discardImageData() has been called or if the texture was not
			 * initialized with data from RAM before.
			 * @param side Side of the cubemap.
			 * @param x X coordinate of the rectangle to be updated.
			 * @param y Y coordinate of the rectangle to be updated.
			 * @param width Width of the rectangle to be updated.
			 * @param height Height of the rectangle to be updated.
			 * @return Returns false if the update failed, usually because of
			 * invalid input parameters.
			 */
			bool updateCube(CubeSide::List side,
			                unsigned int x,
			                unsigned int y,
			                unsigned int width,
			                unsigned int height,
			                void *data);

			/**
			 * Creates a new 3D texture, erasing all previous content.
			 * @param width New width of the texture.
			 * @param height New height of the texture.
			 * @param depth New depth of the texture.
			 * @param internalformat Color format on the GPU.
			 * @param format Input color format in RAM. This has to be set if
			 * the texture shall be initialized with data.
			 * @param data New texture data. Can be 0 for an uninitialized
			 * texture.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool set3D(unsigned int width,
			           unsigned int height,
			           unsigned int depth,
			           TextureFormat::List internalformat,
			           TextureFormat::List format = TextureFormat::Invalid,
			           void *data = 0,
			           bool copy = true);
			/**
			 * Updates only some parts of the texture. This does not work after
			 * discardImageData() has been called or if the texture was not
			 * initialized with data from RAM before.
			 * @param x X coordinate of the rectangle to be updated.
			 * @param y Y coordinate of the rectangle to be updated.
			 * @param width Width of the rectangle to be updated.
			 * @param height Height of the rectangle to be updated.
			 * @param data New texture data.
			 * @return Returns false if the update failed, usually because of
			 * invalid input parameters.
			 */
			bool update3D(unsigned int x,
			              unsigned int y,
			              unsigned int z,
			              unsigned int width,
			              unsigned int height,
			              unsigned int depth,
			              void *data);

			/**
			 * Updates the whole texture without changing the internal format
			 * or the dimensions of the texture.
			 * @param format Input color format in RAM.
			 * @param data New texture data.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool update(TextureFormat::List format,
			            void *data,
			            bool copy = true);

			/**
			 * Sets whether mipmaps are generated and used for this texture.
			 * This includes generating mipmaps when rendering to the texture.
			 * If you are going to upload data from RAM and want mipmaps, you
			 * should to call this before the texture is uploaded.
			 * Mipmaps are only used for rendering if filtering is set to
			 * linear.
			 * @param mipmaps If true, mipmaps are generated and used.
			 */
			void setMipmapsEnabled(bool mipmaps);
			/**
			 * Returns whether mipmaps are enabled for this texture.
			 * @return True if mipmaps are generated and used.
			 */
			bool getMipmapsEnabled();

			/**
			 * Sets whether this texture is used for depth comparisons or not.
			 * This has to be set to true if you use the texture as a
			 * sampler2DShadow or similar in a shader. This has to be set to
			 * false if you just want to read the data from the texture, which
			 * is the default value.
			 * @param depthcompare If set to true, this texture is used for
			 * depth comparisons.
			 */
			void setDepthCompare(bool depthcompare);
			/**
			 * Returns whether this texture is set up for depth comparisons
			 * (sampler2DShadow) or for reading texture data.
			 * @return True if the texture is set up for depth comparisons.
			 */
			bool getDepthCompare();

			/**
			 * Sets the filtering which is done when data is fetched from the
			 * texture in shaders.
			 * @param filtering New filtering mode.
			 */
			void setFiltering(TextureFiltering::List filtering);
			/**
			 * Returns the filtering mode set for this texture.
			 * @return Current filtering mode.
			 */
			TextureFiltering::List getFiltering();

			/**
			 * Discards the image data in RAM and only keeps the copy in VRAM if
			 * possible.
			 */
			void discardImageData();

			virtual bool load();
			virtual bool unload();

			/**
			 * Returns the width of the texture.
			 * @return Width of the texture.
			 */
			unsigned int getWidth()
			{
				return currentdata.width;
			}
			/**
			 * Returns the height of the texture.
			 * Only returns valid data if the texture is not a 1D texture.
			 * @return Height of the texture.
			 */
			unsigned int getHeight()
			{
				return currentdata.height;
			}
			/**
			 * Returns the depth of the texture.
			 * Only returns valid data if the texture is a 3D texture.
			 * @return Depth of the texture.
			 */
			unsigned int getDepth()
			{
				return currentdata.depth;
			}
			/**
			 * Returns the internal format of the texture.
			 * @return Internal format of the texture.
			 */
			TextureFormat::List getInternalFormat()
			{
				return currentdata.internalformat;
			}
			/**
			 * Returns the format of the texture data in RAM. This value is not
			 * valid if the texture was not uploaded from RAM previously.
			 * @return Input format of the texture.
			 */
			TextureFormat::List getFormat()
			{
				return currentdata.format;
			}

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
			 * Returns whether the uploaded texture has mipmaps. Do not call
			 * this directly, use getMipmapsEnabled() instead, this is for the
			 * video driver.
			 */
			bool hasMipmaps()
			{
				return hasmipmaps;
			}
			/**
			 * Returns the OpenGL type of the texture, for example
			 * GL_TEXTURE_2D.
			 */
			unsigned int getOpenGLType()
			{
				return opengltype;
			}
			/**
			 * Returns the texture type (2D, 3D, cube map).
			 * @return Texture type.
			 */
			TextureType::List getTextureType()
			{
				return currentdata.type;
			}

			virtual const char *getType()
			{
				return "Texture";
			}

			typedef core::SharedPointer<Texture> Ptr;
		protected:
			struct TextureData
			{
				TextureType::List type;
				unsigned int width;
				unsigned int height;
				unsigned int depth;
				TextureFormat::List internalformat;
				TextureFormat::List format;
				unsigned int datasize;
				void *data;
				TextureFiltering::List filtering;
				bool mipmaps;
				bool createmipmaps;
				bool depthcompare;
			};

			virtual void *getUploadData();

			unsigned int handle;
			bool hasmipmaps;
			unsigned int opengltype;
		private:
			bool set(TextureType::List type,
			         unsigned int width,
			         unsigned int height,
			         unsigned int depth,
			         TextureFormat::List internalformat,
			         TextureFormat::List format,
			         unsigned int datasize,
			         void *data,
			         bool copy,
			         bool createmipmaps = true);

			bool discarddata;

			tbb::spin_mutex imagemutex;
			TextureData currentdata;
	};
}
}

#endif
