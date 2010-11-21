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

#include "CoreRender/render/UploadManager.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/render/RenderObject.hpp"

namespace cr
{
namespace render
{
	UploadManager::UploadManager()
	{
	}
	UploadManager::~UploadManager()
	{
	}

	void UploadManager::registerUpload(RenderResource *resource)
	{
		tbb::spin_mutex::scoped_lock lock(listmutex);
		resupload.push_back(resource);
	}
	void UploadManager::registerDeletion(RenderResource *resource)
	{
		tbb::spin_mutex::scoped_lock lock(listmutex);
		resdeletion.push_back(resource);
	}
	void UploadManager::registerUpload(RenderObject *object)
	{
		tbb::spin_mutex::scoped_lock lock(listmutex);
		objupload.push_back(object);
	}
	void UploadManager::registerDeletion(RenderObject *object)
	{
		tbb::spin_mutex::scoped_lock lock(listmutex);
		objdeletion.push_back(object);
	}

	void UploadManager::getLists(UploadLists &lists, core::MemoryPool *memory)
	{
		{
			tbb::spin_mutex::scoped_lock lock(listmutex);
			// Get resource upload list
			unsigned int memsize = sizeof(UploadLists::ResourceUploadEntry) * resupload.size();
			lists.resupload = (UploadLists::ResourceUploadEntry*)memory->allocate(memsize);
			for (unsigned int i = 0; i < resupload.size(); i++)
			{
				lists.resupload[i].resource = resupload[i];
			}
			lists.resuploadcount = resupload.size();
			// Get resource deletion list
			memsize = sizeof(RenderResource*) * resdeletion.size();
			lists.resdeletion = (RenderResource**)memory->allocate(memsize);
			for (unsigned int i = 0; i < resdeletion.size(); i++)
			{
				lists.resdeletion[i] = resdeletion[i];
			}
			lists.resdeletioncount = resdeletion.size();
			// Get object upload list
			memsize = sizeof(UploadLists::ObjectUploadEntry) * objupload.size();
			lists.objupload = (UploadLists::ObjectUploadEntry*)memory->allocate(memsize);
			for (unsigned int i = 0; i < objupload.size(); i++)
			{
				lists.objupload[i].object = objupload[i];
			}
			lists.objuploadcount = objupload.size();
			// Get object deletion list
			memsize = sizeof(RenderObject*) * objdeletion.size();
			lists.objdeletion = (RenderObject**)memory->allocate(memsize);
			for (unsigned int i = 0; i < objdeletion.size(); i++)
			{
				lists.objdeletion[i] = objdeletion[i];
			}
			lists.objdeletioncount = objdeletion.size();
			// Reset lists
			resupload.clear();
			resdeletion.clear();
			objupload.clear();
			objdeletion.clear();
		}
		// We retrieve the upload data here to prevent deadlocks as the
		// resource might be locked while it calls registerUpload().
		for (unsigned int i = 0; i < lists.resuploadcount; i++)
			lists.resupload[i].data = lists.resupload[i].resource->prepareForUpload();
		for (unsigned int i = 0; i < lists.objuploadcount; i++)
			lists.objupload[i].data = lists.objupload[i].object->prepareForUpload();
	}
	void UploadManager::uploadResources(UploadLists &lists)
	{
		for (unsigned int i = 0; i < lists.resuploadcount; i++)
		{
			lists.resupload[i].resource->upload(lists.resupload[i].data);
		}
		for (unsigned int i = 0; i < lists.objuploadcount; i++)
		{
			lists.objupload[i].object->upload(lists.objupload[i].data);
		}
	}
	void UploadManager::deleteResources(UploadLists &lists)
	{
		for (unsigned int i = 0; i < lists.objdeletioncount; i++)
		{
			delete lists.objdeletion[i];
		}
		for (unsigned int i = 0; i < lists.resdeletioncount; i++)
		{
			delete lists.resdeletion[i];
		}
	}
}
}
