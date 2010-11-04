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

#ifndef _CORERENDER_RENDER_TEXTURE2D_HPP_INCLUDED_
#define _CORERENDER_RENDER_TEXTURE2D_HPP_INCLUDED_

#include "Texture.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace render
{
	/**
	 * Normal 2D texture resource class.
	 *
	 * This class should be created via ResourceManager::getOrCreate().
	 */
	class Texture2D : public Texture
	{
		public:
			/**
			 * Constructor.
			 * @param renderer Renderer to be used with this texture.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Texture2D(UploadManager &uploadmgr,
			          res::ResourceManager *rmgr,
			          const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Texture2D();

			/**
			 * Fills the complete texture with new data, erasing all previous
			 * content.
			 * @param width New width of the texture.
			 * @param height New height of the texture.
			 * @param internalformat Color format on the GPU.
			 * @param format Input color format in RAM. This has to be set if
			 * the texture shall be initialized with data.
			 * @param data New texture data.
			 * @param copy If false, the resource does not copy the data but
			 * rather takes over ownership of the buffer to which data points
			 * and frees it itself later. For this, data has to be allocated
			 * with malloc() as it is later freed with free().
			 * @return Returns false if the texture could not be set (usually
			 * because of invalid input parameters).
			 */
			bool set(unsigned int width,
			         unsigned int height,
			         TextureFormat::List internalformat,
			         TextureFormat::List format = TextureFormat::Invalid,
			         void *data = 0,
			         bool copy = true);
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
			 * Updates only some parts of the texture. This does not work after
			 * discardImageData() has been called.
			 * @param x X coordinate of the rectangle to be updated.
			 * @param y Y coordinate of the rectangle to be updated.
			 * @param width Width of the rectangle to be updated.
			 * @param height Height of the rectangle to be updated.
			 * @param format Input color format in RAM.
			 * @param data New texture data.
			 * @return Returns false if the update failed, usually because of
			 * invalid input parameters.
			 */
			bool update(unsigned int x,
			            unsigned int y,
			            unsigned int width,
			            unsigned int height,
			            TextureFormat::List format,
			            void *data);
			/**
			 * Discards the image data in RAM and only keeps the copy in VRAM if
			 * possible.
			 */
			void discardImageData();

			virtual bool load();
			virtual bool unload();

			/**
			 * Returns the width of the texture.
			 * @return Width.
			 */
			unsigned int getWidth()
			{
				return currentdata.width;
			}
			/**
			 * Returns the height of the texture.
			 * @return Height.
			 */
			unsigned int getHeight()
			{
				return currentdata.height;
			}

			virtual const char *getType()
			{
				return "Texture2D";
			}

			typedef core::SharedPointer<Texture2D> Ptr;
		protected:
			virtual void *getUploadData();

			bool discarddata;

			struct TextureData
			{
				unsigned int width;
				unsigned int height;
				TextureFormat::List internalformat;
				TextureFormat::List format;
				unsigned int datasize;
				void *data;
			};

			tbb::spin_mutex imagemutex;
			TextureData currentdata;
	};
}
}

#endif
