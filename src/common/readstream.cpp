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

#include <iostream>
#include <regex>

#include "src/common/endianness.h"
#include "src/common/readstream.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/bit_cast.h"

namespace Common {

ReadStream::~ReadStream() = default;

byte ReadStream::readByte() {
	byte value;
	read(&value, sizeof(byte));
	return value;
}

uint64_t ReadStream::readUint64LE() {
	uint64_t value;
	read(&value, sizeof(uint64_t));
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN
	return value;
}

uint64_t ReadStream::readUint64BE() {
	uint64_t value;
	read(&value, sizeof(uint64_t));
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // LITTLE_ENDIAN
	return value;
}

int64_t ReadStream::readSint64LE() {
	return Common::bit_cast<int64_t>(readUint64LE());
}

int64_t ReadStream::readSint64BE() {
	return Common::bit_cast<int64_t>(readUint64BE());
}

uint32_t ReadStream::readUint32LE() {
	uint32_t value;
	read(&value, sizeof(uint32_t));
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN
	return value;
}

uint32_t ReadStream::readUint32BE() {
	uint32_t value;
	read(&value, sizeof(uint32_t));
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // LITTLE_ENDIAN
	return value;
}

int32_t ReadStream::readSint32LE() {
	return Common::bit_cast<int32_t>(readUint32LE());
}

int32_t ReadStream::readSint32BE() {
	return Common::bit_cast<int32_t>(readUint32BE());
}

uint16_t ReadStream::readUint16LE() {
	uint16_t value;
	read(&value, sizeof(uint16_t));
#ifdef BIG_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // BIG_ENDIAN
	return value;
}

uint16_t ReadStream::readUint16BE() {
	uint16_t value;
	read(&value, sizeof(uint16_t));
#ifdef LITTLE_ENDIAN_SYSTEM
	value = swapBytes(value);
#endif // LITTLE_ENDIAN
	return value;
}

int16_t ReadStream::readSint16LE() {
	return Common::bit_cast<int16_t>(readUint16LE());
}

int16_t ReadStream::readSint16BE() {
	return Common::bit_cast<int16_t>(readUint16BE());
}

float ReadStream::readIEEEFloatLE() {
	float value;
	read(&value, sizeof(float));
	return value;
}

std::string ReadStream::readFixedSizeString(size_t length, bool nullTerminated) {
	std::vector<char> data(length);
	read(data.data(), length);
	if (nullTerminated) {
		return std::string(data.data());
	}
	return std::string(data.data(), length);
}

std::u16string ReadStream::readFixedSizeStringUTF16(size_t length) {
	std::vector<char16_t> data(length);
	read(reinterpret_cast<char *>(data.data()), length * sizeof(char16_t));
	return std::u16string(data.data(), data.size());
}

std::string ReadStream::readNullTerminatedString() {
	std::stringstream ss;
	char c = static_cast<char>(readByte());
	while (c != '\0') {
		ss << c;
		if (eos())
			break;
		c = static_cast<char>(readByte());
	}
	return ss.str();
}

std::string ReadStream::readNullTerminatedString(size_t stepSize) {
	std::stringstream ss;
	std::string s = readFixedSizeString(stepSize);
	while (!std::regex_match(s, std::regex(".*\\0$"))) {
		ss << s;
		s = readFixedSizeString(stepSize);
	}

	ss << s;

	return ss.str();
}

std::string ReadStream::readLine(char delimiter) {
	std::stringstream ss;

	char data;
	do {
		read(&data, sizeof(char));
		if (data != delimiter)
			ss << data;
	} while (data != delimiter && !eos());

	return ss.str();
}

Common::ReadStream *ReadStream::readStream(size_t length) {
	Common::DynamicMemoryWriteStream stream(false);
	while (length > 0 && !eos()) {
		byte partData[4096];
		size_t readSize = read(partData, std::min<size_t>(4096, length));
		length -= readSize;
		stream.write(partData, readSize);
	}
	return new Common::MemoryReadStream(stream.getData(), stream.getLength());
}

void ReadStream::skip(ptrdiff_t offset) {
	seek(offset, CURRENT);
}

} // End of namespace Common
