/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>

#include "src/common/writestream.h"
#include "src/common/endianness.h"

namespace Common {

void WriteStream::writeByte(byte value) {
	write(&value, sizeof(byte));
}

void WriteStream::writeStream(Common::ReadStream *stream) {
	byte buffer[4096];
	while (!stream->eos()) {
		std::memset(buffer, 0, 4096);
		size_t sizeToRead = stream->read(buffer, 4096);
		write(buffer, sizeToRead);
	}
}

void WriteStream::writeString(std::string string) {
	write(string.c_str(), string.size());
}

void WriteStream::writeUint16LE(uint16_t value) {
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN_SYSTEM
	write(&value, sizeof(uint16_t));
}

void WriteStream::writeUint16BE(uint16_t value) {
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // LITTLE_ENDIAN_SYSTEM
	write(&value, sizeof(uint16_t));
}

void WriteStream::writeUint32LE(uint32_t value) {
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN_SYSTEM
	write(&value, sizeof(uint32_t));
}

void WriteStream::writeUint32BE(uint32_t value) {
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN_SYSTEM
	write(&value, sizeof(uint32_t));
}

void WriteStream::writeUint64LE(uint64_t value) {
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN_SYSTEM
	write(&value, sizeof(uint64_t));
}

void WriteStream::writeUint64BE(uint64_t value) {
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN_SYSTEM
	write(&value, sizeof(uint64_t));
}

void WriteStream::writeValues(byte value, size_t count) {
	for (int i = 0; i < count; ++i) {
		write(&value, sizeof(byte));
	}
}

void WriteStream::writeZeros(size_t count) {
	writeValues(0, count);
}

void WriteStream::writeIEEEFloatLE(float value) {
	write(&value, sizeof(float));
}

}
