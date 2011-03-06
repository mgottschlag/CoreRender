/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CORERENDER_ROCKET_FILEINTERFACE_HPP_INCLUDED_
#define _CORERENDER_ROCKET_FILEINTERFACE_HPP_INCLUDED_

#include <Rocket/Core.h>
#include "CoreRender/core/FileSystem.hpp"

namespace cr
{
namespace rocket
{
	class FileInterface : public Rocket::Core::FileInterface
	{
		public:
			FileInterface(core::FileSystem::Ptr fs)
				: fs(fs)
			{
			}

			virtual Rocket::Core::FileHandle Open(const Rocket::Core::String& path)
			{
				core::File::Ptr file = fs->open(path.CString(),
				                                core::FileAccess::Read);
				if (!file)
					return 0;
				file->grab();
				return (uintptr_t)file.get();
			}
			virtual void Close(Rocket::Core::FileHandle filehandle)
			{
				core::File *file = (core::File*)filehandle;
				file->drop();
			}
			virtual size_t Read(void *buffer, size_t size, Rocket::Core::FileHandle filehandle)
			{
				core::File *file = (core::File*)filehandle;
				return file->read(size, buffer);
			}
			virtual bool Seek(Rocket::Core::FileHandle filehandle, long offset, int origin)
			{
				core::File *file = (core::File*)filehandle;
				switch (origin)
				{
					case SEEK_SET:
						return file->seek(offset, false) == offset;
					case SEEK_CUR:
					{
						unsigned int position = file->getPosition();
						return file->seek(offset, true) == position + offset;
					}
					case SEEK_END:
					{
						int size = file->getSize();
						if (offset > size)
							return false;
						return file->seek(size - offset, false) == size - offset;
					}
					default:
						return false;
				}
			}
			virtual size_t Tell(Rocket::Core::FileHandle filehandle)
			{
				core::File *file = (core::File*)filehandle;
				return file->getPosition();
			}
		private:
			core::FileSystem::Ptr fs;
	};
}
}

#endif
