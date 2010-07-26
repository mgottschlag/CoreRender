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

#include "CoreRender/core/StandardFile.hpp"

#include <cstring>

#ifdef __unix__
#include <sys/stat.h>
#else
#error Unimplemented.
#endif

namespace cr
{
namespace core
{
	StandardFile::StandardFile(const std::string &path,
	                           const std::string &abspath,
	                           unsigned int mode)
		: file(0), path(path), mode(mode)
	{
		// Open file
		std::string modestring;
		if ((mode & FileAccess::Read) && (mode & FileAccess::Write))
			modestring = "r+";
		else if (mode & FileAccess::Read)
			modestring = "r";
		else if (mode & FileAccess::Write)
			modestring = "w";
		else
			return;
		if (mode & FileAccess::Text)
			modestring += "t";
		file = fopen(abspath.c_str(), modestring.c_str());
		if (!file)
			return;
		// Get size
		if ((mode & FileAccess::Text) == 0)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);
		}
		else
		{
			#ifdef __unix__
			int fd = fileno(file);
			struct stat buf;
			fstat(fd, &buf);
			size = buf.st_size;
			#else
			#error Unimplemented.
			// TODO
			#endif
		}
	}
	StandardFile::~StandardFile()
	{
		if (file)
			fclose(file);
	}

	bool StandardFile::isOpen()
	{
		return file != 0;
	}

	std::string StandardFile::getPath()
	{
		tbb::spin_mutex::scoped_lock lock(pathmutex);
		return path;
	}
	unsigned int StandardFile::getMode()
	{
		return mode;
	}

	int StandardFile::read(int size, void *data)
	{
		return fread(data, 1, size, file);
	}
	int StandardFile::write(int size, const void *data)
	{
		return fwrite(data, 1, size, file);
	}

	bool StandardFile::write(const std::string &str)
	{
		return write(str.size(), str.c_str()) == (int)str.size();
	}
	std::string StandardFile::readLine()
	{
		std::string s;
		char buffer[256];
		while (fgets(buffer, 256, file))
		{
			unsigned int length = strlen(buffer);
			bool endreached = false;
			if (buffer[strlen(buffer) - 1] == '\n')
			{
				buffer[strlen(buffer) - 1] = 0;
				endreached = true;
			}
			else if (length != 255)
				endreached = true;
			s += buffer;
			if (endreached)
				break;
		}
		return s;
	}
	std::string StandardFile::readAll()
	{
		unsigned int position = getPosition();
		seek(0);
		char *buffer = new char[getSize() + 1];
		unsigned int bytesread = read(getSize(), buffer);
		seek(position);
		buffer[bytesread] = 0;
		std::string s = buffer;
		delete[] buffer;
		return s;
	}

	unsigned int StandardFile::getSize()
	{
		return size;
	}
	unsigned int StandardFile::seek(int pos, bool relative)
	{
		if (relative)
			fseek(file, pos, SEEK_CUR);
		else
			fseek(file, pos, SEEK_SET);
		return ftell(file);
	}
	unsigned int StandardFile::getPosition()
	{
		return ftell(file);
	}
	bool StandardFile::eof()
	{
		return feof(file);
	}
	bool StandardFile::error()
	{
		return ferror(file);
	}

	void StandardFile::flush()
	{
		fflush(file);
	}
}
}
