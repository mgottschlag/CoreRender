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

#ifndef _CORERENDER_RES_LOADINGTHREAD_HPP_INCLUDED_
#define _CORERENDER_RES_LOADINGTHREAD_HPP_INCLUDED_

#include "Resource.hpp"
#include "CoreRender/core/Semaphore.hpp"
#include "CoreRender/core/Thread.hpp"
#include "CoreRender/core/Log.hpp"

#include <queue>

namespace cr
{
namespace res
{
	class LoadingThread
	{
		public:
			LoadingThread(core::Log::Ptr log);
			~LoadingThread();

			bool start();
			void stop();

			void queueForLoading(Resource::Ptr res);
		private:
			void entry(void);

			core::Thread thread;

			core::Semaphore workavailable;
			bool stopping;

			tbb::spin_mutex queuemutex;
			std::queue<Resource::Ptr> loadingqueue;

			core::Log::Ptr log;
	};
}
}

#endif
