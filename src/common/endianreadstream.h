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

#ifndef SRC_COMMON_ENDIANREADSTREAMS_H
#define SRC_COMMON_ENDIANREADSTREAMS_H

#include "src/common/readstream.h"
#include "src/common/types.h"

namespace Common {

class EndianReadStream {
public:
	/*!
	 * Create a ReadStream wrapper that has a predefined
	 * endianness.
	 */
	EndianReadStream(ReadStream *parentStream, bool bigEndian);

	virtual ~EndianReadStream();

	/*!
	 * Read a 16 bit unsigned int
	 * \return the 16bit value
	 */
	uint16_t readUint16();

	/*!
	 * Read a 32 bit unsigned int
	 * \return the 32bit value
	 */
	uint32_t readUint32();

	/*!
	 * Read a 64 bit unsigned int
	 * \return the 64bit value
	 */
	uint64_t readUint64();

private:
	bool _bigEndian;
	ReadStream *_parentStream;
};

} // End of namespace Common

#endif //SRC_COMMON_ENDIANREADSTREAMS_H