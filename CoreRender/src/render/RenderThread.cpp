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

#include "CoreRender/render/RenderThread.hpp"
#include "CoreRender/core/Functor.hpp"
#include "CoreRender/render/Renderer.hpp"

namespace cr
{
namespace render
{
	RenderThread::RenderThread(Renderer *renderer)
		: renderer(renderer)
	{
	}
	RenderThread::~RenderThread()
	{
	}

	bool RenderThread::start()
	{
		stopping = false;
		core::ClassFunctor<RenderThread> *threadstart;
		threadstart = new core::ClassFunctor<RenderThread>(this,
		                                                   &RenderThread::entry);
		return thread.create(threadstart);
	}
	bool RenderThread::stop()
	{
		stopping = true;
		framestart.post();
		thread.wait();
		return true;
	}

	void RenderThread::startFrame()
	{
		framestart.post();
	}
	void RenderThread::waitForFrame()
	{
		frameend.wait();
	}

	void RenderThread::entry()
	{
		// Register thread in the renderer
		renderer->enterThread();
		// We do not want the first access to waitForFrame() to deadlock
		frameend.post();
		// Render loop
		while (true)
		{
			// Wait for next frame to begin
			framestart.wait();
			if (stopping)
				break;
			// Render
			renderer->render();
			// Notify other thread that the frame has ended
			frameend.post();
		}
		// Deregister thread in the renderer
		renderer->exitThread();
	}
}
}
