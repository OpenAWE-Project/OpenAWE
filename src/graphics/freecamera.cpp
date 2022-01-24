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

FreeCamera::FreeCamera() : _movementFactor(1.0), _movementDirection(0.0f), _movementRotation(0.0f) {
}

void FreeCamera::update(float delta) {
	_direction = glm::rotateX(_direction, delta * _movementFactor * _movementRotation.x);
	_direction = glm::rotateY(_direction, delta * _movementFactor * _movementRotation.y);
	_direction = glm::rotateZ(_direction, delta * _movementFactor * _movementRotation.z);
	_direction = glm::normalize(_direction);

	glm::vec3 right = glm::cross(_up, _direction);

	_position += delta * _movementFactor * _movementDirection.x * 100.0f * right;
	_position += delta * _movementFactor * _movementDirection.y * 100.0f * _up;
	_position += delta * _movementFactor * _movementDirection.z * 100.0f * _direction;
}

float FreeCamera::getMovementFactor() const {
	return _movementFactor;
}

void FreeCamera::setMovementFactor(float movementFactor) {
	_movementFactor = movementFactor;
}

} // End of namespace Graphics
