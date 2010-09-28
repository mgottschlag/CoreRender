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

#ifndef _CORERENDER_UTIL_FRAMELIMITER_HPP_INCLUDED_
#define _CORERENDER_UTIL_FRAMELIMITER_HPP_INCLUDED_

#include "CoreRender/math/StdInt.hpp"
#include "CoreRender/core/Time.hpp"

namespace cr
{
namespace util
{
	/**
	 * Class which ensures that a loop (for example render loop) is only run a
	 * certain number of times per second. Note that the interval is set in
	 * microseconds, but you will never get that accuracy - on Windows for
	 * example the times can vary for about 16 milliseconds. The FrameLimiter
	 * class will try to ensure a certain number of iterations per second
	 * though, e.g. if the OS timer waits 30 instead of 20 ms once, the class
	 * will only wait 10 ms the next time wait() is called. As soon as one slice
	 * of time is used up, e.g. the OS timer waited 40 ms, the class will drop
	 * 20 ms and not let wait return instantly twice.
	 * @note Not thread-safe.
	 */
	class FrameLimiter
	{
		public:
			/**
			 * Constructor.
			 * @param interval Interval of the timer.
			 */
			FrameLimiter(core::Duration interval = core::Duration::Seconds(0));
			/**
			 * Destructor.
			 */
			~FrameLimiter();

			/**
			 * Sets the interval.
			 */
			void setInterval(core::Duration interval)
			{
				this->interval = interval;
			}
			/**
			 * Returns the interval.
			 */
			core::Duration getInterval()
			{
				return interval;
			}

			/**
			 * If between the last call of this function and this call is no
			 * more time than the interval set for this instance, the function
			 * will wait until the interval was used up. If the OS timer happens
			 * to wait longer than requested, the difference is substracted from
			 * the time for the next wait() call.
			 */
			void wait();
		private:
			core::Duration interval;
			core::Time lasttime;
	};
}
}

#endif
