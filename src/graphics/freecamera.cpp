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

#include <glm/gtx/rotate_vector.hpp>

#include "src/graphics/freecamera.h"

namespace Graphics {

FreeCamera::FreeCamera() : 
	_movementFactor(1.0), _movementDirection(0.0f), 
	_rotationFactor(10.0), _movementRotation(0.0f), _rotationAttitude(0.0f){
}

void FreeCamera::update(float delta) {
	_rotationAttitude.x += glm::radians(_movementRotation.x * delta * _rotationFactor);
	_rotationAttitude.y -= glm::radians(_movementRotation.y * delta * _rotationFactor);
	// roll is not used so far
	_rotationAttitude.z += glm::radians(_movementRotation.z * delta * _rotationFactor);
	// limit pitch to -90..90 degree range
	_rotationAttitude.y = glm::clamp(double(_rotationAttitude.y), -M_PI_2, M_PI_2);
	_direction.x = cos(_rotationAttitude.y) * cos(_rotationAttitude.x);
	_direction.y = sin(_rotationAttitude.y);
	_direction.z = sin(_rotationAttitude.x) * cos(_rotationAttitude.y);
	_direction = glm::normalize(_direction);

	glm::vec3 right = glm::cross(_up, _direction);

	_position += delta * _movementFactor * _movementDirection.x * 100.0f * right;
	_position += delta * _movementFactor * _movementDirection.y * 100.0f * _up;
	_position += delta * _movementFactor * _movementDirection.z * 100.0f * _direction;

	// clear movementRotation
	_movementRotation = glm::zero<glm::vec3>();
}

float FreeCamera::getMovementFactor() const {
	return _movementFactor;
}

void FreeCamera::setMovementFactor(float movementFactor) {
	_movementFactor = movementFactor;
}

float FreeCamera::getRotationFactor() const {
	return _rotationFactor;
}

void FreeCamera::setRotationFactor(float rotationFactor) {
	_rotationFactor = rotationFactor;
}

} // End of namespace Graphics
