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

#include <glm/gtx/quaternion.hpp>

#include "src/physics/physicsman.h"
#include "src/physics/charactercontroller.h"

namespace Physics {

CharacterController::CharacterController(float capsuleHeight, float capsuleRadius) :
	_ghostObject(new btPairCachingGhostObject),
	_shape(std::make_unique<btCapsuleShape>(capsuleRadius, capsuleHeight)) {
	_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	_ghostObject->setCollisionShape(_shape.get());
	setCollisionObject(_ghostObject);

	btTransform offset;
	offset.setIdentity();
	offset.setOrigin(btVector3(0.0f, capsuleHeight / 2.0f + capsuleRadius, 0.0f));
	setOffset(offset);

	_groundOffset = capsuleHeight / 2.0f + capsuleRadius;

	_characterController = new btKinematicCharacterController(
		_ghostObject,
		_shape.get(),
		1.0f
	);
	_characterController->setGravity(btVector3(0.0f, -9.8f, 0.0f));
	_characterController->setFallSpeed(100.0f);
	_characterController->setJumpSpeed(100.0f);
	_characterController->setUseGhostSweepTest(false);
}

void CharacterController::setLinearVelocity(glm::vec3 direction) {
	_characterController->setLinearVelocity(btVector3(direction.x, direction.y, direction.z) * 0.05);
}

void CharacterController::setAngularVelocity(float factor) {
	_characterController->setAngularVelocity(btVector3(0.0, factor, 0.0));
}

void CharacterController::jump() {
	if (_characterController->onGround())
		_characterController->jump();
}

glm::vec3 CharacterController::getPosition() {
	btVector3 origin = _ghostObject->getWorldTransform().getOrigin();
	return glm::vec3(origin.x(), origin.y() - _groundOffset, origin.z());
}

glm::mat3 CharacterController::getRotation() {
	btQuaternion rotation = _ghostObject->getWorldTransform().getRotation();
	return glm::toMat3(glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()));
}

void CharacterController::setActive(bool active) {
	if (_active == active)
		return;

	_active = active;

	if (active) {
		PhysicsMan.add(
			_ghostObject,
			btBroadphaseProxy::CharacterFilter,
			btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter
		);
		PhysicsMan.add(_characterController);
	} else {
		PhysicsMan.remove(_ghostObject);
		PhysicsMan.remove(_characterController);
	}
}

} // End of namespace Physics