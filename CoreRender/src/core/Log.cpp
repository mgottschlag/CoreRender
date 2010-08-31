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

#include "CoreRender/core/Log.hpp"
#include "CoreRender/core/Time.hpp"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <iomanip>

namespace cr
{
namespace core
{
	Log::Log(FileSystem::Ptr fs, const std::string &filename)
		: consolelevel(LogLevel::Warning), filelevel(LogLevel::Information)
	{
		// Open file
		file = fs->open(filename, FileAccess::Write, true);
		if (!file)
		{
			std::cout << "Could not open log file." << std::endl;
			return;
		}
		// Get log start time
		starttime = Time::Now();
		// Write file header
		file->write("<html>\n<header>\n<title>CoreRender Log</title>\n");
		file->write("<style type=\"text/css\">\ntable {\n");
		file->write("border: thin solid lightgrey;");
		file->write("border-spacing: 0px;");
		file->write("}\ntd {\n");
		file->write("border: 0px;");
		file->write("}\n.error {\n");
		file->write("background-color: #FFDDDD;");
		file->write("}\n.warning {\n");
		file->write("background-color: #FFFF80;");
		file->write("}\n.debug {\n");
		file->write("background-color: #FFFFFF;");
		file->write("}\n.info {\n");
		file->write("background-color: #FFFFFF;");
		file->write("}\n</style>\n</header>\n");
		file->write("<body>\n");
		file->write("<h1>CoreRender</h1>");
		file->write("<table border=\"1\" width=\"100%\">\n");
		file->write("<tr><td>Time</td><td>Message</td></tr>\n");
		info("Log started on %s.", AbsoluteTime::Now().toString().c_str());
	}
	Log::~Log()
	{
		// Write file footer
		if (file)
		{
			file->write("</table>\n");
			file->write("</body></html>\n");
		}
	}
	
	void Log::setConsoleLevel(LogLevel::List level)
	{
		consolelevel = level;
	}
	LogLevel::List Log::getConsoleLevel()
	{
		return consolelevel;
	}
	void Log::setFileLevel(LogLevel::List level)
	{
		filelevel = level;
	}
	LogLevel::List Log::getFileLevel()
	{
		return filelevel;
	}
	
	void Log::error(const char *format, ...)
	{
		std::va_list args;
		va_start(args, format);
		write(LogLevel::Error, format, args);
		va_end(args);
	}
	void Log::warning(const char *format, ...)
	{
		std::va_list args;
		va_start(args, format);
		write(LogLevel::Warning, format, args);
		va_end(args);
	}
	void Log::info(const char *format, ...)
	{
		std::va_list args;
		va_start(args, format);
		write(LogLevel::Information, format, args);
		va_end(args);
	}
	void Log::debug(const char *format, ...)
	{
		std::va_list args;
		va_start(args, format);
		write(LogLevel::Debug, format, args);
		va_end(args);
	}
	
	void Log::write(LogLevel::List level, const char *format, ...)
	{
		std::va_list args;
		va_start(args, format);
		write(level, format, args);
		va_end(args);
	}
	void Log::write(LogLevel::List level,
	                const char *format,
	                std::va_list args)
	{
		// Construct message
		char msg[1024];
		vsnprintf(msg, 1024, format, args);
		// Get time
		Time currenttime = Time::Now();
		float seconds = 0.001f * (currenttime - starttime).getMilliseconds();
		// Write to console/file
		tbb::mutex::scoped_lock lock(mutex);
		if (level >= consolelevel)
		{
			std::cout << msg << std::endl;
		}
		if (level >= filelevel)
		{
			std::ostringstream stream;
			stream.precision(3);
			std::string cssclass;
			switch (level)
			{
				case LogLevel::Error:
					cssclass = "error";
					break;
				case LogLevel::Warning:
					cssclass = "warning";
					break;
				case LogLevel::Information:
					cssclass = "info";
					break;
				case LogLevel::Debug:
				default:
					cssclass = "debug";
					break;
					
			}
			stream << "<tr><td class=\"" << cssclass << "\">"
				<< std::fixed << seconds << "</td><td class=\""
				<< cssclass << "\">";
			file->write(stream.str());
			file->write(msg);
			file->write("</td></tr>\n");
		}
	}
}
}
