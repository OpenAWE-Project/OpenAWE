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

#include <cmath>
#include <glm/gtx/rotate_vector.hpp>

#include "src/graphics/orbitalcamera.h"

namespace Graphics {


OrbitalCamera::OrbitalCamera() : 
	_rotationFactor(10.0), _movementRotation(0.0f),
	_rotationDirection(glm::vec3(0.0f, 0.0f, 1.0f)), _rotationAttitude(0.0f) {
		_orbitOriginCurrent = _orbitOriginTarget = glm::zero<glm::vec3>();
		_shoulderCurrent = _shoulderTarget = 1.0f;
		_orbitRadiusCurrent = _orbitRadiusTarget = _orbitRadiusBase = 4.0f;
}

void OrbitalCamera::calcOrbitPosition() {
	_rotationDirection = glm::normalize(glm::vec3(
		cos(_rotationAttitude.y) * cos(_rotationAttitude.x),
		sin(_rotationAttitude.y),
		sin(_rotationAttitude.x) * cos(_rotationAttitude.y)));

	glm::vec3 right = glm::cross(_up, _rotationDirection);

	// Add orbit and shoulder
	_position = _orbitOriginCurrent + (right * _shoulderCurrent * _shoulderCoef - _rotationDirection) * _orbitRadiusCurrent;
}

void OrbitalCamera::update(float delta) {
	// Lerp values
	const float lerpCoef = 1.0f - glm::pow(0.03125f, delta);
	_orbitOriginCurrent = glm::mix(_orbitOriginCurrent, _orbitOriginTarget, lerpCoef);
	_shoulderCurrent = glm::mix(_shoulderCurrent, _shoulderTarget, lerpCoef);
	_orbitRadiusCurrent = glm::mix(_orbitRadiusCurrent, _orbitRadiusTarget, lerpCoef);
	const float fovTarget = _state == kCameraAiming ? _fovBase * _fovAimMultiplier : _fovBase;
	_fov = glm::mix(_fov, fovTarget, lerpCoef);

	// Get target look direction
	// roll (_rotationAttitude.z) is not used so far
	_rotationAttitude.x += glm::radians(_movementRotation.x * delta * _rotationFactor);
	_rotationAttitude.y -= glm::radians(_movementRotation.y * delta * _rotationFactor);
	// limit pitch to -80..80 degree range
	constexpr float cameraPitchLimit = M_PI_2 / 9 * 8;
	_rotationAttitude.y = glm::clamp(_rotationAttitude.y, -cameraPitchLimit, cameraPitchLimit);

	calcOrbitPosition();

	glm::vec3 target = 
		_state == kCameraFocused ? _focusTarget : 
		_orbitOriginCurrent + _rotationDirection * _orbitRadiusCurrent * 10.0f;

	target = glm::normalize(target - _position);
	if (glm::distance(_direction, target) > 1e-3)
		_direction = glm::slerp(_direction, target, lerpCoef);

	// Always clear rotation for mouse/gamepad input
	_movementRotation = glm::zero<glm::vec3>();
}

void OrbitalCamera::snapRadius(const float &radius) {
	_orbitRadiusCurrent = radius;
	calcOrbitPosition();
}

float OrbitalCamera::getRotationFactor() const {
	return _rotationFactor;
}

void OrbitalCamera::setRotationFactor(float rotationFactor) {
	_rotationFactor = rotationFactor;
}

void OrbitalCamera::focusOn(glm::vec3 target) {
	_state = kCameraFocused;
	_focusTarget = target;
}

void OrbitalCamera::unfocus() {
	_state = kCameraDefault;
}

void OrbitalCamera::switchShoulder() {
	_shoulderTarget = -_shoulderTarget;
}

void OrbitalCamera::setOrbitOrigin(const glm::vec3 &newOrigin) {
	_orbitOriginTarget = newOrigin;
	_orbitOriginTarget.z = -_orbitOriginTarget.z;
}

void OrbitalCamera::setAiming(bool aiming) {
	_state = aiming ? kCameraAiming : kCameraDefault;
}

glm::vec3 OrbitalCamera::getRotationPlane() { 
	return glm::normalize(glm::vec3(_rotationDirection.x, 0.0f, -_rotationDirection.z)); 
};

} // End of namespace Graphics
