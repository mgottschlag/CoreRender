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

#ifndef _CORERENDER_CORE_HASHMAP_HPP_INCLUDED_
#define _CORERENDER_CORE_HASHMAP_HPP_INCLUDED_

#include "Platform.hpp"

#if defined(CORERENDER_MSVC)
	#include <hash_map>
#elif defined(CORERENDER_GCC)
	#ifdef __GXX_EXPERIMENTAL_CXX0X__
		#include <tr1/unordered_map>
	#else
		#include <ext/hash_map>
	#endif
namespace __gnu_cxx
{
	template<> struct hash<std::string>
	{
		size_t operator()(std::string const &s) const
		{
			return __stl_hash_string(s.c_str());
		}
	};
}
#else
	#include <map>
#endif

namespace cr
{
namespace core
{
	template<typename Key, typename Value> struct HashMap
	{
#if defined(CORERENDER_MSVC)
		typedef std::hash_map<Key, Value> Type;
#elif defined(CORERENDER_GCC)
	#ifdef __GXX_EXPERIMENTAL_CXX0X__
		typedef _std::tr1::unordered_map<Key, Value> Type;
	#else
		typedef __gnu_cxx::hash_map<Key, Value> Type;
	#endif
#else
		typedef std::map<Key, Value> Type;
#endif
	};
}
}

#endif
