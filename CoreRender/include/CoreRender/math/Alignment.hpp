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

#ifndef _CORERENDER_MATH_ALIGNMENT_HPP_INCLUDED_
#define _CORERENDER_MATH_ALIGNMENT_HPP_INCLUDED_

#include <new>

#define CORERENDER_MATH_ALIGNED_NEW_16 void *operator new(size_t size)\
{\
	return malloc_aligned_16(size);\
}\
void operator delete(void *p)\
{\
	free_aligned_16(p);\
}\
void *operator new[](size_t size)\
{\
	return malloc_aligned_16(size);\
}\
void operator delete[](void *p)\
{\
	free_aligned_16(p);\
}

namespace cr
{
namespace math
{
	static inline void *malloc_aligned_16(size_t size)
	{
		#if defined(__GLIBC__) && defined(__amd64__)
		// glibc returns 16-byte aligned memory on amd64
		void *p = malloc(size);
		if (!p)
			throw std::bad_alloc();
		return p;
		#elif _POSIX_VERSION >= 199506L
		void *data;
		if (posix_memalign(&data, 16, size))
			throw std::bad_alloc();
		return data;
		#elif defined(_MSC_VER)
		void *p = _aligned_malloc(size, 16);
		if (!p)
			throw std::bad_alloc();
		return p;
		#else
		// Save the number of bytes wasted one byte before the address returned
		char *mem = (char*)malloc(size + 16);
		if (!mem)
			throw std::bad_alloc();
		unsigned int offset = (unsigned int)mem % 16;
		if (offset == 0)
			offset = 16;
		mem[offset - 1] = offset;
		return mem + offset;
		#endif
	}
	static inline void free_aligned_16(void *p)
	{
		#if defined(__GLIBC__) && defined(__amd64__)
		free(p);
		#elif _POSIX_VERSION >= 199506L
		free(p);
		#elif defined(_MSC_VER)
		_aligned_free(p);
		#else
		char *data = p;
		data = data - data[-1];
		free(data);
		#endif
	}
}
}

#endif
