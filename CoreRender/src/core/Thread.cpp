/*
Copyright (C) 2009, Mathias Gottschlag

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

#include "CoreRender/core/Thread.hpp"

namespace cr
{
namespace core
{
#if defined(CORERENDER_UNIX)
	static void *threadProxy(void *param)
#elif defined(CORERENDER_WINDOWS)
	static DWORD WINAPI threadProxy(LPVOID param)
#endif
	{
		Functor *functor = (Functor*)param;
		functor->call();
		delete functor;
		return 0;
	}
	Thread::Thread()
#if defined(CORERENDER_WINDOWS)
		: thread(NULL)
#endif
	{
	}
	Thread::~Thread()
	{
#if defined(CORERENDER_WINDOWS)
		if (thread != NULL)
		{
			CloseHandle(thread);
		}
#endif
	}

	bool Thread::create(Functor *code)
	{
#if defined(CORERENDER_UNIX)
		if (pthread_create(&thread, NULL, threadProxy, code) != 0)
		{
			delete code;
			return false;
		}
#elif defined(CORERENDER_WINDOWS)
		thread = CreateThread (0, 0, threadProxy, code, 0, &threadid);
		if (thread == NULL)
			return false;
#endif
		return true;
	}
	void Thread::wait()
	{
#if defined(CORERENDER_UNIX)
		pthread_join(thread, 0);
#elif defined(CORERENDER_WINDOWS)
		WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
		thread = NULL;
#endif
	}
}
}
