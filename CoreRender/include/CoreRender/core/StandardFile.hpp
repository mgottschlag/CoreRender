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

#ifndef _CORERENDER_CORE_STANDARDFILE_HPP_INCLUDED_
#define _CORERENDER_CORE_STANDARDFILE_HPP_INCLUDED_

#include "File.hpp"

#include <cstdio>
#include <tbb/spin_mutex.h>

namespace cr
{
namespace core
{
	class StandardFile : public File
	{
		public:
			StandardFile(const std::string &path,
			             const std::string &abspath,
			             unsigned int mode);
			virtual ~StandardFile();

			bool isOpen();

			virtual std::string getPath();
			virtual unsigned int getMode();

			virtual int read(int size, void *data);
			virtual int write(int size, const void *data);

			virtual bool write(const std::string &str);
			virtual std::string readLine();
			virtual std::string readAll();

			virtual unsigned int getSize();
			virtual unsigned int seek(int pos, bool relative = false);
			virtual unsigned int getPosition();
			virtual bool eof();
			virtual bool error();

			virtual void flush();
		private:
			FILE *file;
			tbb::spin_mutex pathmutex;
			std::string path;
			unsigned int mode;
			unsigned int size;
	};
}
}

#endif
