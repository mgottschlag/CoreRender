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
		// List resources still in use
		tbb::mutex::scoped_lock lock(mutex);
		for (ResourceMap::iterator it = resources.begin();
		     it != resources.end(); it++)
		{
			log->error("Resource \"%s\" still referenced, expect crashes.",
			           it->first.c_str());
		}
		// Deinitialize image loader
		FreeImage_DeInitialise();
		return true;
	}

	void ResourceManager::addFactory(const std::string &name, ResourceFactory::Ptr factory)
	{
		tbb::spin_mutex::scoped_lock lock(factorymutex);
		factories[name] = factory;
	}
	void ResourceManager::removeFactory(const std::string &name)
	{
		tbb::spin_mutex::scoped_lock lock(factorymutex);
		FactoryMap::iterator it = factories.find(name);
		if (it == factories.end())
			return;
		factories.erase(it);
	}
	ResourceFactory::Ptr ResourceManager::getFactory(const std::string &name)
	{
		tbb::spin_mutex::scoped_lock lock(factorymutex);
		FactoryMap::iterator it = factories.find(name);
		if (it == factories.end())
			return 0;
		return it->second;
	}

	Resource::Ptr ResourceManager::getOrLoad(const std::string &type,
	                                         const std::string &path,
	                                         const std::string &name)
	{
		// TODO: This is not thread-safe!
		// Get existing resource
		Resource::Ptr existing;
		if (name == "")
			existing = getResource(path);
		else
			existing = getResource(name);
		if (existing)
		{
			// Check type of existing resource
			if (type != existing->getType())
				return 0;
			return existing;
		}
		// Create resource
		ResourceFactory::Ptr factory = getFactory(type);
		if (!factory)
			return 0;
		Resource::Ptr created;
		if (name == "")
			created = factory->create(path);
		else
			created = factory->create(name);
		// Load resource
		created->loadFromFile(path);
		return created;
	}
	Resource::Ptr ResourceManager::getOrCreate(const std::string &type,
	                                           const std::string &name)
	{
		// Get existing resource
		Resource::Ptr existing = getResource(name);
		if (existing)
		{
			// Check type of existing resource
			if (type != existing->getType())
				return 0;
			return existing;
		}
		// Create resource
		ResourceFactory::Ptr factory = getFactory(type);
		if (!factory)
			return 0;
		Resource::Ptr created = factory->create(name);
		return created;
	}
	Resource::Ptr ResourceManager::createResource(const std::string &type,
	                                              const std::string &name)
	{
		std::string resname = name;
		if (resname == "")
			resname = getInternalName();
		// Create resource
		ResourceFactory::Ptr factory = getFactory(type);
		if (!factory)
			return 0;
		Resource::Ptr created = factory->create(resname);
		return created;
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
			log->warning("Duplicated resource name: \"%s\"",
			             res->getName().c_str());
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
