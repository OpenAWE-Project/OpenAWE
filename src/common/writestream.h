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

#ifndef SRC_COMMON_WRITESTREAM_H
#define SRC_COMMON_WRITESTREAM_H

#include <cstddef>

#include "src/common/types.h"
#include "readstream.h"

namespace Common {

/*!
 * \brief Class for writing to a binary stream
 *
 * This class is responsible for writing to a binary. It is an abstract interface, for implementing concrete
 * stream writing to either files or memory
 */
class WriteStream {
public:
	enum SeekOrigin {
		BEGIN,
		CURRENT,
		END
	};

	/*!
	 * Write a byte value to this stream
	 * \param value the value to write
	 */
	void writeByte(byte value);

	/*!
	 * Write the stream to this write stream
	 * \param stream stream to write
	 */
	void writeStream(Common::ReadStream *stream);

	/*!
	 * Write an ascii string to this write stream
	 * \param string the string to write
	 */
	void writeString(std::string string);

	/*!
	 * Write a string using the fmt syntax
	 * \tparam FormatString The type of the format string
	 * \tparam Args The types of the format strings arguments
	 * \param format The format string
	 * \param args The arguments for the format string
	 */
	template<typename FormatString, typename ...Args>
	void writeString(const FormatString &format, const Args &... args) {
		writeString(fmt::format(fmt::runtime(format), args...));
	}

	/*!
	 * Write a uint16 value as little endian to this stream
	 * \param value The value to write
	 */
	void writeUint16LE(uint16_t value);

	/*!
	 * Write a uint16 value as big endian to this stream
	 * \param value The value to write
	 */
	void writeUint16BE(uint16_t value);

	/*!
	 * Write uint32 value as little endian to this stream
	 * \param value The value to write
	 */
	void writeUint32LE(uint32_t value);

	/*!
	 * Write uint32 value as big endian to this stream
	 * \param value The value to write
	 */
	void writeUint32BE(uint32_t value);

	/*!
	 * Write uint64 value as little endian to this stream
	 * \param value The value to write
	 */
	void writeUint64LE(uint64_t value);

	/*!
	 * Write uint64 value as big endian to this stream
	 * \param value The value to write
	 */
	void writeUint64BE(uint64_t value);

	/*!
	 * Write a float value to this stream.
	 * \param value the float
	 */
	void writeIEEEFloatLE(float value);

	/*!
	 * Write a specific value multiple times.
	 * \param value the value to write
	 * \param count how many times the value should be written
	 */
	void writeValues(byte value, size_t count);

	/*!
	 * Write a specified number of zeros.
	 * \param count number of zeros to write
	 */
	void writeZeros(size_t count);

	/*!
	 * Write generic data to this stream
	 * \param data the data to write
	 * \param length the length to write
	 */
	virtual void write(const void *data, size_t length) = 0;

	/*!
	 * Seek the write stream to a specific location
	 * \param length
	 * \param origin
	 */
	virtual void seek(ptrdiff_t length, SeekOrigin origin = BEGIN) = 0;

	/*!
	 * Get the current position in the write stream in bytes
	 * \return The current position in the write stream
	 */
	virtual size_t pos() = 0;
};

} // End of namespace Common

#endif // SRC_COMMON_WRITESTREAM_H
