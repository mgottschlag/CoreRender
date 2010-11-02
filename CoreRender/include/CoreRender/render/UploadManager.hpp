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

#ifndef _CORERENDER_RENDER_UPLOADMANAGER_HPP_INCLUDED_
#define _CORERENDER_RENDER_UPLOADMANAGER_HPP_INCLUDED_

#include "FrameData.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace core
{
	class MemoryPool;
}
namespace render
{
	class RenderResource;

	/**
	 * Keeps track of the RenderResource objects which need to be uploaded to
	 * the graphics card or deleted from it. This additional work has to be done
	 * as these uploads/deletions have to be done from within
	 * GraphicsEngine::render() or GraphicsEngine::discard() as they need a
	 * valid OpenGL context. Also this ensures that editing resources at any
	 * time is possible even if they are in use.
	 */
	class UploadManager
	{
		public:
			UploadManager();
			~UploadManager();

			void registerUpload(RenderResource *resource);
			void registerDeletion(RenderResource *resource);

			void getLists(UploadLists &lists, core::MemoryPool *memory);
			void uploadResources(UploadLists &lists);
			void deleteResources(UploadLists &lists);
		private:
			tbb::spin_mutex listmutex;
			std::vector<RenderResource*> upload;
			std::vector<RenderResource*> deletion;
	};
}
}

#endif
