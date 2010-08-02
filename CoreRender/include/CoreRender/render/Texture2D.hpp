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
	class Texture2D : public Texture
	{
		public:
			Texture2D(Renderer *renderer,
			          res::ResourceManager *rmgr,
			          const std::string &name);
			virtual ~Texture2D();

			bool set(unsigned int width,
			         unsigned int height,
			         TextureFormat::List internalformat,
			         TextureFormat::List format = TextureFormat::Invalid,
			         void *data = 0,
			         bool copy = true);
			bool update(TextureFormat::List format,
			            void *data,
			            bool copy = true);
			bool update(unsigned int x,
			            unsigned int y,
			            unsigned int width,
			            unsigned int height,
			            TextureFormat::List format,
			            void *data);
			void discardImageData();

			virtual bool load();
			virtual bool unload();

			unsigned int getWidth()
			{
				return width;
			}
			unsigned int getHeight()
			{
				return height;
			}

			virtual const char *getType()
			{
				return "Texture2D";
			}

			typedef core::SharedPointer<Texture2D> Ptr;
		protected:
			bool discarddata;

			tbb::spin_mutex imagemutex;

			unsigned int width;
			unsigned int height;
			TextureFormat::List internalformat;
			TextureFormat::List format;
			void *data;
	};
}
}

#endif
