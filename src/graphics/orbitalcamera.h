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

#ifndef OPENAWE_ORBITALCAMERA_H
#define OPENAWE_ORBITALCAMERA_H

#include "src/graphics/camera.h"

namespace Graphics {

enum OrbitalCameraStates {
	kCameraDefault,
	kCameraAiming,
	kCameraFocused
};

class OrbitalCamera : public Camera {
public:
	OrbitalCamera();

	float getRotationFactor() const;

	void setRotationFactor(float rotationFactor);

	void setOrbitOrigin(const glm::vec3 &newOrigin);

	void snapRadius(const float &radius);

	void switchShoulder();

	void focusOn(glm::vec3 target);

	void unfocus();

	void setAiming(bool aiming);

	void update(float delta) override;

	glm::vec3 getRotationPlane();

protected:
	float _rotationFactor;
	glm::vec3 _movementRotation;
	glm::vec3 _rotationDirection;
	glm::vec3 _rotationAttitude; // yaw, pitch and roll
	const float _orbitRadiusBase;
	float _orbitRadiusCurrent, _orbitRadiusTarget, _orbitRadiusTargetFull;
	glm::vec3 _orbitOriginCurrent, _orbitOriginTarget;
	float _shoulderCurrent, _shoulderTarget;
	static constexpr float _shoulderCoef = 0.2f;
	glm::vec3 _focusTarget;
	float _fovBase = _fov, _fovAimMultiplier = 0.75f;
	OrbitalCameraStates _state = kCameraDefault;
	bool _radiusOverride;

	glm::vec3 calcOrbitPosition(float radius);
};

} // End of namespace Graphics

#endif //OPENAWE_ORBITALCAMERA_H
