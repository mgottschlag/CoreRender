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

#ifndef _CORERENDER_RES_NAMEREGISTRY_HPP_INCLUDED_
#define _CORERENDER_RES_NAMEREGISTRY_HPP_INCLUDED_

#include <vector>
#include <string>
#include <tbb/mutex.h>

namespace cr
{
namespace res
{
	class NameRegistry
	{
		public:
			NameRegistry()
			{
			}
			~NameRegistry()
			{
			}

			unsigned int getAttrib(const std::string &name)
			{
				tbb::mutex::scoped_lock lock(attribmutex);
				// TODO: This is slow, maybe use a hash map additionally
				for (unsigned int i = 0; i < attribnames.size(); i++)
				{
					if (attribnames[i] == name)
						return i;
				}
				attribnames.push_back(name);
				return attribnames.size() - 1;
			}
			std::string getAttrib(unsigned int name)
			{
				tbb::mutex::scoped_lock lock(attribmutex);
				if (name >= attribnames.size())
					return "";
				return attribnames[name];
			}

			unsigned int getContext(const std::string &name)
			{
				tbb::mutex::scoped_lock lock(contextmutex);
				// TODO: This is slow, maybe use a hash map additionally
				for (unsigned int i = 0; i < contextnames.size(); i++)
				{
					if (contextnames[i] == name)
						return i;
				}
				contextnames.push_back(name);
				return contextnames.size() - 1;
			}
			std::string getContext(unsigned int name)
			{
				tbb::mutex::scoped_lock lock(contextmutex);
				if (name >= contextnames.size())
					return "";
				return contextnames[name];
			}
		private:
			tbb::mutex attribmutex;
			std::vector<std::string> attribnames;
			tbb::mutex contextmutex;
			std::vector<std::string> contextnames;
	};
}
}

#endif
