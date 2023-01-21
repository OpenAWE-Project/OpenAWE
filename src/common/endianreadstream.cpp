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

#include "src/common/endianreadstream.h"
#include "src/common/readstream.h"

namespace Common {

EndianReadStream::EndianReadStream(ReadStream *parentStream, bool bigEndian) {
    _bigEndian = bigEndian;
    _parentStream = parentStream;
};

EndianReadStream::~EndianReadStream() = default;

uint16_t EndianReadStream::readUint16() {
    return _bigEndian ? _parentStream->readUint16BE() : _parentStream->readUint16LE();
}

uint32_t EndianReadStream::readUint32() {
    return _bigEndian ? _parentStream->readUint32BE() : _parentStream->readUint32LE();
}

uint64_t EndianReadStream::readUint64() {
    return _bigEndian ? _parentStream->readUint64BE() : _parentStream->readUint64LE();
}

int16_t EndianReadStream::readSint16() {
	return _bigEndian ? _parentStream->readSint16BE() : _parentStream->readSint16LE();
}

int32_t EndianReadStream::readSint32() {
	return _bigEndian ? _parentStream->readSint32BE() : _parentStream->readSint32LE();
}

int64_t EndianReadStream::readSint64() {
	return _bigEndian ? _parentStream->readSint64BE() : _parentStream->readSint64LE();
}

}; // End of namespace Common