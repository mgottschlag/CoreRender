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

#include "GameMath.hpp"

#include <string>

namespace cr
{
namespace core
{
	/**
	 * Class to store time spans with nanosecond accuracy. This class can be
	 * computed as the difference of two Time or AbsoluteTime instances.
	 */
	class Duration
	{
		public:
			/**
			 * Constructor. Does not do any initialization. To get a time span
			 * with 0 nanoseconds, use for example Duration::Seconds(0).
			 */
			Duration()
			{
			}
			/**
			 * Copy constructor.
			 * @param other Duration to copy the value from.
			 */
			Duration(const Duration &other)
			{
				duration = other.duration;
			}
			/**
			 * Destructor.
			 */
			~Duration()
			{
			}

			/**
			 * Constructs a time span from hours.
			 * @param hours Value of the time span.
			 * @return Time span.
			 */
			static Duration Hours(math::int64 hours)
			{
				Duration d;
				d.duration = hours * 3600000000000LL;
				return d;
			}
			/**
			 * Constructs a time span from minutes.
			 * @param minutes Value of the time span.
			 * @return Time span.
			 */
			static Duration Minutes(math::int64 minutes)
			{
				Duration d;
				d.duration = minutes * 60000000000LL;
				return d;
			}
			/**
			 * Constructs a time span from seconds.
			 * @param seconds Value of the time span.
			 * @return Time span.
			 */
			static Duration Seconds(math::int64 seconds)
			{
				Duration d;
				d.duration = seconds * 1000000000LL;
				return d;
			}
			/**
			 * Constructs a time span from microseconds.
			 * @param microseconds Value of the time span.
			 * @return Time span.
			 */
			static Duration Microseconds(math::int64 microseconds)
			{
				Duration d;
				d.duration = microseconds * 1000;
				return d;
			}
			/**
			 * Constructs a time span from nanoseconds.
			 * @param nanoseconds Value of the time span.
			 * @return Time span.
			 */
			static Duration Nanoseconds(math::int64 nanoseconds)
			{
				Duration d;
				d.duration = nanoseconds;
				return d;
			}

			/**
			 * Returns the value of the time span in nanoseconds.
			 * @return Value of the time span.
			 */
			math::int64 getNanoseconds() const
			{
				return duration;
			}
			/**
			 * Returns the value of the time span in microseconds.
			 * @return Value of the time span.
			 */
			math::int64 getMicroseconds() const
			{
				return duration / 1000;
			}
			/**
			 * Returns the value of the time span in milliseconds.
			 * @return Value of the time span.
			 */
			math::int64 getMilliseconds() const
			{
				return duration / 1000000;
			}
			/**
			 * Returns the value of the time span in seconds.
			 * @return Value of the time span.
			 */
			math::int64 getSeconds() const
			{
				return duration / 1000000000LL;
			}
			/**
			 * Returns the value of the time span in minutes.
			 * @return Value of the time span.
			 */
			math::int64 getMinutes() const
			{
				return duration / 60000000000LL;
			}
			/**
			 * Returns the value of the time span in hours.
			 * @return Value of the time span.
			 */
			math::int64 getHours() const
			{
				return duration / 3600000000000LL;
			}

			/**
			 * Returns a formatted string containing the value of the time span.
			 *
			 * This can be done in two different formats, depending on the value
			 * of asseconds: If asseconds is true, the time is printed as a
			 * floating point number containing the seconds of the time span, if
			 * it is false, the function returns a string with the format
			 * "hh:mm:ss.uuuuuu" (where u are microseconds).
			 * @todo Actually implement fixed float width.
			 * @param asseconds Format of the string as described above.
			 * @return String containing the duration in text form.
			 */
			std::string toString(bool asseconds = true) const;

			Duration operator+(const Duration &other) const
			{
				Duration out;
				out.duration = duration + other.duration;
				return out;
			}
			Duration operator-(const Duration &other) const
			{
				Duration out;
				out.duration = duration - other.duration;
				return out;
			}
			Duration &operator+=(const Duration &other)
			{
				duration += other.duration;
				return *this;
			}
			Duration &operator-=(const Duration &other)
			{
				duration -= other.duration;
				return *this;
			}

