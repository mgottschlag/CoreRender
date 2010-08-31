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

#ifndef _CORERENDER_CORE_TIME_HPP_INCLUDED_
#define _CORERENDER_CORE_TIME_HPP_INCLUDED_

#include "../math/StdInt.hpp"

#include <string>

namespace cr
{
namespace core
{
	class Duration
	{
		public:
			Duration()
			{
			}
			~Duration()
			{
			}

			static Duration Hours(int64_t hours)
			{
				Duration d;
				d.duration = hours * 3600000000000LL;
				return d;
			}
			static Duration Minutes(int64_t minutes)
			{
				Duration d;
				d.duration = minutes * 60000000000LL;
				return d;
			}
			static Duration Seconds(int64_t seconds)
			{
				Duration d;
				d.duration = seconds * 1000000000LL;
				return d;
			}
			static Duration Microseconds(int64_t microseconds)
			{
				Duration d;
				d.duration = microseconds * 1000;
				return d;
			}
			static Duration Nanoseconds(int64_t nanoseconds)
			{
				Duration d;
				d.duration = nanoseconds;
				return d;
			}

			int64_t getNanoseconds()
			{
				return duration;
			}
			int64_t getMicroseconds()
			{
				return duration / 1000;
			}
			int64_t getMilliseconds()
			{
				return duration / 1000000;
			}
			int64_t getSeconds()
			{
				return duration / 1000000000LL;
			}
			int64_t getMinutes()
			{
				return duration / 60000000000LL;
			}
			int64_t getHours()
			{
				return duration / 3600000000000LL;
			}

			std::string toString(bool asseconds = true) const;

			Duration operator+(const Duration &other) const;
			Duration operator-(const Duration &other) const;
			Duration &operator+=(const Duration &other);
			Duration &operator-=(const Duration &other);

			bool operator>(const Duration &d)
			{
				return duration > d.duration;
			}
			bool operator>=(const Duration &d)
			{
				return duration >= d.duration;
			}
			bool operator<(const Duration &d)
			{
				return duration < d.duration;
			}
			bool operator<=(const Duration &d)
			{
				return duration <= d.duration;
			}
			bool operator==(const Duration &d)
			{
				return duration == d.duration;
			}
			bool operator!=(const Duration &d)
			{
				return duration != d.duration;
			}
		private:
			friend class Time;
			friend class AbsoluteTime;

			int64_t duration;
	};

	/**
	 * Absolute time with nanosecond accuracy starting at un undefined time in
	 * the past. This class has better precision than AbsoluteTime usually and
	 * is a bit faster. Also the time returned by Now() usually is monotonic and
	 * not changed if the operating system time is changed.
	 */
	class Time
	{
		public:
			Time()
			{
			}
			Time(const Time &other)
			{
				time = other.time;
			}
			static Time Now();
			static Duration Precision();

			static void sleep(const Duration &duration);
			static void sleep(uint64_t nanoseconds);

			Duration operator-(Time &other) const
			{
				Duration d;
				d.duration = time - other.time;
				return d;
			}
			Time operator-(Duration &duration) const
			{
				Time t;
				t.time = time - duration.duration;
				return t;
			}
			Time operator+(Duration &duration) const
			{
				Time t;
				t.time = time + duration.duration;
				return t;
			}
			Time &operator+=(Duration &duration)
			{
				time += duration.duration;
				return *this;
			}
			Time &operator-=(Duration &duration)
			{
				time -= duration.duration;
				return *this;
			}

			bool operator>(const Time &t)
			{
				return time > t.time;
			}
			bool operator>=(const Time &t)
			{
				return time >= t.time;
			}
			bool operator<(const Time &t)
			{
				return time < t.time;
			}
			bool operator<=(const Time &t)
			{
				return time <= t.time;
			}
			bool operator==(const Time &t)
			{
				return time == t.time;
			}
			bool operator!=(const Time &t)
			{
				return time != t.time;
			}
		private:
			int64_t time;
	};

	/**
	 * Absolute time with nanosecond accuracy (if supported by the hardware),
	 * starting at january 1st 1970. This has less precision and speed than Time
	 * usually.
	 */
	class AbsoluteTime
	{
		public:
			AbsoluteTime()
			{
			}
			AbsoluteTime(const AbsoluteTime &other)
			{
				time = other.time;
			}
			static AbsoluteTime Epoch()
			{
				AbsoluteTime t;
				t.time = 0;
				return t;
			}
			static AbsoluteTime Now();
			static Duration Precision();

			std::string toString(bool date = true,
			                     bool time = true,
			                     bool subsecond = true) const;

			inline Duration operator-(AbsoluteTime &other) const
			{
				Duration d;
				d.duration = time - other.time;
				return d;
			}
			inline AbsoluteTime operator-(Duration &duration) const
			{
				AbsoluteTime t;
				t.time = time - duration.duration;
				return t;
			}
			inline AbsoluteTime operator+(Duration &duration) const
			{
				AbsoluteTime t;
				t.time = time + duration.duration;
				return t;
			}
			inline AbsoluteTime &operator+=(Duration &duration)
			{
				time += duration.duration;
				return *this;
			}
			inline AbsoluteTime &operator-=(Duration &duration)
			{
				time -= duration.duration;
				return *this;
			}

			bool operator>(const AbsoluteTime &t)
			{
				return time > t.time;
			}
			bool operator>=(const AbsoluteTime &t)
			{
				return time >= t.time;
			}
			bool operator<(const AbsoluteTime &t)
			{
				return time < t.time;
			}
			bool operator<=(const AbsoluteTime &t)
			{
				return time <= t.time;
			}
			bool operator==(const AbsoluteTime &t)
			{
				return time == t.time;
			}
			bool operator!=(const AbsoluteTime &t)
			{
				return time != t.time;
			}
		private:
			int64_t time;
	};
}
}

#endif
