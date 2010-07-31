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

#ifndef _CORERENDER_CORE_THREAD_HPP_INCLUDED_
#define _CORERENDER_CORE_THREAD_HPP_INCLUDED_

#include "Functor.hpp"
#include "Platform.hpp"

#if defined(CORERENDER_UNIX)
	#include <pthread.h>
#elif defined(CORERENDER_WINDOWS)
	#include <Windows.h>
#endif

namespace cr
{
namespace core
{
	class Thread
	{
		public:
			Thread();
			~Thread();

			bool create(Functor *code);
			void wait();
		private:
#if defined(CORERENDER_UNIX)
			pthread_t thread;
#elif defined(CORERENDER_WINDOWS)
			HANDLE thread;
			DWORD threadid;
#endif
	};
}
}

#endif
