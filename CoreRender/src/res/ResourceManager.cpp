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

#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/res/LoadingThread.hpp"
#include "CoreRender/core/Platform.hpp"

#include <iostream>
#include <cstdio>
#include <FreeImagePlus.h>

#if defined(CORERENDER_WINDOWS)
	#define snprintf sprintf_s
#endif

namespace cr
{
namespace res
{
	ResourceManager::ResourceManager(core::FileSystem::Ptr fs,
		core::Log::Ptr log)
		: namecounter(0), fs(fs), log(log)
	{
		// Start loading thread
		thread = new LoadingThread(log);
		thread->start();
	}
	ResourceManager::~ResourceManager()
	{
		thread->stop();
		delete thread;
	}

	bool ResourceManager::init()
	{
		FreeImage_Initialise(FALSE);
		// Create default resources
		// TODO
		return true;
	}
	bool ResourceManager::shutdown()
	{
		// Delete all resources
		// TODO
		FreeImage_DeInitialise();
		return true;
	}

	void ResourceManager::addResource(Resource *res)
	{
		tbb::mutex::scoped_lock lock(mutex);
		resources[res->getName()] = res;
	}
	void ResourceManager::removeResource(Resource *res)
	{
		tbb::mutex::scoped_lock lock(mutex);
		ResourceMap::iterator it = resources.find(res->getName());
		if (it == resources.end())
			return;
		if (it->second != res)
		{
			std::cout << "Warning: duplicated resource name." << std::endl;
			return;
		}
		resources.erase(it);
	}

	Resource::Ptr ResourceManager::getResource(const std::string &name)
	{
		tbb::mutex::scoped_lock lock(mutex);
		ResourceMap::iterator it = resources.find(name);
		if (it == resources.end())
			return 0;
		return it->second;
	}

	void ResourceManager::queueForLoading(Resource::Ptr res)
	{
		thread->queueForLoading(res);
	}
	void ResourceManager::prioritize(Resource::Ptr res)
	{
		// TODO
	}

	std::string ResourceManager::getInternalName()
	{
		while (1)
		{
			// Generate new name
			char name[32];
			snprintf(name, 32, "_internal_%u", ++namecounter);
			// Test whether the name is available
			tbb::mutex::scoped_lock lock(mutex);
			ResourceMap::iterator it = resources.find(name);
			if (it == resources.end())
				return name;
			// TODO: _Theoretically_ this could be not thread-safe
		}
	}

	void ResourceManager::setFileSystem(core::FileSystem::Ptr fs)
	{
		// TODO: Maybe not thread-safe
		this->fs = fs;
	}
}
}
