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

#include "CoreRender/core/ConditionVariable.hpp"

namespace cr
{
namespace core
{
	ConditionVariable::ConditionVariable()
	{
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&cond, 0);
	}
	ConditionVariable::~ConditionVariable()
	{
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
	}

	void ConditionVariable::lock()
	{
		pthread_mutex_lock(&mutex);
	}
	void ConditionVariable::unlock()
	{
		pthread_mutex_unlock(&mutex);
	}

	void ConditionVariable::signal()
	{
		pthread_cond_signal(&cond);
	}
	void ConditionVariable::broadcast()
	{
		pthread_cond_broadcast(&cond);
	}
	void ConditionVariable::wait()
	{
		pthread_cond_wait(&cond, &mutex);
	}
}
}
