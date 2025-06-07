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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/physics/collisionobject.h"
#include "src/physics/physicsman.h"

namespace Physics {

CollisionObject::CollisionObject() : _collision(new btCollisionObject) {
	_offset.setIdentity();
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
	if (_active == active)
		_active = active;

	_active = active;

	if (active)
		PhysicsMan.add(_collision.get());
	else
		PhysicsMan.remove(_collision.get());
}

void CollisionObject::setCollisionShape(btCollisionShape *shape) {
	_shape.reset(shape);
	_collision->setCollisionShape(shape);
}

void CollisionObject::setCollisionObject(btCollisionObject *collisionObject) {
	_collision.reset(collisionObject);
	_collision->activate();
}

void CollisionObject::setOffset(btTransform offset) {
	_offset = offset;
}

btCollisionShape &CollisionObject::getCollisionShape() {
	return *_shape;
}

} // End of namespace Physics
