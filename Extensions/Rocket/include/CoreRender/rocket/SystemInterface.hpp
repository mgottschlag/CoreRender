/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CORERENDER_ROCKET_SYSTEMINTERFACE_HPP_INCLUDED_
#define _CORERENDER_ROCKET_SYSTEMINTERFACE_HPP_INCLUDED_

#include "CoreRender/core/Time.hpp"

#include <Rocket/Core.h>

namespace cr
{
namespace rocket
{
	class SystemInterface : public Rocket::Core::SystemInterface
	{
	public:
		SystemInterface()
			: starttime(cr::core::Time::Now())
		{
		}
		virtual float GetElapsedTime()
		{
			math::int64 us = (cr::core::Time::Now() - starttime).getMicroseconds();
			return (float)us / 1000000;
		}
	private:
		cr::core::Time starttime;
	};
}
}

#endif
