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

#ifndef _CORERENDER_RENDER_RENDERRESOURCE_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERRESOURCE_HPP_INCLUDED_

#include "../res/Resource.hpp"

#include <tbb/mutex.h>

namespace cr
{
namespace render
{
	class UploadManager;

	/**
	 * Base class for all resources which are uploaded to the GPU. Those need
	 * to be treated differently as they have to be uploaded and destroyed in a
	 * thread with a valid render context. This class handles uploading and
	 * destoying itself through UploadManager. Note that the destructor is
	 * always called within a valid OpenGL context, so GPU resources should be
	 * destroyed there.
	 */
	class RenderResource : public res::Resource
	{
		public:
			/**
			 * Constructor.
			 * @param uploadmgr Class which is used for uploading/deleting the
			 * resource.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			RenderResource(UploadManager &uploadmgr,
			               res::ResourceManager *rmgr,
			               const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~RenderResource();

			/**
			 * Uploads the resource to the GPU. Called by UploadManager. Do not
			 * call this manually. When this function is called, it is
			 * guaranteed that a valid OpenGL context is current.
			 */
			virtual void upload(void *data) = 0;

			/**
			 * This function prepares the data which shall be uploaded. The
			 * function is called when UploadManager collects the upload data
			 * before a frame is finished. It also resets the value returned by
			 * isUploading().
			 * @note This locks the resource, if this function and
			 * registerUpload() are called simultaneously, one of them will
			 * wait. This function might take some time if getUploadData() is
			 * slow.
			 * @todo This should probably be able to use MemoryPool.
			 */
			void *prepareForUpload();

			typedef core::SharedPointer<RenderResource> Ptr;
		protected:
			UploadManager &getUploadManager()
			{
				return uploadmgr;
			}

			/**
			 * This function shall return the resource data which has to be
			 * uploaded. It only shall be called by prepareForUpload().
			 */
			virtual void *getUploadData() = 0;

			/**
			 * Queues the resource for uploading to the video driver. Shall be
			 * called whenever the resource changes. Calling this function
			 * multiple times within one frame causes only the first call
			 * actually to be forwarded to the UploadManager. This sets the
			 * value returned by isUploading() to true.
			 */
			void registerUpload();
			virtual void onDelete();
		private:
			UploadManager &uploadmgr;

			tbb::mutex uploadmutex;
			bool uploading;
	};
}
}

#endif
