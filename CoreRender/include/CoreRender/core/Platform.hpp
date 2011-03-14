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

#ifndef _CORERENDER_CORE_PLATFORM_HPP_INCLUDED_
#define _CORERENDER_CORE_PLATFORM_HPP_INCLUDED_

#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
	#define CORERENDER_WINDOWS
#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) && defined(__MACH__)
	#define CORERENDER_UNIX
#else
	#error Unsupported platform!
#endif

#if defined(_MSC_VER)
	#define CORERENDER_MSVC
#else
	#define CORERENDER_GCC
#endif

#endif
