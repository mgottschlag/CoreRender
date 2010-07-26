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

namespace cr
{
namespace core
{
	/**
	 * Operating system independent class to measure time or to wait for some
	 * time.
	 * @note All functions in this class are completely thread-safe.
	 */
	class Time
	{
		public:
			/**
			 * Returns the global time object.
			 */
			static Time &get();
			/**
			 * Destructor.
			 */
			~Time();

			/**
			 * Returns the time in microseconds passed after the constructor
			 * has been called.
			 */
			unsigned int getTime();
			/**
			 * Returns the system time in microseconds.
			 */
			static uint64_t getSystemTime();

			/**
			 * Sleeps for at least a certain amount of microseconds.
			 * @note This actually can sleep much longer, depending on the
			 * operating system.
			 */
			static void sleep(unsigned int usecs);
		private:
			Time();

			uint64_t starttime;
	};
}
}

#endif
