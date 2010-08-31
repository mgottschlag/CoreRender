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

#include "CoreRender/util/FrameLimiter.hpp"
#include "CoreRender/core/Time.hpp"

namespace cr
{
namespace util
{
	FrameLimiter::FrameLimiter(core::Duration interval)
		: interval(interval)
	{
		lasttime = core::Time::Now();
	}
	FrameLimiter::~FrameLimiter()
	{
	}

	void FrameLimiter::wait()
	{
		// Compute target time
		lasttime = lasttime + interval;
		// Get current time
		core::Time currenttime = core::Time::Now();
		if (currenttime < lasttime)
		{
			// TODO: Statistics
			core::Time::sleep(lasttime - currenttime);
		}
		else
		{
			// TODO: Warning/statistics
			if (currenttime > lasttime + interval)
				lasttime = core::Time::Now();
		}
	}
}
}
