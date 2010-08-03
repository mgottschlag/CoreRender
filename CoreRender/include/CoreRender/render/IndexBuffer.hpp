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

#ifndef _CORERENDER_RENDER_INDEXBUFFER_HPP_INCLUDED_
#define _CORERENDER_RENDER_INDEXBUFFER_HPP_INCLUDED_

#include "RenderResource.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace render
{
	class IndexBuffer : public RenderResource
	{
		public:
			IndexBuffer(Renderer *renderer,
			             res::ResourceManager *rmgr,
			             const std::string &name);
			virtual ~IndexBuffer();

			void set(unsigned int size,
			         void *data,
			         bool copy = true);
			void update(unsigned int offset,
			            unsigned int size,
			            const void *data);
			void discardData();

			int getHandle()
			{
				return handle;
			}

			virtual const char *getType()
			{
				return "IndexBuffer";
			}

			typedef core::SharedPointer<IndexBuffer> Ptr;
		protected:
			unsigned int handle;

			tbb::spin_mutex datamutex;
			unsigned int size;
			void *data;
	};
}
}

#endif
