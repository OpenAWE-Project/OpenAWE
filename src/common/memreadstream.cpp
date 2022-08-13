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
#include <algorithm>

#include "src/common/exception.h"
#include "src/common/memreadstream.h"

namespace Common {

MemoryReadStream::MemoryReadStream(byte *data, size_t length, bool dispose) : _data(data), _size(length), _dispose(dispose), _position(0) {
}

MemoryReadStream::MemoryReadStream(const byte *data, size_t length) : _data(data), _size(length), _dispose(false), _position(0) {
}

MemoryReadStream::MemoryReadStream(const char *data, size_t length) :
	_data(reinterpret_cast<const byte*>(data)),
	_size(length),
	_dispose(false),
	_position(0) {
}

MemoryReadStream::~MemoryReadStream() {
	if (_dispose)
		delete[] _data;
}

size_t MemoryReadStream::read(void *data, size_t length) {
	size_t sizeToRead = std::clamp<size_t>(length, 0, _size - _position);
	std::memcpy(data, _data + _position, sizeToRead);
	_position += length;
	return sizeToRead;
}

void MemoryReadStream::seek(ptrdiff_t length, ReadStream::SeekOrigin origin) {
	switch (origin) {
		case BEGIN:
			_position = length;
			break;
		case CURRENT:
			_position += length;
			break;
		case END:
			_position = _size + length;
			break;
	}

	if (_position > _size || _position < 0)
		throw Common::Exception("Memory stream out of bounds");
}

bool MemoryReadStream::eos() const {
	return _position >= _size;
}

size_t MemoryReadStream::pos() const {
	return _position;
}

size_t MemoryReadStream::size() const {
	return _size;
}

}