			Duration operator*(int factor) const
			{
				Duration out(*this);
				out *= factor;
				return out;
			}
			Duration &operator*=(int factor)
			{
				duration *= factor;
				return *this;
			}
			Duration operator/(int divisor) const
			{
				Duration out(*this);
				out /= divisor;
				return out;
			}
			Duration &operator/=(int divisor)
			{
				duration /= divisor;
				return *this;
			}

			Duration &operator=(const Duration &other)
			{
				duration = other.duration;
				return *this;
			}

			bool operator>(const Duration &d) const
			{
				return duration > d.duration;
			}
			bool operator>=(const Duration &d) const
			{
				return duration >= d.duration;
			}
			bool operator<(const Duration &d) const
			{
				return duration < d.duration;
			}
			bool operator<=(const Duration &d) const
			{
				return duration <= d.duration;
			}
			bool operator==(const Duration &d) const
			{
				return duration == d.duration;
			}
			bool operator!=(const Duration &d) const
			{
				return duration != d.duration;
			}
		private:
			friend class Time;
			friend class AbsoluteTime;

			math::int64 duration;
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
			/**
			 * Constructor. Does not initialize the time, use Time::Now() for
			 * that.
			 */
			Time()
			{
			}
			/**
			 * Copy constructor.
			 * @param other Time to copy value from.
			 */
			Time(const Time &other)
			{
				time = other.time;
			}
			/**
			 * Returns the current time. If you are interested in a readable
			 * time format, look for AbsoluteTime::Now() instead.
			 * @return Current time.
			 */
			static Time Now();
			/**
			 * Returns the precision of the operating system functions
			 * called by Now().
			 * @return Precision of Time::Now().
			 */
			static Duration Precision();

			/**
			 * Sleeps for a certain duration. Depending on the operating
			 * system this might sleep much longer (for example on windows
			 * this usually waits for at least 16ms).
			 * @param duration Time for which execution shall be paused.
			 */
			static void sleep(const Duration &duration);
			/**
			 * Sleeps for a certain duration. Depending on the operating
			 * system this might sleep much longer (for example on windows
			 * this usually waits for at least 16ms).
			 * @param nanoseconds Time in nanoseconds for which execution
			 * shall be paused.
			 */
			static void sleep(math::uint64 nanoseconds);

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
			math::int64 time;
	};

	/**
	 * Absolute time with nanosecond accuracy (if supported by the hardware),
	 * starting at january 1st 1970. This has less precision and speed than Time
	 * usually.
	 */
	class AbsoluteTime
	{
		public:
			/**
			 * Constructor. Does not initialize the time, use
			 * AbsoluteTime::Now() for that.
			 */
			AbsoluteTime()
			{
			}
			/**
			 * Copy constructor.
			 * @param other Time to copy value from.
			 */
			AbsoluteTime(const AbsoluteTime &other)
			{
				time = other.time;
			}
			/**
			 * Returns the epoch of the values returned by
			 * AbsoluteTime::Now().
			 * @return Epoch.
			 */
			static AbsoluteTime Epoch()
			{
				AbsoluteTime t;
				t.time = 0;
				return t;
			}
			/**
			 * Returns the current time.
			 * @return Current time.
			 */
			static AbsoluteTime Now();
			/**
			 * Returns the precision of the operating system functions
			 * called by Now(). Note that this number sometimes is
			 * misleading. For example on windows the time has 100ns
			 * accuracy, but is only updated once per 16ms.
			 * @return Precision of Time::Now().
			 */
			static Duration Precision();

			/**
			 * Returns the time as a formatted string. The string is in the
			 * format "dd.mm.yyyy hh:mm:ss.mmm", where "mmm" are
			 * milliseconds.
			 * @param date If true, include "dd.mm.yyyy" in the string.
			 * @param time If true, include "hh:mm:ss" in the string.
			 * @param subsecond If true, include milliseconds in the string.
			 * @return Formatted string.
			 */
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
			math::int64 time;
	};
}
}

#endif
