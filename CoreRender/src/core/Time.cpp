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

#include "CoreRender/core/Platform.hpp"

#include <time.h>

#if defined(CORERENDER_UNIX)
	#include <sys/time.h>
	#include <unistd.h>
#elif defined(CORERENDER_WINDOWS)
	#include <windows.h>
#endif

#include <sstream>
#include <iomanip>

namespace cr
{
namespace core
{
	std::string Duration::toString(bool asseconds) const
	{
		if (asseconds)
		{
			double seconds = duration;
			seconds /= 60000000000.0;
			std::ostringstream stream;
			stream << seconds;
			return stream.str();
		}
		else
		{
			// Split time
			unsigned int hours = duration / 3600000000000LL;
			int64_t remaining = duration % 3600000000000LL;
			unsigned int minutes = remaining / 60000000000LL;
			remaining = remaining % 60000000000LL;
			double seconds = remaining;
			seconds /= 60000000000.0;
			// Create string
			std::ostringstream stream;
			stream << std::setfill('0') << std::setw(2) << hours << ":";
			stream << std::setfill('0') << std::setw(2) << minutes << ":";
			stream << seconds;
			return stream.str();
		}
	}

	Time Time::Now()
	{
#if defined(CORERENDER_UNIX)
		struct timespec unixtime;
	#ifdef _POSIX_MONOTONIC_CLOCK
		clock_gettime(CLOCK_MONOTONIC, &unixtime);
	#else
		clock_gettime(CLOCK_REALTIME, &unixtime);
	#endif
		Time t;
		t.time = unixtime.tv_nsec + unixtime.tv_sec * 1000000000;
		return t;
#elif defined(CORERENDER_WINDOWS)
		Time t;
		t.time = (int64_t)GetTickCount() * 1000000;
		return t;
#else
	#error Unimplemented!
#endif
	}
	Duration Time::Precision()
	{
#if defined(CORERENDER_WINDOWS)
		return Duration::Nanoseconds(1000);
#else
		return Duration::Nanoseconds(1);
#endif
	}

	void Time::sleep(const Duration &duration)
	{
		sleep(duration.getNanoseconds());
	}
	void Time::sleep(uint64_t nanoseconds)
	{
#if defined(CORERENDER_UNIX)
		usleep(nanoseconds / 1000);
#elif defined(CORERENDER_WINDOWS)
		Sleep((unsigned int)(nanoseconds / 1000000));
#else
	#error Unimplemented!
#endif
	}

	AbsoluteTime AbsoluteTime::Now()
	{
#if defined(CORERENDER_UNIX)
		struct timespec unixtime;
		clock_gettime(CLOCK_REALTIME, &unixtime);
		AbsoluteTime t;
		t.time = unixtime.tv_nsec + unixtime.tv_sec * 1000000000;
		return t;
#elif defined(CORERENDER_WINDOWS)
		FILETIME filetime;
		GetSystemTimeAsFileTime(&filetime);
		ULARGE_INTEGER windowstime;
		windowstime.LowPart = filetime.dwLowDateTime;
		windowstime.HighPart = filetime.dwHighDateTime;
		AbsoluteTime t;
		t.time = (windowstime.QuadPart - 116444736000000000ULL) * 100;
		return t;
#else
	#error Unimplemented!
#endif
	}
	Duration AbsoluteTime::Precision()
	{
#if defined(CORERENDER_WINDOWS)
		return Duration::Nanoseconds(100);
#else
		return Duration::Nanoseconds(1);
#endif
	}

	std::string AbsoluteTime::toString(bool date,
	                                   bool time,
	                                   bool subsecond) const
	{
#if defined(CORERENDER_UNIX)
	// Split time
	time_t unixtime = this->time / 1000000000;
	struct tm splitted;
	localtime_r(&unixtime, &splitted);
	// Print date and time
	std::ostringstream stream;
	if (date)
	{
		stream << std::setfill('0') << std::setw(2) << splitted.tm_mday << "/";
		stream << std::setfill('0') << std::setw(2) << splitted.tm_mon << "/";
		stream << splitted.tm_year + 1900 << " ";
	}
	if (time)
	{
		stream << std::setfill('0') << std::setw(2) << splitted.tm_hour << ":";
		stream << std::setfill('0') << std::setw(2) << splitted.tm_min << ":";
		stream << std::setfill('0') << std::setw(2) << splitted.tm_sec;
		// Print sub-second time
		if (subsecond)
		{
			unsigned int msecs = (this->time % 1000000000) / 1000000;
			stream << "." << std::setfill('0') << std::setw(3) << msecs;
		}
	}
	return stream.str();
#elif defined(CORERENDER_WINDOWS)
	// Split time
	ULARGE_INTEGER windowstime;
	windowstime.QuadPart = (this->time / 100) - 116444736000000000ULL;
	FILETIME filetime;
	filetime.dwLowDateTime = windowstime.LowPart;
	filetime.dwHighDateTime = windowstime.HighPart;
	SYSTEMTIME systemtime;
	if (!FileTimeToSystemTime(&filetime, &systemtime))
		return "";
	// Print date and time
	std::ostringstream stream;
	if (date)
	{
		stream << std::setfill('0') << std::setw(2) << systemtime.wDay << "/";
		stream << std::setfill('0') << std::setw(2) << systemtime.wMonth << "/";
		stream << systemtime.wYear << " ";
	}
	if (time)
	{
		stream << std::setfill('0') << std::setw(2) << systemtime.wHour << ":";
		stream << std::setfill('0') << std::setw(2) << systemtime.wMinute << ":";
		stream << std::setfill('0') << std::setw(2) << systemtime.wSecond;
		// Print sub-second time
		if (subsecond)
			stream << "." << std::setfill('0') << std::setw(3) << systemtime.wMilliseconds;
	}
	return stream.str();
#else
	#error Unimplemented!
#endif
	}
}
}
