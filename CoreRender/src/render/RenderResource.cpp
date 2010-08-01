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

#include "CoreRender/render/RenderResource.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/core/Semaphore.hpp"
#include "CoreRender/res/ResourceManager.hpp"

namespace cr
{
namespace render
{
	RenderResource::RenderResource(Renderer *renderer,
	                               res::ResourceManager *rmgr,
	                               const std::string &name)
		: Resource(rmgr, name), renderer(renderer), uploading(false), waiting(0)
	{
		renderer->registerNew(this);
	}
	RenderResource::~RenderResource()
	{
	}

	bool RenderResource::create()
	{
		return true;
	}
	bool RenderResource::destroy()
	{
		return false;
	}
	bool RenderResource::upload()
	{
		uploadFinished();
		return true;
	}

	void RenderResource::registerUpload()
	{
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			// Do not submit this several times
			if (uploading)
				return;
			uploading = true;
		}
		renderer->registerUpload(this);
	}
	void RenderResource::uploadFinished()
	{
		core::Semaphore *wait = 0;
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			uploading = false;
			wait = waiting;
		}
		if (wait)
		{
			wait->post();
		}
	}

	void RenderResource::waitForUpload()
	{
		if (!uploading)
			return;
		core::Semaphore waiting;
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			if (!uploading)
				return;
			this->waiting = &waiting;
		}
		waiting.wait();
	}

	void RenderResource::onDelete()
	{
		// We probably cannot free GPU resources here
		getManager()->removeResource(this);
		renderer->registerDelete(this);
	}
}
}
