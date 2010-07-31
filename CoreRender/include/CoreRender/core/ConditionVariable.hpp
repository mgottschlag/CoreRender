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

#ifndef _CORERENDER_CORE_CONDITIONVARIABLE_HPP_INCLUDED_
#define _CORERENDER_CORE_CONDITIONVARIABLE_HPP_INCLUDED_

#if defined(CORERENDER_UNIX)
	#include <pthread.h>
#else
	#error Unimplemented
#endif

namespace cr
{
namespace core
{
	/**
	 * Condition variable class for safe variable locking. One thread locks the
	 * condition variable and then waits for it, the other (signalling) thread
	 * locks it and then signals the condition variable which makes the other
	 * thread resume. Note that wait() has to be called BEFORE the signal()
	 * occurs or a deadlock will take place! Example:
	 * \code
	 * ConditionVariable done;
	 * void test()
	 * {
	 *     // Lock the variable first!
	 *     done.lock();
	 *     // Start the thread
	 *     startThread(thread);
	 *     // Wait for the thread
	 *     done.wait();
	 *     done.unlock();
	 * }
	 * void thread()
	 * {
	 *     // Do something here
	 *     // Signal the first thread that we are done
	 *     done.lock();
	 *     done.signal();
	 *     done.unlock();
	 * }
	 * \endcode
	 * If you only have one waiting thread, use signal(). As soon as there can
	 * be more than one, you have to use broadcast().
	 */
	class ConditionVariable
	{
		public:
			/**
			 * Constructor.
			 */
			ConditionVariable();
			/**
			 * Destructor.
			 */
			~ConditionVariable();

			/**
			 * Locks the condition variable. Has to be called before signal() or
			 * wait().
			 */
			void lock();
			/**
			 * Unlocks the condition variable. Has to be called after signal()
			 * or wait().
			 */
			void unlock();

			/**
			 * Causes the waiting thread to wake up.
			 */
			void signal();
			/**
			 * Causes multiple waiting threads to wake up.
			 */
			void broadcast();
			/**
			 * Waits for a signal() to happen.
			 */
			void wait();
		private:
#if defined(CORERENDER_UNIX)
			pthread_mutex_t mutex;
			pthread_cond_t cond;
#else
#endif
	};
}
}

#endif
