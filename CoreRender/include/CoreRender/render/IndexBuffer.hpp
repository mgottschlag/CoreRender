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
	 * Hint for the video driver where to store the index buffer.
	 */
	struct IndexBufferUsage
	{
		enum List
		{
			/**
			 * Static data which is never or rarely modified. Usually stored in
			 * fast VRAM. This maps to GL_STATIC_DRAW in OpenGL.
			 */
			Static,
			/**
			 * Data which is written once per frame and then used only once.
			 * This maps to GL_STREAM_DRAW in OpenGL.
			 */
			Stream,
			/**
			 * Data which is changed and used multiple times. This maps to
			 * GL_DYNAMIC_DRAW in OpenGL.
			 */
			Dynamic
		};
	};

	/**
	 * Resource holding index data for one or multiple meshes. This directly
	 * translates into an OpenGL IBO. Multiple meshes even with different
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
			IndexBuffer(UploadManager &uploadmgr,
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
			 * @param usage Usage hint for best performance.
			 * @param copy If set to false, the buffer creates no copy of the
			 * data but rather takes over ownership of it and frees it itself
			 * later. For this, data has to be allocated with malloc() as it is
			 * later freed with free().
			 * @param discard If set to true, the data is removed from RAM after
			 * it has been uploaded to VRAM. Set this to false if you want to
			 * call update() later.
			 */
			void set(unsigned int size,
			         void *data,
			         IndexBufferUsage::List usage = IndexBufferUsage::Static,
			         bool copy = true,
			         bool discard = true);
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
			virtual void *getUploadData();

			unsigned int handle;

			struct BufferData
			{
				IndexBufferUsage::List usage;
				unsigned int size;
				void *data;
			};
		private:
			BufferData currentdata;

			bool discarddata;
	};
}
}

#endif
