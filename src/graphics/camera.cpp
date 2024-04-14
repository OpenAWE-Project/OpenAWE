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

#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

namespace Graphics {

Camera::Camera() {
	_position = glm::vec3(0.0f, 0.0f, 0.0f);
	_direction = glm::vec3(0.0f, 0.0f, 1.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::setPosition(const glm::vec3 &position) {
	_position = position;
}

void Camera::setDirection(const glm::vec3 &direction) {
	_direction = direction;
}

glm::mat4 Camera::getLookAt() const {
	return glm::lookAt(_position, _position + _direction, _up);
}

void Camera::update(float delta) {
}

const glm::vec3 &Camera::getPosition() const {
	return _position;
}

const glm::vec3 &Camera::getDirection() const {
	return _direction;
}

const glm::vec3 &Camera::getUp() const {
	return _up;
}

}
