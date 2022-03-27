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

#include "mesh.h"

namespace Graphics {

Mesh::Mesh() {

}

const std::vector<Mesh::PartMesh> &Mesh::getMeshs() const {
	return _meshs;
}

BufferPtr Mesh::getIndices() const {
	return _indices;
}

const std::map<std::string, glm::mat4> & Mesh::getInitialJointPositions() const {
	return _initialPose;
}

void Mesh::setIndices(BufferPtr indices) {
	_indices = indices;
}

void Mesh::addPartMesh(const Mesh::PartMesh &partMesh) {
	_meshs.emplace_back(partMesh);
}

}
