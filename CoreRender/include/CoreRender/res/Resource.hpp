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

#ifndef _CORERENDER_RES_RESOURCE_HPP_INCLUDED_
#define _CORERENDER_RES_RESOURCE_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"

#include <string>
#include <tbb/spin_mutex.h>
#include <vector>

namespace cr
{
namespace core
{
	class Semaphore;
}
namespace res
{
	class ResourceManager;

	class Resource : public core::ReferenceCounted
	{
		public:
			Resource(ResourceManager *rmgr, const std::string &name);
			virtual ~Resource();

			void setName(const std::string &name);
			std::string getName();

			void loadFromFile(const std::string &path);

			/**
			 * @todo Can this be private?
			 */
			void queueForLoading();

			virtual bool load();
			virtual bool unload();
			bool isLoaded()
			{
				return loaded;
			}
			bool isLoading()
			{
				tbb::spin_mutex::scoped_lock lock(statemutex);
				return loading;
			}
			void prioritizeLoading();
			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			virtual const char *getType() = 0;

			ResourceManager *getManager()
			{
				return rmgr;
			}

			typedef core::SharedPointer<Resource> Ptr;
		protected:
			void finishLoading(bool loaded);

			const std::string &getPath();
		private:
			tbb::spin_mutex statemutex;
			bool loaded;
			bool loading;
			std::vector<core::Semaphore*> waiting;

			std::string name;
			std::string path;

			ResourceManager *rmgr;
	};
}
}

#endif
