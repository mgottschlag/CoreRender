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

#include "CoreRender/core/Time.hpp"

#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
#include <time.h>
#include <windows.h>
#define usleep(x) Sleep((x)/1000)
#else
#include <unistd.h>
#include <sys/time.h>
#endif

namespace cr
{
namespace core
{
	Time &Time::get()
	{
		static Time time;
		return time;
	}
	Time::~Time()
	{
	}

	unsigned int Time::getTime()
	{
		return(unsigned int)(getSystemTime() - starttime);
	}
	uint64_t Time::getSystemTime()
	{
		#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
		return (uint64_t)GetTickCount() * 1000;
		#else
		struct timeval tv;
		gettimeofday(&tv, 0);
		return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
		#endif
	}

	void Time::sleep(unsigned int usecs)
	{
		usleep(usecs);
	}

	Time::Time()
	{
		starttime = getSystemTime();
	}
}
}
