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
	 * @param stream stream to write
	 */
	void writeStream(Common::ReadStream *stream);

	/*!
	 * Write an ascii string to this write stream
	 * @param string the string to write
	 */
	void writeString(std::string string);

	void writeUint16LE(uint16_t value);

	void writeUint32LE(uint32_t value);

	void writeUint32BE(uint32_t value);

	void writeUint64LE(uint64_t value);

	void writeUint64BE(uint64_t value);

	/*!
	 * Write a float value to this stream.
	 * @param value the float
	 */
	void writeIEEEFloatLE(float value);

	/*!
	 * Write a specific value multiple times.
	 * @param value the value to write
	 * @param count how many times the value should be written
	 */
	void writeValues(byte value, size_t count);

	/*!
	 * Write a specified number of zeros.
	 * @param count number of zeros to write
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

	virtual size_t pos() = 0;
};

} // End of namespace Common

#endif // SRC_COMMON_WRITESTREAM_H
