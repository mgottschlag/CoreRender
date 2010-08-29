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
#include <stdlib.h>

namespace cr
{
namespace res
{
	class LoadingThread;

	/**
	 * Class which manages creation/loading of resources.
	 *
	 * Resources are created through ResourceFactory instances, one for each
	 * resource type, which are registered through addFactory().
	 *
	 * Usually one wants to use getOrLoad() for resource creation so that
	 * existing instances are reused.
	 */
	class ResourceManager
	{
		public:
			/**
			 * Constructor.
			 * @param fs File system to be used for resource loading.
			 * @param log Log writer to be used for the resource system.
			 */
			ResourceManager(core::FileSystem::Ptr fs, core::Log::Ptr log);
			/**
			 * Destructor.
			 */
			~ResourceManager();

			/**
			 * Initializes the resource manager.
			 * @return Returns whether initialization was successful.
			 */
			bool init();
			/**
			 * Destroys the resource manager.
			 * @note All resources have to be destroyed prior to this.
			 * @return Returns false if 
			 */
			bool shutdown();

			/**
			 * Adds a new resource type to the resource system.
			 * @param name Resource type name, has to be the same as what is
			 * returned by Resource::getType()->
			 * @param factory Resource factory for the resource type.
			 * @note This function is thread-safe.
			 */
			void addFactory(const std::string &name, ResourceFactory::Ptr factory);
			/**
			 * Removed a certain resource type from the resource system.
			 * @param name Resource type name to be removed.
			 * @note This function is thread-safe.
			 */
			void removeFactory(const std::string &name);
			/**
			 * Returns the resource factory for the given resource type.
			 * @param name Resource type name.
			 * @return Resource factory or null pointer if the resource type has
			 * not been registered before.
			 * @note This function is thread-safe.
			 */
			ResourceFactory::Ptr getFactory(const std::string &name);

			/**
			 * Checks whether a resource already exists and returns it or if no
			 * resource was found creates one and queues it for loading.
			 * @param type Resource type name.
			 * @param path File name to load the resource from.
			 * @param name Resource name. If this is an empty string, the path
			 * will be used as the resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 */
			Resource::Ptr getOrLoad(const std::string &type,
			                        const std::string &path,
			                        const std::string &name = "");
			/**
			 * Checks whether a resource already exists and returns it or if no
			 * resource was found creates one and queues it for loading.
			 * @param type Resource type name.
			 * @param path File name to load the resource from.
			 * @param name Resource name. If this is an empty string, the path
			 * will be used as the resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 * @note This function only does a static check of the pointer, so
			 * take care that the factory produces instances of the correct
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

			/**
			 * Checks whether a resource already exists and returns it or if no
			 * resource was found creates one.
			 * @param type Resource type name.
			 * @param name Resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 */
			Resource::Ptr getOrCreate(const std::string &type,
			                          const std::string &name);
			/**
			 * Checks whether a resource already exists and returns it or if no
			 * resource was found creates one.
			 * @param type Resource type name.
			 * @param name Resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 * @note This function only does a static check of the pointer, so
			 * take care that the factory produces instances of the correct
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

			/**
			 * Creates a new resource with of certain type.
			 * @param type Resource type name.
			 * @param name Resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 */
			Resource::Ptr createResource(const std::string &type,
			                             const std::string &name = "");
			/**
			 * Creates a new resource with of certain type.
			 * @param type Resource type name.
			 * @param name Resource name.
			 * @return Resource with the given name or 0 if no resource could be
			 * created.
			 * @note This function only does a static check of the pointer, so
			 * take care that the factory produces instances of the correct
			 * class.
			 */
			template<class T> typename T::Ptr createResource(const std::string &type,
			                                                 const std::string &name = "")
			{
				// TODO: Dynamic checks in debug version?
				Resource::Ptr res = createResource(type, name);
				typename T::Ptr derived = (T*)res.get();
				return derived;
			}

			/**
			 * Adds a resource to the resource list. This is called by
			 * Resource::Resource(), do not call this manually.
			 * @note This function is thread-safe.
			 * @param res Resource to be added.
			 */
			void addResource(Resource *res);
			/**
			 * Removes a resource from the resource list. This is called by
			 * Resource::~Resource(), do not call this manually.
			 * This function also does a quick check for duplicated resource
			 * names.
			 * @note This function is thread-safe.
			 * @param res Resource to be removed.
			 */
			void removeResource(Resource *res);

			/**
			 * Returns the resource with the given name.
			 * @return Resource with the name or 0 if no resource was found.
			 * @note This function is thread-safe.
			 */
			Resource::Ptr getResource(const std::string &name);

			/**
			 * Queues a resource for loading. This is called by
			 * Resource::loadFromFile(), do not call this manually.
			 * @param res Resource to be loaded.
			 * @note This function is thread-safe.
			 */
			void queueForLoading(Resource::Ptr res);
			/**
			 * Prioritizes loading of a certain resource. This is called by
			 * Resource::prioritizeLoading(), do not call this manually.
			 * @param res Resource to be loaded.
			 * @note This function is thread-safe.
			 */
			void prioritize(Resource::Ptr res);

			/**
			 * Returns a new unique name for a new resource. The name will be
			 * in the form "_internal_<counter>" where counter is a unique
			 * integer. This can be used for procedurally created resources.
			 * @return Unique resource identifier.
			 */
			std::string getInternalName();

			/**
			 * Sets the file system to be used for loading resources.
			 * @param fs New file system used for the resource system.
			 */
			void setFileSystem(core::FileSystem::Ptr fs);
			/**
			 * Returns the file system used for resource loading.
			 * @return File system used for resource loading.
			 */
			core::FileSystem::Ptr getFileSystem()
			{
				return fs;
			}
			/**
			 * Returns the log writer used for the resource system.
			 * @return Log writer.
			 */
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
