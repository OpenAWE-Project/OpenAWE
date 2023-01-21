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

#ifndef SRC_COMMON_READSTREAMS_H
#define SRC_COMMON_READSTREAMS_H

#include <cstdint>
#include <cstddef>

#include <string>

#include "src/common/types.h"

namespace Common {

class ReadStream {
public:
	enum SeekOrigin {
		BEGIN,
		CURRENT,
		END
	};

	virtual ~ReadStream();

	/*!
	 * Read the next byte from the read stream
	 * \return the next byte
	 */
	byte readByte();

	/*!
	 * Read a 64 bit little endian unsigned int
	 * \return the 64bit value
	 */
	uint64_t readUint64LE();

	/*!
	 * Read a 64 bit big endian unsigned int
	 * \return the 64bit value
	 */
	uint64_t readUint64BE();

	/*!
	 * Read a 64 bit little endian signed int
	 * \return the 64bit value
	 */
	int64_t readSint64LE();

	/*!
	 * Read a 64 bit big endian signed int
	 * \return the 64bit value
	 */
	int64_t readSint64BE();

	/*!
	 * Read a 32 bit little endian unsigned int.
	 * \return the 32 bit value
	 */
	uint32_t readUint32LE();

	/*!
	 * Read a 32 bit big endian unsigned int.
	 * \return the 32 bit value
	 */
	uint32_t readUint32BE();

	/*!
	 * Read a 32 bit little endian signed int.
	 * \return the 32 bit value
	 */
	int32_t readSint32LE();

	/*!
	 * Read a 32 bit big endian signed int.
	 * \return the 32 bit value
	 */
	int32_t readSint32BE();

	/*!
	 * Read the next 16 bit unsigned integer as little
	 * endian value
	 * \return the 16 bit value
	 */
	uint16_t readUint16LE();

	/*!
	 * Read the next 16 bit unsigned integer as big
	 * endian value
	 * \return the 16 bit value
	 */
	uint16_t readUint16BE();

	/*!
	 * Read the next 16 bit signed integer as little
	 * endian value
	 * \return the 16 bit value
	 */
	int16_t readSint16LE();

	/*!
	 * Read the next 16 bit signed integer as big
	 * endian value
	 * \return the 16 bit value
	 */
	int16_t readSint16BE();

	/*!
	 * Read a 32 bit little endian floating point value
	 * \return the float value
	 */
	float readIEEEFloatLE();

	/*!
	 * Read a simple fixed length ASCII string
	 * \param length the maximum length of the string
	 * \param nullTerminated if the string is null terminated
	 * \return the read string
	 */
	 std::string readFixedSizeString(size_t length, bool nullTerminated = false);

	/*!
	* Read a fixed length UTF-16 string.
	* \param length the maximum length of the string
	* \param nullTerminated if the string is null terminated
	* \return the read utf16 string
	*/
	std::u16string readFixedSizeStringUTF16(size_t length);

	 /*!
	  * Read a null terminated ASCII string
	  * \return the read string
	  */
	 std::string readNullTerminatedString();

	 std::string readNullTerminatedString(size_t stepSize);

	 /*!
	  * Read a line of text until a certain delimiter
	  * \param delimiter the delimiter until to read
	  * \return the string read
	  */
	 std::string readLine(char delimiter = '\n');

	/*!
	 * Read a sub stream out of the stream with a
	 * specified length
	 * \param length the length to read the stream
	 * \return the newly created stream
	 */
	Common::ReadStream *readStream(size_t length = SIZE_MAX);

	/*!
	 * skip a specified number of bytes
	 * \param offset the number of bytes to skip
	 */
	void skip(ptrdiff_t offset);

	/*!
	 * Read a generic chunk of data into the data pointer
	 * with length specified in length
	 * \param data the data pointer in which to write the read data
	 * \param length the length of the data to read
	 * \return the length of the read data
	 */
	virtual size_t read(void *data, size_t length) = 0;

	/*!
	 * Seek a specified length from a specified origin, which
	 * is either the beginning, the end or the current position
	 * \param length the length to seek
	 * \param origin the origin to seek from
	 */
	virtual void seek(ptrdiff_t length, SeekOrigin origin = BEGIN) = 0;

	/*!
	 * Get the current position of the stream
	 * \return the current stream position
	 */
	virtual size_t pos() const = 0;

	/*!
	 * Check if the stream is at it's end
	 * \return if the stream is at it's end
	 */
	virtual bool eos() const = 0;

	/*!
	 * Get the total size of the stream in bytes
	 * \return The total size of the stream
	 */
	 virtual size_t size() const = 0;
};

} // End of namespace Common


#endif // SRC_COMMON_READSTREAMS_H
