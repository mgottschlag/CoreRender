/*
Copyright (C) 2009, Mathias Gottschlag

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

#ifndef _CORERENDER_CORE_BUFFER_HPP_INCLUDED_
#define _CORERENDER_CORE_BUFFER_HPP_INCLUDED_

#include "ReferenceCounted.hpp"
#include "../math/StdInt.hpp"

#include <string>

namespace cr
{
namespace core
{
	/**
	 * Dynamically sized buffer for network data etc.
	 *
	 * Data is written/read to/from the buffer via read*() and write*(). All
	 * read/write operations start at the current read/write position which
	 * is 0 if the buffer has just been created. If multiple bytes are written
	 * or read, they are converted to/from network byte order if necessary. The
	 * buffer uses bit-packing, so integers with arbitrary size can be written
	 * and read without wasting any space. Note that reads/writes at byte
	 * boundaries are faster than reads/writes starting in the middle of a byte.
	 * nextByte() can be used to align the current position in order to
	 * minimize CPU usage, although this usually comes with a larger buffer
	 * size.
	 */
	class Buffer : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Buffer();
			/**
			 * Constructor from preinitialized data.
			 * @param data Pointer to the new buffer content.
			 * @param size Length of the data to be used for the buffer.
			 * @param copy If set to true, a copy of the data is created, else
			 * the preallocated memory is used and deleted if the buffer is
			 * destroyed.
			 */
			Buffer(void *data, unsigned int size, bool copy = false);
			/**
			 * Copy constructor.
			 */
			Buffer(const Buffer &b);
			/**
			 * Destructor. Frees the memory connected to the buffer.
			 */
			~Buffer();

			/**
			 * Sets the size of the buffer in bytes. The read/write position is
			 * not changed if it is still valid.
			 */
			void setSize(unsigned int size);
			/**
			 * Returns the size of the buffer.
			 */
			unsigned int getSize();

			/**
			 * Sets the read/write position in bits.
			 */
			void setPosition(unsigned int position);
			/**
			 * Returns the read/write position.
			 */
			unsigned int getPosition();

			/**
			 * Returns a pointer to the buffer data. The pointer is only valid
			 * as long as the buffer remains unchanged.
			 */
			void *getData();

			/**
			 * Writes one byte to the buffer.
			 */
			void write8(uint8_t value);
			/**
			 * Reads one byte from the buffer.
			 */
			uint8_t read8();
			/**
			 * Writes two bytes to the buffer.
			 */
			void write16(uint16_t value);
			/**
			 * Reads two bytes from the buffer.
			 */
			uint16_t read16();
			/**
			 * Writes four bytes to the buffer.
			 */
			void write32(uint32_t value);
			/**
			 * Reads four bytes from the buffer.
			 */
			uint32_t read32();
			/**
			 * Writes eight bytes to the buffer.
			 */
			void write64(uint64_t value);
			/**
			 * Reads eight bytes from the buffer.
			 */
			uint64_t read64();

			/**
			 * Writes a float to the buffer.
			 */
			void writeFloat(float value);
			/**
			 * Reads a float from the buffer.
			 */
			float readFloat();

			/**
			 * Writes a string to the buffer.
			 */
			void writeString(std::string value);
			/**
			 * Reads a string from the buffer.
			 */
			std::string readString();

			/**
			 * Writes raw data to the buffer.
			 */
			void write(void *data, unsigned int size);
			/**
			 * Reads raw data from the buffer.
			 */
			unsigned int read(void *data, unsigned int size);

			/**
			 * Writes an integer with an arbitrary size to the buffer. Only
			 * sizes from 1 to 32 are supported.
			 */
			void writeInt(int value, unsigned int size);
			/**
			 * Reads an integer with an arbitrary size from the buffer.
			 */
			int readInt(unsigned int size);
			/**
			 * Writes a unsigned integer with an arbitrary size to the buffer.
			 */
			void writeUnsignedInt(unsigned int value, unsigned int size);
			/**
			 * Reads a unsigned integer with an arbitrary size from the buffer.
			 * Only sizes from 1 to 32 are supported.
			 */
			unsigned int readUnsignedInt(unsigned int size);

			/**
			 * Rounds the read/write position up to the next byte. This is
			 * useful to reduce CPU load if there is no gain from using bit
			 * packing.
			 */
			void nextByte();

			/**
			 * Creates a copy of buf.
			 */
			Buffer &operator=(const Buffer &buf);
			/**
			 * Appends one buffer to the other.
			 */
			Buffer &operator+=(const Buffer &buf);

			/**
			 * Dumps the buffer to stdout.
			 */
			void dump();

			typedef SharedPointer<Buffer> Ptr;
		private:
			/**
			 * Writes one byte to the buffer, does not change the current
			 * position.
			 */
			void writeByte(unsigned char value);
			/**
			 * Reads one byte from the buffer, does not change the current
			 * position.
			 */
			unsigned char readByte();

			/**
			 * Buffer data.
			 */
			char *data;
			/**
			 * Buffer size in bytes.
			 */
			unsigned int size;
			/**
			 * Read/write position in bits.
			 */
			unsigned int position;
	};
}
}

#endif
