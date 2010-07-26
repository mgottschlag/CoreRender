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

#ifndef _CORERENDER_CORE_LOG_HPP_INCLUDED_
#define _CORERENDER_CORE_LOG_HPP_INCLUDED_

#include "ReferenceCounted.hpp"
#include "FileSystem.hpp"

#include <tbb/mutex.h>
#include <cstdarg>

#define CR_FILE_LINE __FILE__ CORE_RENDER_STRINGIFY(__LINE__)
#define CORE_RENDER_STRINGIFY(number) CORE_RENDER_STRINGIFY2(number)
#define CORE_RENDER_STRINGIFY2(number) #number

namespace cr
{
namespace core
{
	struct LogLevel
	{
		enum List
		{
			Nothing = 4,
			Error = 3,
			Warning = 2,
			Information = 1,
			Debug = 0
		};
	};
	class Log : public ReferenceCounted
	{
		public:
			Log(FileSystem::Ptr fs, const std::string &filename);
			~Log();

			void setConsoleLevel(LogLevel::List level);
			LogLevel::List getConsoleLevel();
			void setFileLevel(LogLevel::List level);
			LogLevel::List getFileLevel();

			void error(const char *format, ...);
			void warning(const char *format, ...);
			void info(const char *format, ...);
			void debug(const char *format, ...);

			void write(LogLevel::List level, const char *format, ...);

			typedef SharedPointer<Log> Ptr;
		private:
			void write(LogLevel::List level,
			           const char *format,
			           std::va_list args);

			LogLevel::List consolelevel;
			LogLevel::List filelevel;

			uint64_t starttime;

			File::Ptr file;

			tbb::mutex mutex;
	};
}
}

#endif
