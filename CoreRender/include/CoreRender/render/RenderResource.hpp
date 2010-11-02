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

namespace cr
{
namespace render
{
	class UploadManager;

	/**
	 * Base class for all resources which are uploaded to the GPU. Those need
	 * to be treated differently as they have to be uploaded and destroyed in a
	 * thread with a valid render context.
	 *
	 * Basically, these resources register themselves first for creation
	 * (Renderer::registerNew()) which the happens at
	 * GraphicsEngine::beginFrame() and then for upload
	 * (Renderer::registerUpload()) which happens in the render thread.
	 * onDelete() also is overridden and just calls Renderer::registerDelete()
	 * when the reference count reaches 0 which causes the resource to be
	 * deleted in the render thread later.
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
			~RenderResource();

			/**
			 * Waits until the resource has been uploaded to the render thread.
			 * If you are modifying an existing resource, you might need to call
			 * this as the resource might not have been completely uploaded in
			 * the other thread. If you do not, your modifications might already
			 * be uploaded one frame too early. However, this is only necessary
			 * if modifications happened in two subsequent frames.
			 */
			void waitForUpload();

			/**
			 * Creates the GPU part of the resource. Called from
			 * Renderer::uploadNewObjects(). Do not call this manually.
			 */
			virtual bool create();
			/**
			 * Destroys the GPU part of the resource. Called from
			 * Renderer::deleteObjects(). Do not call this manually.
			 */
			virtual bool destroy();
			/**
			 * Uploads the resource to the GPU. Called from
			 * Renderer::uploadObjects(). Do not call this manually.
			 */
			virtual bool upload();

			typedef core::SharedPointer<RenderResource> Ptr;
		protected:
			UploadManager &getUploadManager()
			{
				return uploadmgr;
			}

			void registerUpload();
			void uploadFinished();

			virtual void onDelete();
		private:
			UploadManager &uploadmgr;

			tbb::spin_mutex uploadmutex;
			bool uploading;
			core::Semaphore *waiting;
	};
}
}

#endif
