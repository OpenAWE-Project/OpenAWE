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

#include <stdexcept>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/awe/types.h"
#include "src/awe/resman.h"

#include "src/physics/collisionobject.h"
#include "src/physics/physicsman.h"

namespace Physics {

CollisionObject::CollisionObject() : _collision(new btCollisionObject) {
	_offset.setIdentity();
	_collision->activate();
}

CollisionObject::CollisionObject(rid_t rid) : _collision(new btCollisionObject) {
	std::unique_ptr<Common::ReadStream> havok(ResMan.getResource(rid));
	if (!havok)
		throw std::runtime_error(fmt::format("Havok file for animation not found with the rid {:x}", rid));

	AWE::HavokFile havokFile(*havok);
	const auto &physicsSystem = havokFile.getPhysicsSystem();
	if (physicsSystem.rigidBodies.empty())
		return;

	const auto rigidBody = havokFile.getRigidBody(physicsSystem.rigidBodies.front());
	const auto shape = havokFile.getShape(rigidBody.shape);

	switch (shape.type) {
		case AWE::HavokFile::kBox: {
			const auto boxShape = std::get<AWE::HavokFile::hkpBoxShape>(shape.shape);
			btVector3 halfExtents(
				boxShape.halfExtents.x,
				boxShape.halfExtents.y,
				boxShape.halfExtents.z
			);

			_shape = std::make_unique<btBoxShape>(halfExtents);
			_collision->setCollisionShape(_shape.get());
			break;
		}

		default:
			throw std::runtime_error("Invalid shape type");
	}

	_offset.setOrigin(btVector3(
		rigidBody.position.x,
		rigidBody.position.y,
		rigidBody.position.z
	));

	_offset.setRotation(btQuaternion(
		rigidBody.rotation.x,
		rigidBody.rotation.y,
		rigidBody.rotation.z,
		rigidBody.rotation.w
	));


	_collision->activate();

	PhysicsMan.add(_collision.get());
}

void CollisionObject::setTransform(const glm::vec3 &position, const glm::mat3 &rotation) {
	btTransform transform;
	transform.setOrigin(btVector3(position.x, position.y, position.z));

	glm::quat r = glm::toQuat(rotation);
	btQuaternion q(r.x, r.y, r.z, r.w);
	transform.setRotation(q);

	_collision->setWorldTransform(transform * _offset);
}

void CollisionObject::setActive(bool active) {
	if (active)
		PhysicsMan.add(_collision.get());
	else
		PhysicsMan.remove(_collision.get());
}

void CollisionObject::setCollisionShape(btCollisionShape *shape) {
	_shape.reset(shape);
	_collision->setCollisionShape(shape);
}

void CollisionObject::setOffset(btTransform offset) {
	_offset = offset;
}

btCollisionShape &CollisionObject::getCollisionShape() {
	return *_shape;
}

} // End of namespace Physics
