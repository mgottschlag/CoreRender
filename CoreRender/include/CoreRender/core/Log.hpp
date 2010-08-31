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
#include "Time.hpp"

#include <tbb/mutex.h>
#include <cstdarg>

#define CR_FILE_LINE __FILE__ CORE_RENDER_STRINGIFY(__LINE__)
#define CORE_RENDER_STRINGIFY(number) CORE_RENDER_STRINGIFY2(number)
#define CORE_RENDER_STRINGIFY2(number) #number

namespace cr
{
namespace core
{
	/**
	 * Severity level of log messages.
	 */
	struct LogLevel
	{
		enum List
		{
			/**
			 * No log level, but rather a value to prevent everything from
			 * being written to the log in Log::setConsoleLevel() and
			 * Log::setFileLevel().
			 */
			Nothing = 4,
			/**
			 * Error messages.
			 */
			Error = 3,
			/**
			 * Warning messages.
			 */
			Warning = 2,
			/**
			 * General information.
			 */
			Information = 1,
			/**
			 * Debug information.
			 */
			Debug = 0
		};
	};
	/**
	 * Class to log to a file or to the console.
	 */
	class Log : public ReferenceCounted
	{
		public:
			/**
			 * Constructor. Opens a file for logging and if that fails only logs
			 * to the console.
			 * @param fs File system to be used for the log file.
			 * @param filename File name of the log file.
			 */
			Log(FileSystem::Ptr fs, const std::string &filename);
			/**
			 * Destructor.
			 */
			~Log();

			/**
			 * Sets the minimum log level for the console. Only lines with a
			 * higher or equal severity than this are printed out.
			 *
			 * The default is LogLevel::Warning.
			 * @param level New minimum log level.
			 */
			void setConsoleLevel(LogLevel::List level);
			/**
			 * Returns the minimum log level for the console.
			 * @return Minimum log level.
			 */
			LogLevel::List getConsoleLevel();
			/**
			 * Sets the minimum log level for the log file. Only lines with a
			 * higher or equal severity than this are printed out.
			 *
			 * The default is LogLevel::Information.
			 * @param level New minimum log level.
			 */
			void setFileLevel(LogLevel::List level);
			/**
			 * Returns the minimum log level for the log file.
			 * @return Minimum log level.
			 */
			LogLevel::List getFileLevel();

			/**
			 * Prints a line with the log level LogLevel::Error.
			 * @param format Format string which uses the same formatting as
			 * printf().
			 */
			void error(const char *format, ...);
			/**
			 * Prints a line with the log level LogLevel::Warning.
			 * @param format Format string which uses the same formatting as
			 * printf().
			 */
			void warning(const char *format, ...);
			/**
			 * Prints a line with the log level LogLevel::Information.
			 * @param format Format string which uses the same formatting as
			 * printf().
			 */
			void info(const char *format, ...);
			/**
			 * Prints a line with the log level LogLevel::Debug.
			 * @param format Format string which uses the same formatting as
			 * printf().
			 */
			void debug(const char *format, ...);

			/**
			 * Prints a line with a certain log level.
			 * @param level Log level of the line.
			 * @param format Format string which uses the same formatting as
			 * printf().
			 */
			void write(LogLevel::List level, const char *format, ...);

			typedef SharedPointer<Log> Ptr;
		private:
			void write(LogLevel::List level,
			           const char *format,
			           std::va_list args);

			LogLevel::List consolelevel;
			LogLevel::List filelevel;

			Time starttime;

			File::Ptr file;

			tbb::mutex mutex;
	};
}
}

#endif
