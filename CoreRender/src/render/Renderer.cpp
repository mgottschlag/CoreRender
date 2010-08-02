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

#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/core/MemoryPool.hpp"

namespace cr
{
namespace render
{
	Renderer::Renderer(RenderContext::Ptr primary,
	                   RenderContext::Ptr secondary,
	                   core::Log::Ptr log,
	                   VideoDriver *driver)
		: primary(primary), secondary(secondary), log(log), driver(driver)
	{
		// Make context active for this thread
		if (secondary)
			secondary->makeCurrent();
		else
			primary->makeCurrent();
		// Create memory pools
		memory[0] = new core::MemoryPool();
		memory[1] = new core::MemoryPool();
	}
	Renderer::~Renderer()
	{
		// Release context
		if (secondary)
			secondary->makeCurrent(false);
		else
			primary->makeCurrent(false);
	}

	void Renderer::registerNew(RenderResource::Ptr res)
	{
	}
	void Renderer::registerUpload(RenderResource::Ptr res)
	{
	}
	void Renderer::registerDelete(RenderResource *res)
	{
	}

	void Renderer::enterThread()
	{
		// Make context current
		secondary->makeCurrent();
	}
	void Renderer::exitThread()
	{
		// Release context
		secondary->makeCurrent(false);
	}

	void Renderer::uploadNewObjects()
	{
	}
	void Renderer::prepareRendering()
	{
		// Move batches/passes to the render thread
		// TODO
		// Swap memory pools
		core::MemoryPool *pool = memory[1];
		memory[1] = memory[0];
		memory[0] = pool;
	}
	void Renderer::uploadObjects()
	{
		// TODO
	}
	void Renderer::deleteObjects()
	{
		// TODO
	}

	void Renderer::render()
	{
		// Upload changed objects
		uploadObjects();
		// Render passes
		// TODO
		// Reset memory pool
		// TODO
		// Delete unused objects
		deleteObjects();
	}
}
}
