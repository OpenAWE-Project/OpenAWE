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

#include "src/common/exception.h"

#include "src/graphics/opengl/opengl.h"
#include "src/graphics/uniformbufferwriter.h"

#include <ranges>

size_t itemSize(UniformType type) {
	switch (type) {
		using enum UniformType;

		case kFloat: return sizeof(GLfloat);
		case kVec2: return 2 * sizeof(GLfloat);
		case kVec3: [[fallthrough]];
		case kVec4: return 4 * sizeof(GLfloat);

		case kInt: return sizeof(GLint);
		case kVec2I: return 2 * sizeof(GLint);
		case kVec3I: [[fallthrough]];
		case kVec4I: return 4 * sizeof(GLint);

		case kMat2: return 2 * 2 * sizeof(GLfloat);
		case kMat3x2: return 3 * 2 * sizeof(GLfloat);
		case kMat3x4: [[fallthrough]];
		case kMat3: return 3 * 4 * sizeof(GLfloat);
		case kMat4: return 4 * 4 * sizeof(GLfloat);

		default: return 0;
	}
}

size_t blockSize(const UniformBlockDescription &descr) {
	size_t size = 0;
	for (const auto &item: descr.items) {
		size += itemSize(item.type);
	}

	// Ceil the block size to 16 bytes
	size = (size + 15) & ~15;

	return size;
}

std::vector<size_t> blockOffsets(const UniformBlockDescription &descr) {
	size_t size = 0;
	std::vector<size_t> offsets;
	for (const auto &item: descr.items) {
		offsets.emplace_back(size);
		size += itemSize(item.type);
	}

	return offsets;
}

std::vector<std::string> itemNames(const UniformBlockDescription &descr) {
	std::vector<std::string> names;
	for (const auto &item: descr.items) {
		names.emplace_back(item.name);
	}

	return names;
}

namespace Graphics {

UniformBufferWriter::UniformBufferWriter(const UniformBlockDescription &descr)
	: _descr(descr)
	, _index(itemNames(descr))
	, _offsets(blockOffsets(descr))
	, _data(blockSize(descr)) {
	std::ranges::fill(_data, std::byte{0});
}

void UniformBufferWriter::setFloat(const std::string &id, float value) {
	const auto offset = itemOffset(id);
	std::memcpy(_data.data() + offset, &value, sizeof(float));
}

void UniformBufferWriter::setVec2(const std::string &id, glm::vec2 value) {
	const auto offset = itemOffset(id);
	std::memcpy(_data.data() + offset, &value, sizeof(glm::vec2));
}

void UniformBufferWriter::setVec3(const std::string &id, glm::vec3 value) {
	const auto offset = itemOffset(id);
	std::memcpy(_data.data() + offset, &value, sizeof(glm::vec3));
}

void UniformBufferWriter::setVec4(const std::string &id, glm::vec4 value) {
	const auto offset = itemOffset(id);
	std::memcpy(_data.data() + offset, &value, sizeof(glm::vec4));
}

std::span<std::byte> UniformBufferWriter::getData() {
	return _data;
}

size_t UniformBufferWriter::itemOffset(const std::string &id) const {
	const auto iter = std::ranges::find(_index, id);
	if (iter == _index.end())
		throw Common::SourceException("Cannot find uniform {} in {}", id, _descr.name);

	const auto index = std::distance(_index.begin(), iter);
	return _offsets.at(index);
}

} // Graphics
