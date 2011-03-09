/*
Copyright (C) 2011, Mathias Gottschlag

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

#ifndef _CORERENDER_RENDER_IMAGE_HPP_INCLUDED_
#define _CORERENDER_RENDER_IMAGE_HPP_INCLUDED_

#include "Texture.hpp"
#include "../core/FileSystem.hpp"

namespace cr
{
namespace render
{
	/**
	 * Class which can load image data from files.
	 */
	class Image
	{
		public:
			/**
			 * Constructor.
			 */
			Image();
			/**
			 * Destructor.
			 */
			~Image();

			/**
			 * Creates a 2D image from image data.
			 *
			 * @param width Width of the image.
			 * @param height Height of the image.
			 * @param format Format of the image data.
			 * @param data Image data.
			 * @param createmipmaps If true, mipmaps are created for the image.
			 * @return Created image.
			 */
			static Image *create2D(unsigned int width,
			                       unsigned int height,
			                       TextureFormat::List format,
			                       void *data,
			                       bool copy = true);

			void set(unsigned int width,
			         unsigned int height,
			         unsigned int depth,
			         TextureType::List type,
			         TextureFormat::List format,
			         bool deletedata,
			         void *imagedata,
			         unsigned int imagesize,
			         bool hasmipmaps);

			/**
			 * Loads an image from a file.
			 *
			 * @param fs File system to use.
			 * @param filename Absolute file name of the image.
			 * @return Image or 0 if an error occurred.
			 */
			static Image *load(core::FileSystem::Ptr fs, const std::string &filename);
			/**
			 * Loads an image from data in RAM.
			 *
			 * @param filename File name of the image.
			 * @param datasize Size of the file contents.
			 * @param data File contents.
			 * @return Image or 0 if an error occurred.
			 */
			static Image *load(const std::string &filename,
			                   unsigned int datasize,
			                   unsigned char *data);

			/**
			 * Saves the image to a file.
			 *
			 * The format is chosen based on the file extension.
			 *
			 * @param fs File system to use.
			 * @param filename Output file name.
			 * @return True if the file could be saved.
			 */
			bool save(core::FileSystem::Ptr fs, const std::string &filename);

			unsigned int getWidth()
			{
				return width;
			}
			unsigned int getHeight()
			{
				return height;
			}
			unsigned int getDepth()
			{
				return depth;
			}
			TextureType::List getType()
			{
				return type;
			}
			TextureFormat::List getFormat()
			{
				return format;
			}
			void *getImageData(bool dropownership = false)
			{
				if (dropownership)
					deletedata = false;
				return imagedata;
			}
			unsigned int getImageSize()
			{
				return imagesize;
			}
			bool hasMipmaps()
			{
				return hasmipmaps;
			}
		protected:
			unsigned int width;
			unsigned int height;
			unsigned int depth;
			TextureType::List type;
			TextureFormat::List format;
			bool deletedata;
			void *imagedata;
			unsigned int imagesize;
			bool hasmipmaps;
	};
}
}

#endif
