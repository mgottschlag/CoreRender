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
	/**
	 * Resource holding index data for one or multiple meshes. This directly
	 * translates into an OpenGL IBO. Multiple meshes oeven with different
	 * vertex buffers can be stored within one index buffer to minimize state
	 * switches.
	 *
	 * This class should be created via ResourceManager::getOrCreate().
	 */
	class IndexBuffer : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param renderer Renderer to be used with this index buffer.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			IndexBuffer(Renderer *renderer,
			             res::ResourceManager *rmgr,
			             const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~IndexBuffer();

			/**
			 * Fills the whole index buffer with new data, erasing all previous
			 * content.
			 * @param size New size of the buffer in bytes.
			 * @param data New content of the buffer. Must hold at least "size"
			 * bytes.
			 * @param copy If set to false, the buffer creates no copy of the
			 * data but rather takes over ownership of it and frees it itself
			 * later. For this, data has to be allocated with malloc() as it is
			 * later freed with free().
			 */
			void set(unsigned int size,
			         void *data,
			         bool copy = true);
			/**
			 * Updates a part of the index buffer. This can be used if multiple
			 * meshes share one index buffer.
			 * @param offset Byte offset of the area to be updated.
			 * @param size Number of bytes to be updated.
			 * @param data New data of this buffer area.
			 */
			void update(unsigned int offset,
			            unsigned int size,
			            const void *data);
			/**
			 * Discards the buffer data in RAM and only keeps the copy in VRAM
			 * if possible.
			 */
			void discardData();

			/**
			 * Returns the handle to this index buffer.
			 * @return Handle of the buffer. Usually this is the OpenGL IBO
			 * handle.
			 */
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
