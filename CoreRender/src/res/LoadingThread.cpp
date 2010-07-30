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

#include "CoreRender/res/LoadingThread.hpp"

namespace cr
{
namespace res
{
	LoadingThread::LoadingThread(core::Log::Ptr log)
		: stopping(false), log(log)
	{
	}
	LoadingThread::~LoadingThread()
	{
	}

	bool LoadingThread::start()
	{
		stopping = false;
		core::ClassFunctor<LoadingThread> *threadstart;
		threadstart = new core::ClassFunctor<LoadingThread>(this,
		                                                   &LoadingThread::entry);
		return thread.create(threadstart);
	}
	void LoadingThread::stop()
	{
		stopping = true;
		workavailable.post();
		thread.wait();
	}

	void LoadingThread::queueForLoading(Resource::Ptr res)
	{
		{
			tbb::spin_mutex::scoped_lock lock(queuemutex);
			loadingqueue.push(res);
		}
		workavailable.post();
	}

	void LoadingThread::entry(void)
	{
		while (true)
		{
			// Wait for loadable resources
			workavailable.wait();
			if (stopping)
				break;
			// Take one resource and load it
			Resource::Ptr res;
			{
				tbb::spin_mutex::scoped_lock lock(queuemutex);
				res = loadingqueue.front();
				loadingqueue.pop();
			}
			if (!res->load())
				log->error("Could not load resource \"%s\"", res->getName().c_str());
			else
				log->info("Loaded resource \"%s\"", res->getName().c_str());
		}
	}
}
}
