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

#include <iterator>

#include "objectbinaryreadstream.h"

namespace AWE {

ObjectBinaryReadStream::ObjectBinaryReadStream(Common::ReadStream &stream) : _stream(stream) {
}

ObjectBinaryReadStream::ObjectBinaryReadStream(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) : _stream(stream), _dp(dp) {
}

void ObjectBinaryReadStream::variable(const std::string &name, bool &value) {
	value = _stream.readByte() != 0;
}

void ObjectBinaryReadStream::variable(const std::string &name, int32_t &value) {
	value = _stream.readSint32LE();
}

void ObjectBinaryReadStream::variable(const std::string &name, uint32_t &value, bool bigEndian) {
	if (bigEndian)
		value = _stream.readUint32BE();
	else
		value = _stream.readUint32LE();
}

void ObjectBinaryReadStream::variable(const std::string &name, float &value) {
	value = _stream.readIEEEFloatLE();
}

void ObjectBinaryReadStream::variable(const std::string &name, std::string &value, bool dp) {
	if (dp) {
		uint32_t offset = _stream.readUint32LE();
		value = _dp->getString(offset);
	} else {
		uint32_t length = _stream.readUint32LE();
		value = _stream.readFixedSizeString(length);
	}
}

void ObjectBinaryReadStream::variable(const std::string &name, glm::vec3 &value) {
	value.x = _stream.readIEEEFloatLE();
	value.y = _stream.readIEEEFloatLE();
	value.z = _stream.readIEEEFloatLE();
}

void ObjectBinaryReadStream::variable(const std::string &name, glm::mat3 &value) {
	value[0].x = _stream.readIEEEFloatLE();
	value[0].y = _stream.readIEEEFloatLE();
	value[0].z = _stream.readIEEEFloatLE();
	value[1].x = _stream.readIEEEFloatLE();
	value[1].y = _stream.readIEEEFloatLE();
	value[1].z = _stream.readIEEEFloatLE();
	value[2].x = _stream.readIEEEFloatLE();
	value[2].y = _stream.readIEEEFloatLE();
	value[2].z = _stream.readIEEEFloatLE();
}

void ObjectBinaryReadStream::variable(const std::string &name, GID &value) {
	value.type = _stream.readUint32LE();
	value.id = _stream.readUint32BE();
}

void ObjectBinaryReadStream::variable(const std::string &name, ObjectID &value) {
	value = _stream.readUint32LE();
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<int32_t> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	std::vector<uint32_t> values = _dp->getValues(offset, count);
	std::copy(values.begin(), values.end(), std::back_inserter(value));
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<bool> &value, size_t fixedSize) {
	value.resize(fixedSize);
	for (std::vector<bool>::reference item : value) {
		item = _stream.readByte() != 0;
	}
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<uint32_t> &value, bool dp) {
	uint32_t count = _stream.readUint32LE();
	if (dp) {
		uint32_t offset = _stream.readUint32LE();
		std::vector<uint32_t> values = _dp->getValues(offset, count);
	} else {
		value.resize(count);
		for (auto &item: value) {
			item = _stream.readUint32LE();
		}
	}
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<int32_t> &value, size_t fixedSize) {
	value.resize(fixedSize);
	for (auto &item: value) {
		item = _stream.readSint32LE();
	}
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<rid_t> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	std::vector<uint32_t> values = _dp->getValues(offset, count);
	std::copy(values.begin(), values.end(), std::back_inserter(value));
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<glm::vec2> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	value = _dp->getPositions2D(offset, count);
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<float> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	value = _dp->getFloats(offset, count);
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<ObjectID> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	std::vector<uint32_t> values = _dp->getValues(offset, count);
	std::copy(values.begin(), values.end(), std::back_inserter(value));
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<GID> &value) {
	uint32_t count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	value = _dp->getGIDs(offset, count);
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<std::string> &value) {
	uint32_t count = _stream.readUint32LE();
	value.resize(count);
	for (unsigned int i = 0; i < count; ++i) {
		uint32_t offset = _stream.readUint32LE();
		value[i] = _dp->getString(offset);
	}
}

void ObjectBinaryReadStream::variable(const std::string &name, std::vector<std::string> &value, size_t fixedSize) {
	value.resize(fixedSize);
	for (size_t i = 0; i < fixedSize; ++i) {
		uint32_t offset = _stream.readUint32LE();
		value[i] = _dp->getString(offset);
	}
}

void ObjectBinaryReadStream::object(const std::string &name, Object &value, ObjectType type) {
	value = readObject(type);
}

void ObjectBinaryReadStream::objects(const std::string &name, std::vector<Object> &value, ObjectType type) {
	uint32_t count = _stream.readUint32LE();
	value.resize(count);
	for (auto &item: value) {
		object("", item, type);
	}
}

void ObjectBinaryReadStream::skip(size_t s) {
	_stream.skip(s);
}

} // End of namespace AWE
