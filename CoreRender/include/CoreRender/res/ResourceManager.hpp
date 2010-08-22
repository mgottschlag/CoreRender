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
#include "../core/Log.hpp"
#include "Resource.hpp"
#include "ResourceFactory.hpp"

#include <map>
#include <tbb/mutex.h>

namespace cr
{
namespace res
{
	class LoadingThread;

	class ResourceManager
	{
		public:
			ResourceManager(core::FileSystem::Ptr fs, core::Log::Ptr log);
			~ResourceManager();

			bool init();
			bool shutdown();

			void addFactory(const std::string &name, ResourceFactory::Ptr factory);
			void removeFactory(const std::string &name);
			ResourceFactory::Ptr getFactory(const std::string &name);

			Resource::Ptr getOrLoad(const std::string &type,
			                        const std::string &path,
			                        const std::string &name = "");
			/**
			 * @note This function only does a static check of the pointer, so
			 * take care that the factory produces instances of the correct+
			 * class.
			 */
			template<class T> typename T::Ptr getOrLoad(const std::string &type,
			                                            const std::string &path,
			                                            const std::string &name = "")
			{
				// TODO: Dynamic checks in debug version?
				Resource::Ptr res = getOrLoad(type, path, name);
				typename T::Ptr derived = (T*)res.get();
				return derived;
			}

			Resource::Ptr getOrCreate(const std::string &type,
			                          const std::string &name);
			/**
			 * @note This function only does a static check of the pointer, so
			 * take care that the factory produces instances of the correct+
			 * class.
			 */
			template<class T> typename T::Ptr getOrCreate(const std::string &type,
			                                              const std::string &name)
			{
				// TODO: Dynamic checks in debug version?
				Resource::Ptr res = getOrLoad(type, name);
				typename T::Ptr derived = (T*)res.get();
				return derived;
			}

			Resource::Ptr createResource(const std::string &type,
			                             const std::string &name = "");
			template<class T> typename T::Ptr createResource(const std::string &type,
			                                                 const std::string &name = "")
			{
				// TODO: Dynamic checks in debug version?
				Resource::Ptr res = createResource(type, name);
				typename T::Ptr derived = (T*)res.get();
				return derived;
			}

			void addResource(Resource *res);
			void removeResource(Resource *res);

			Resource::Ptr getResource(const std::string &name);

			void queueForLoading(Resource::Ptr res);
			void prioritize(Resource::Ptr res);

			std::string getInternalName();

			void setFileSystem(core::FileSystem::Ptr fs);
			core::FileSystem::Ptr getFileSystem()
			{
				return fs;
			}
			core::Log::Ptr getLog()
			{
				return log;
			}
		private:
			typedef std::map<std::string, Resource*> ResourceMap;
			ResourceMap resources;

			tbb::spin_mutex factorymutex;
			typedef std::map<std::string, ResourceFactory::Ptr> FactoryMap;
			FactoryMap factories;

			unsigned int namecounter;

			core::FileSystem::Ptr fs;
			core::Log::Ptr log;

			LoadingThread *thread;

			tbb::mutex mutex;
	};
}
}

#endif
