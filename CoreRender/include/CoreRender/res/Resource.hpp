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

class TiXmlDocument;

namespace cr
{
namespace core
{
	class Semaphore;
}
namespace res
{
	class ResourceManager;

	/**
	 * Base class for all engine-managed resources. Resources have a unique name
	 * and in most cases can be loaded from a file. They are not created
	 * directly via the constructor but rather through the ResourceManager
	 * interface and are identified via a unique name.
	 *
	 * Resource loading is done asynchronously, loadFromFile() returns
	 * immediately and queues this resource for loading in the resource loading
	 * thread. One can then either poll via isLoaded() and isLoading() to get
	 * the current state of the resource or call waitForLoading() to wait until
	 * the resoure (and the resources it depends upon) are ready to be used.
	 *
	 * @note By default, resources are not thread-safe and should only be
	 * manipulated by one thread at a time.
	 */
	class Resource : public core::ReferenceCounted
	{
		public:
			/**
			 * Constructor. This calls ResourceManager::addResource().
			 * @param rmgr The ResourceManager this resource is inserted into.
			 * @param name The unique name used for this resource.
			 */
			Resource(ResourceManager *rmgr, const std::string &name);
			/**
			 * Destructor. Also removes the resource from the manager.
			 */
			virtual ~Resource();

			/**
			 * Returns the name of the resource.
			 * @note This function is thread-safe.
			 */
			std::string getName();

			/**
			 * Queues the resource for loading from a file.
			 * @param path Path of the resource to be loaded from.
			 */
			void loadFromFile(const std::string &path);

			/**
			 * Loads the resource from a file. This shall be called only by the
			 * resource loading thread.
			 * @return Returns false if the resource could not be called.
			 * @note The inplementation of this function has to call
			 * finishLoading() or waitForLoading() will deadlock.
			 */
			virtual bool load();
			/**
			 * Unloads the resource removing all data previously loaded.
			 * @return False if unloading is not supported.
			 */
			virtual bool unload();
			/**
			 * Returns true if the resource was successfully loaded before. Note
			 * that the result of this function is only valid when isLoading()
			 * is false as only then the loading process has finished.
			 * @return Success of the last call to load().
			 */
			bool isLoaded()
			{
				return loaded;
			}
			/**
			 * Returns whether the resource is currently in the loading queue or
			 * being loaded.
			 * @return True if the resource is being loaded.
			 */
			bool isLoading()
			{
				tbb::spin_mutex::scoped_lock lock(statemutex);
				return loading;
			}
			/**
			 * Moves the resource to the front of the loading queue loading it
			 * before all other resources in the queue. This can be done to
			 * minimize blocking due to resource loading.
			 */
			void prioritizeLoading();
			/**
			 * Waits until loading of the resource has finished.
			 * @param recursive If true, also wait for resources this resource
			 * on.
			 * @param highpriority If true, the function calls
			 * prioritizeLoading() before waiting.
			 */
			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			/**
			 * Returns the name of the resource class as a string.
			 * @return Resource type name.
			 */
			virtual const char *getType() = 0;

			/**
			 * Returns the ResourceManager this resource belongs to.
			 * @return Resource manager of this resource.
			 */
			ResourceManager *getManager()
			{
				return rmgr;
			}

			typedef core::SharedPointer<Resource> Ptr;
		protected:
			void finishLoading(bool loaded);

			bool loadResourceFile(TiXmlDocument &xml);

			const std::string &getPath()
			{
				return path;
			}
		private:
			void queueForLoading();

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
