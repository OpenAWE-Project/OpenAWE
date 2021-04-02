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

#include <cstdlib>
#include <cstring>
#include "memwritestream.h"

namespace Common {

DynamicMemoryWriteStream::DynamicMemoryWriteStream(bool release) : _release(release), _capacity(0), _size(0), _data(nullptr) {
}

DynamicMemoryWriteStream::~DynamicMemoryWriteStream() {
	if (_release && _data)
		delete[] _data;
}

size_t DynamicMemoryWriteStream::getLength() {
	return _size;
}

byte * DynamicMemoryWriteStream::getData() {
	return _data;
}

void DynamicMemoryWriteStream::write(const void *data, size_t length) {
	while (_size + length > _capacity) {
		extendCapacity();
	}

	std::memcpy(_data + _size, data, length);
	_size += length;
}

void DynamicMemoryWriteStream::extendCapacity() {
	byte *oldData = _data;
	_capacity += 4096;
	_data = new unsigned char[_capacity];
	std::memset(_data, 0, _capacity);

	if (oldData) {
		std::memcpy(_data, oldData, _size);
		delete[] oldData;
	}
}

void DynamicMemoryWriteStream::seek(ptrdiff_t length, WriteStream::SeekOrigin origin) {
	//TODO
}

size_t DynamicMemoryWriteStream::pos() {
	return _size;
}

} // End of namespace Common
