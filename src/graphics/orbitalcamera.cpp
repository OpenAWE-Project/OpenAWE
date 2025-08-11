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

#include <algorithm>
#include <cmath>
#include <glm/gtx/rotate_vector.hpp>

#include "src/graphics/orbitalcamera.h"

namespace Graphics {


OrbitalCamera::OrbitalCamera() : 
	_rotationFactor(2.0), _movementRotation(0.0f), 
	_rotationDirection(glm::vec3(0.0f, 0.0f, 1.0f)), _rotationOrientation(0.0f), _orbitRadiusBase(2.5f),
	_radiusOverride(false) {
		_orbitOriginCurrent = _orbitOriginTarget = glm::zero<glm::vec3>();
		_shoulderCurrent = _shoulderTarget = 1.25f;
		_orbitRadiusCurrent = _orbitRadiusTarget = _orbitRadiusTargetFull = _orbitRadiusBase;
}

glm::vec3 OrbitalCamera::calcOrbitPosition(float radius) {
	// Convert camera direction from pitch and yaw 
	// to a 3D vector
	_rotationDirection = glm::normalize(glm::vec3(
		cos(_rotationOrientation.y) * cos(_rotationOrientation.x),
		sin(_rotationOrientation.y),
		sin(_rotationOrientation.x) * cos(_rotationOrientation.y)));

	const glm::vec3 right = glm::cross(_up, _rotationDirection);

	constexpr float cameraPitchLimit = M_PI_2 / 3 * 2;
	/* Shoulder offset gets calculated relative to camera's yaw,
	   so with higher upper angle shoulder would get wider,
	   keeping the position between player's character and the center of the screen
	   relatively consistent. */
	const float shoulderCoef = _shoulderCoef * (1 + std::abs(_rotationOrientation.y / cameraPitchLimit));

	/* Camera's position consists of 4 main components:
	   1. Smoothed out followed object's position;
	   2. Orbit radius opposite to view direction;
	   3. Shoulder offset parallel to view direction;
	   4. Extra rise in elevation for cases when camera
	   gets close to player's character to mimic
	   original game's behavior. */
	return _orbitOriginCurrent + 
		(right * _shoulderCurrent * shoulderCoef - _rotationDirection) * radius - 
		_up * radius / _orbitRadiusBase * 0.75f;
}

void OrbitalCamera::update(float delta) {
	// Define multiple frame-independent lerping coefficients
	// to smooth out various camera movements
	const float lerpCoefSmooth = 1.0f - glm::pow(0.1f, delta);
	const float lerpCoefSnappy = 1.0f - glm::pow(1e-4, delta);
	const float lerpCoefExtraSnappy = 1.0f - glm::pow(1e-6, delta);
	// Original games would change shoulder offset rather slowly, 
	// so we use a smaller coefficient
	_shoulderCurrent = glm::mix(_shoulderCurrent, _shoulderTarget, lerpCoefSmooth);
	if (!_radiusOverride) {
		// When overriding radius, _orbitRadiusTarget gets smaller, 
		// so we smooth it back to full when not overriding
		_orbitRadiusTarget = glm::mix(_orbitRadiusTarget, _orbitRadiusTargetFull, lerpCoefSnappy);
	} else { 
		_radiusOverride = false;
	}
	_orbitRadiusCurrent = glm::mix(_orbitRadiusCurrent, _orbitRadiusTarget, lerpCoefSnappy);
	_orbitOriginCurrent = glm::mix(_orbitOriginCurrent, _orbitOriginTarget, lerpCoefSnappy);
	// Smooth out FOV changes during and after aiming
	const float fovTarget = _state == kCameraAiming ? _fovBase * _fovAimMultiplier : _fovBase;
	_fov = glm::mix(_fov, fovTarget, lerpCoefSnappy);

	// Get target look direction as pitch and yaw values
	// roll (_rotationAttitude.z) is not used so far
	_rotationOrientation.x += glm::radians(_movementRotation.x * delta * _rotationFactor);
	_rotationOrientation.y -= glm::radians(_movementRotation.y * delta * _rotationFactor);
	// limit pitch to -60..60 degree range similar to original games
	constexpr float cameraPitchLimit = M_PI_2 / 3 * 2;
	_rotationOrientation.y = glm::clamp(_rotationOrientation.y, -cameraPitchLimit, cameraPitchLimit);

	_position = calcOrbitPosition(_orbitRadiusCurrent);

	// When focused, our look target is overriden, but usually we look in front of the character
	glm::vec3 target = 
		_state == kCameraFocused ? _focusTarget : _rotationDirection * 25.0f + _position;

	target = glm::normalize(target - _position);
	// Here, we use spherical interpolation to smoothly rotate
	// our camera towards desired direction
	if (glm::distance(_direction, target) > 1e-3)
		_direction = glm::slerp(_direction, target, lerpCoefExtraSnappy);

	// Always clear rotation for mouse/gamepad input in some way.
	// We use glm::mix to leave a bit of inertia from inputs, 
	// making camera movement way smoother to the eye
	_movementRotation = glm::mix(_movementRotation, glm::zero<glm::vec3>(), lerpCoefExtraSnappy);
}

void OrbitalCamera::snapRadius(const float &radius) {
	_orbitRadiusTarget = std::clamp(radius, 0.1f, _orbitRadiusTargetFull);
	_radiusOverride = true;
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
	_orbitRadiusTargetFull = aiming ? _orbitRadiusBase / 2 : _orbitRadiusBase;
}

glm::vec3 OrbitalCamera::getRotationPlane() { 
	return glm::normalize(glm::vec3(_rotationDirection.x, 0.0f, -_rotationDirection.z)); 
};

} // End of namespace Graphics
