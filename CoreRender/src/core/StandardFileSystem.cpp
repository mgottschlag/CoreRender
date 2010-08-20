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

#include "CoreRender/core/StandardFileSystem.hpp"
#include "CoreRender/core/StandardFile.hpp"
#include "CoreRender/core/Platform.hpp"

#include <iostream>
#include <cstring>

#if defined(CORERENDER_UNIX)
	#include <dirent.h>
#elif defined(CORERENDER_WINDOWS)
	#include <Windows.h>
#endif

namespace cr
{
namespace core
{
	StandardFileSystem::StandardFileSystem()
	{
	}
	StandardFileSystem::~StandardFileSystem()
	{
	}

	bool StandardFileSystem::mount(const std::string &src,
	                               const std::string &dest,
	                               unsigned int mode)
	{
		tbb::mutex::scoped_lock lock(mutex);
		// TODO: Check dest
		// Check mode
		if (mode & ~(FileAccess::Read | FileAccess::Write))
			return false;
		// TODO: Merge mappings if src and dest are identical
		Mapping mapping;
		mapping.src = src;
		mapping.dest = dest;
		mapping.mode = mode;
		mountinfo.push_back(mapping);
		return true;
	}
	bool StandardFileSystem::unmount(const std::string &path)
	{
		tbb::mutex::scoped_lock lock(mutex);
		bool found = false;
		for (unsigned int i = 0; i < mountinfo.size(); ++i)
		{
			if (mountinfo[i].dest == path)
			{
				mountinfo.erase(mountinfo.begin() + i);
				--i;
				found = true;
			}
		}
		return found;
	}

	File *StandardFileSystem::open(const std::string &path, unsigned int mode, bool create)
	{
		if (path == "")
			return 0;
		tbb::mutex::scoped_lock lock(mutex);
		// Look for the newest mount point that contains the file
		unsigned int modecheck = mode;
		modecheck &= FileAccess::Read | FileAccess::Write;
		// TODO: Properly handle create
		for (unsigned int i = 0; i < mountinfo.size(); ++i)
		{
			if ((mountinfo[i].dest == path.substr(0, mountinfo[i].dest.size()))
				&& ((mountinfo[i].mode & modecheck) == modecheck))
			{
				std::string abspath = mountinfo[i].src + path.substr(mountinfo[i].dest.size());
				std::cout << path << " mapped to " << abspath << std::endl;
				// Try to open the file
				StandardFile *file = new StandardFile(path, abspath, mode);
				if (!file->isOpen())
					delete file;
				else
					return file;
			}
		}
		return 0;
	}
	std::string StandardFileSystem::getPath(const std::string &path, const std::string &currentdir)
	{
		if (path[0] == '/' || path[0] == '\\')
			return path;
		return currentdir + "/" + path;
	}
	FileList *StandardFileSystem::listDirectory(const std::string &directory)
	{
		std::vector<FileList::Entry> entries;
		// List files from all directories which exist under the given name in
		// the mount point list
		// TODO: Handle the case where a single source directory is in the list
		// twice
		tbb::mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < mountinfo.size(); ++i)
		{
			if (mountinfo[i].dest == directory.substr(0, mountinfo[i].dest.size()))
			{
				std::string abspath = mountinfo[i].src + directory.substr(mountinfo[i].dest.size());
#ifdef CORERENDER_UNIX
				DIR *dir = opendir(abspath.c_str());
				if (!dir)
					continue;
				struct dirent *direntry;
				while ((direntry = readdir(dir)) != 0)
				{
					if (!strcmp(direntry->d_name, ".")
						|| !strcmp(direntry->d_name, ".."))
						continue;
					FileList::Entry entry;
					entry.name = direntry->d_name;
					entry.path = directory + "/" + entry.name;
					if (direntry->d_type == DT_DIR)
						entry.directory = true;
					else
						entry.directory = true;
					entries.push_back(entry);
				}
				closedir(dir);
#elif defined(CORERENDER_WINDOWS)
				WIN32_FIND_DATA finddata;
				HANDLE find = FindFirstFile((LPCSTR)(abspath + "\\*").c_str(), &finddata);
				if (find == INVALID_HANDLE_VALUE)
					continue;
				do
				{
					FileList::Entry entry;
					entry.name = finddata.cFileName;
					entry.path = directory + "/" + finddata.cFileName;
					if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						entry.directory = true;
					else
						entry.directory = true;
					entries.push_back(entry);
				} while (FindNextFile(find, &finddata) != 0);
				FindClose(find);
#else
	#error Unimplemented
#endif
			}
		}
		FileList *list = new FileList(&entries[0], entries.size());
		return list;
	}

	bool StandardFileSystem::isFile(const std::string &path)
	{
		// Just try to open the file
		File *file = open(path, FileAccess::Read, false);
		if (!file)
			return false;
		delete file;
		return true;
	}
	bool StandardFileSystem::isDirectory(const std::string &path)
	{
		tbb::mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < mountinfo.size(); ++i)
		{
			if (mountinfo[i].dest == path.substr(0, mountinfo[i].dest.size()))
			{
				std::string abspath = mountinfo[i].src + path.substr(mountinfo[i].dest.size());
#if defined(CORERENDER_UNIX)
				DIR *dir = opendir(abspath.c_str());
				if (dir)
				{
					closedir(dir);
					return true;
				}
#elif defined(CORERENDER_WINDOWS)
				DWORD attribs = GetFileAttributes(abspath.c_str());
				if (attribs != INVALID_FILE_ATTRIBUTES
				    && (attribs & FILE_ATTRIBUTE_DIRECTORY))
					return true;
#else
				#error Unimplemented
#endif
			}
		}
		return false;
	}
}
}
