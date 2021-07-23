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

#include "src/common/exception.h"

#include "src/awe/collisionsfile.h"

namespace AWE {

COLLISIONSFile::COLLISIONSFile(Common::ReadStream &collisions) {
	uint32_t version = collisions.readUint32LE();
	if (version != 3)
		throw Common::Exception("Invalid collisions file version. Expected 3 got {}", version);

	uint32_t numVertices = collisions.readUint32LE();
	uint32_t numIndices = collisions.readUint32LE();

	_positions.resize(numVertices);
	for (auto &position : _positions) {
		position.x = collisions.readIEEEFloatLE();
		position.y = collisions.readIEEEFloatLE();
		position.z = collisions.readIEEEFloatLE();
	}

	_indices.resize(numIndices);
	for (auto &index : _indices) {
		index = collisions.readUint16LE();
	}

	// TODO: There is more data here
}

const std::vector<glm::vec3> &COLLISIONSFile::getPositions() {
	return _positions;
}

const std::vector<uint16_t> &COLLISIONSFile::getIndices() {
	return _indices;
}

} // End of namespace AWE
