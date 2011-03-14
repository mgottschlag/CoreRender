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

#include "CoreRender/core/Semaphore.hpp"

namespace cr
{
namespace core
{
	Semaphore::Semaphore(int value)
	{
		// Unnamed semaphores not implemented on Mac OSX
#if defined(CORERENDER_MACOSX)
		semaphore_create(mach_task_self(), &sem, SYNC_POLICY_FIFO, value);
#elif defined(CORERENDER_UNIX)
		sem_init(&sem, 0, value);
#elif defined(CORERENDER_WINDOWS)
		// TODO: Maximum number?
		sem = CreateSemaphore(NULL, value, 20, NULL);
#endif
	}
	Semaphore::~Semaphore()
	{
#if defined(CORERENDER_MACOSX)
		semaphore_destroy(mach_task_self(), sem);
#elif defined(CORERENDER_UNIX)
		sem_destroy(&sem);
#elif defined(CORERENDER_WINDOWS)
		CloseHandle(sem);
#endif
	}

	void Semaphore::wait()
	{
#if defined(CORERENDER_MACOSX)
		semaphore_wait(sem);
#elif defined(CORERENDER_UNIX)
		sem_wait(&sem);
#elif defined(CORERENDER_WINDOWS)
		WaitForSingleObject(sem, INFINITE);
#endif
	}
	void Semaphore::post()
	{
#if defined(CORERENDER_MACOSX)
		semaphore_signal(sem);
#elif defined(CORERENDER_UNIX)
		sem_post(&sem);
#elif defined(CORERENDER_WINDOWS)
		ReleaseSemaphore(sem, 1, NULL);
#endif
	}
}
}
