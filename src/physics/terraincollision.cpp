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

#include "src/awe/resman.h"
#include "src/awe/collisionsfile.h"

#include "src/physics/terraincollision.h"
#include "src/physics/physicsman.h"

namespace Physics {

TerrainCollision::TerrainCollision(AWE::COLLISIONSFile &collisionsFile) {
	const auto &positions = collisionsFile.getPositions();
	const auto &indices = collisionsFile.getIndices();

	btCollisionShape *shape;

	if (indices.empty()) {
		shape = new btEmptyShape();
		setCollisionShape(shape);
		return;
	}

	_mesh = std::make_unique<btTriangleMesh>();
	for (size_t i = 0; i < indices.size() / 3; ++i) {
		const uint16_t i1 = indices[i*3], i2 = indices[i*3 + 1], i3 = indices[i*3 + 2];

		_mesh->addTriangle(
			btVector3(positions[i1].x, positions[i1].y, positions[i1].z),
			btVector3(positions[i2].x, positions[i2].y, positions[i2].z),
			btVector3(positions[i3].x, positions[i3].y, positions[i3].z)
		);
	}

	shape = new btBvhTriangleMeshShape(_mesh.get(), true);
	setCollisionShape(shape);
}

}
