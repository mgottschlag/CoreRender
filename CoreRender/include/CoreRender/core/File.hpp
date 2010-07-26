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

#ifndef _CORERENDER_CORE_FILE_HPP_INCLUDED_
#define _CORERENDER_CORE_FILE_HPP_INCLUDED_

#include "ReferenceCounted.hpp"

#include <string>

namespace cr
{
namespace core
{
	struct FileAccess
	{
		enum List
		{
			Read = 0x1,
			Write = 0x2,
			Text = 0x4
		};
	};

	/**
	 * @note This class is not thread-safe, never use one file from more than
	 * one thread.
	 */
	class File : public ReferenceCounted
	{
		public:
			virtual ~File()
			{
			}

			virtual std::string getPath() = 0;
			virtual unsigned int getMode() = 0;

			virtual int read(int size, void *data) = 0;
			virtual int write(int size, const void *data) = 0;

			virtual bool write(const std::string &str) = 0;
			virtual std::string readLine() = 0;
			virtual std::string readAll() = 0;

			virtual unsigned int getSize() = 0;
			virtual unsigned int seek(int pos, bool relative = false) = 0;
			virtual unsigned int getPosition() = 0;
			virtual bool eof() = 0;
			virtual bool error() = 0;

			virtual void flush() = 0;

			typedef SharedPointer<File> Ptr;
		private:
	};
}
}

#endif
