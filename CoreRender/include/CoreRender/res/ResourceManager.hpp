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

#ifndef _CORERENDER_RES_RESOURCEMANAGER_HPP_INCLUDED_
#define _CORERENDER_RES_RESOURCEMANAGER_HPP_INCLUDED_

#include "../core/FileSystem.hpp"
#include "Resource.hpp"

#include <map>
#include <tbb/mutex.h>

namespace cr
{
namespace res
{
	class ResourceManager
	{
		public:
			ResourceManager(core::FileSystem::Ptr fs);
			~ResourceManager();

			bool init();
			bool shutdown();

			void addResource(Resource *res);
			void removeResource(Resource *res);

			void queueForLoading(Resource::Ptr res);

			std::string getInternalName();

			void setFileSystem(core::FileSystem::Ptr fs);
			core::FileSystem::Ptr getFileSystem()
			{
				return fs;
			}
		private:
			typedef std::map<std::string, Resource*> ResourceMap;
			ResourceMap resources;

			unsigned int namecounter;

			core::FileSystem::Ptr fs;

			tbb::mutex mutex;
	};
}
}

#endif
