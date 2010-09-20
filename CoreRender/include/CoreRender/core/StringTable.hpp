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

#ifndef _CORERENDER_CORE_STRINGTABLE_HPP_INCLUDED_
#define _CORERENDER_CORE_STRINGTABLE_HPP_INCLUDED_

#include "HashMap.hpp"

#include <string>
#include <tbb/spin_mutex.h>
#include <vector>

namespace cr
{
namespace core
{
	class StringTable
	{
		public:
			StringTable()
			{
			}
			~StringTable()
			{
			}

			unsigned int getHandle(const std::string &s)
			{
				tbb::spin_mutex::scoped_lock lock(mutex);
				// Find existing entry
				HashMap<std::string, unsigned int>::Type::iterator it;
				it = handles.find(s);
				if (it != handles.end())
					return it->second;
				// Create new string table entry
				unsigned int handle = strings.size();
				strings.push_back(s);
				handles.insert(std::make_pair(s, handle));
				return handle;
			}
			std::string getString(unsigned int index)
			{
				tbb::spin_mutex::scoped_lock lock(mutex);
				if (index >= strings.size())
					return "";
				return strings[index];
			}
		private:
			HashMap<std::string, unsigned int>::Type handles;
			std::vector<std::string> strings;

			tbb::spin_mutex mutex;
	};
}
}

#endif
