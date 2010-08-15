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
#include "CoreRender/render/VideoDriver.hpp"

namespace cr
{
namespace render
{
	Renderer::Renderer(RenderContext::Ptr primary,
	                   RenderContext::Ptr secondary,
	                   core::Log::Ptr log,
	                   VideoDriver *driver,
	                   GraphicsEngine *input)
		: primary(primary), secondary(secondary), log(log), driver(driver),
		input(input)
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
		// Delete remaining render resources
		// TODO: Do we have to upload objects here? They will not be used.
		uploadNewObjects();
		uploadObjects();
		deleteObjects();
		// Release context
		if (secondary)
			secondary->makeCurrent(false);
		else
			primary->makeCurrent(false);
	}

	void Renderer::registerNew(RenderResource::Ptr res)
	{
		tbb::spin_mutex::scoped_lock lock(newmutex);
		newqueue.push(res);
	}
	void Renderer::registerShaderUpload(Shader::Ptr shader)
	{
		tbb::spin_mutex::scoped_lock lock(shaderuploadmutex);
		shaderuploadqueue.push(shader);
	}
	void Renderer::registerUpload(RenderResource::Ptr res)
	{
		tbb::spin_mutex::scoped_lock lock(uploadmutex);
		uploadqueue.push(res);
	}
	void Renderer::registerDelete(RenderResource *res)
	{
		tbb::spin_mutex::scoped_lock lock(deletemutex);
		deletequeue.push(res);
	}

	void Renderer::enterThread()
	{
		// Make context current
		primary->makeCurrent();
	}
	void Renderer::exitThread()
	{
		// Release context
		primary->makeCurrent(false);
	}

	void Renderer::uploadNewObjects()
	{
		// Upload new objects
		while (true)
		{
			RenderResource::Ptr next;
			{
				tbb::spin_mutex::scoped_lock lock(newmutex);
				if (newqueue.size() == 0)
					break;
				next = newqueue.front();
				newqueue.pop();
			}
			next->create();
		}
		// Upload shaders
		while (true)
		{
			Shader::Ptr next;
			{
				tbb::spin_mutex::scoped_lock lock(shaderuploadmutex);
				if (shaderuploadqueue.size() == 0)
					break;
				next = shaderuploadqueue.front();
				shaderuploadqueue.pop();
			}
			next->uploadShader();
		}
	}
	void Renderer::prepareRendering(PipelineInfo *renderdata,
	                                unsigned int pipelinecount)
	{
		// Store rendering data
		this->renderdata = renderdata;
		this->pipelinecount = pipelinecount;
		// Swap memory pools
		core::MemoryPool *pool = memory[1];
		memory[1] = memory[0];
		memory[0] = pool;
	}
	void Renderer::uploadObjects()
	{
		while (true)
		{
			RenderResource::Ptr next;
			{
				tbb::spin_mutex::scoped_lock lock(uploadmutex);
				if (uploadqueue.size() == 0)
					break;
				next = uploadqueue.front();
				uploadqueue.pop();
			}
			next->upload();
		}
	}
	void Renderer::deleteObjects()
	{
		while (true)
		{
			RenderResource *next;
			{
				tbb::spin_mutex::scoped_lock lock(deletemutex);
				if (deletequeue.size() == 0)
					break;
				next = deletequeue.front();
				deletequeue.pop();
			}
			next->destroy();
			delete next;
		}
	}

	void Renderer::render()
	{
		// Fetch input
		// TODO: Should not been done here
		primary->update(input);
		// Upload changed objects
		uploadObjects();
		// Render passes
		for (unsigned int i = 0; i < pipelinecount; i++)
		{
			renderPipeline(&renderdata[i]);
		}
		// Reset memory pool
		// TODO
		// Delete render data
		for (unsigned int i = 0; i < pipelinecount; i++)
		{
			for (unsigned int j = 0; j < renderdata[i].passes->batchcount; j++)
			{
				delete[] renderdata[i].passes[j].batches;
			}
			delete[] renderdata[i].passes;
		}
		delete[] renderdata;
		// TODO
		// Swap buffers
		primary->swapBuffers();
		// Delete unused objects
		deleteObjects();
	}

	void Renderer::renderPipeline(PipelineInfo *info)
	{
		for (unsigned int i = 0; i < info->passcount; i++)
		{
			renderPass(&info->passes[i]);
		}
	}
	void Renderer::renderPass(RenderPassInfo *info)
	{
		// Set target
		// TODO
		// Clear target
		driver->clear(true, true, core::Color(128, 0, 0, 255));
		// Draw batches
		for (unsigned int i = 0; i < info->batchcount; i++)
		{
			driver->draw(info->batches[i]);
		}
	}
}
}
