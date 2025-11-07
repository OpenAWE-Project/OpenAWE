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

enum OrbitalCameraState {
	kCameraDefault,
	kCameraAiming,
	kCameraFocused
};

/*!
 * \brief Class containing logic for the orbital camera
 * 
 * This class expands Camera logic to handle rotation
 * around a defined point in space that is aimed to 
 * be used for the player controlled character
 */
class OrbitalCamera : public Camera {
public:
	OrbitalCamera();

	float getRotationFactor() const;

	void setRotationFactor(float rotationFactor);

	/*!
	 * Sets a new origin point to rotate around.
	 * @param newOrigin Point position as a 3D vector
	 */
	void setOrbitOrigin(const glm::vec3 &newOrigin);

	/*!
	 * Temporarily (for this frame)
	 * lower rotation orbit radius within
	 * (0.1 .. 1) * (default radius) range
	 * @param radius Radius value to lower to
	 */
	void snapRadius(const float &radius);

	/*!
	 * Make the camera look from another
	 * shoulder by flipping the shoulder's sign.
	 */
	void switchShoulder();

	/*!
	 * Make the camera look at a specific point
	 * on a scene instead of in front of the character.
	 * @param target Look target position as a 3D vector
	 */
	void focusOn(glm::vec3 target);

	/*!
	 * Remove focus from a specific point on a scene
	 * and make the camera look in front of the character.
	 */
	void unfocus();

	/*!
	 * Start or stop aiming mode that changes the FOV
	 * as well as the camera's orbit radius
	 * @param aiming Whether to aim or stop aiming.
	 */
	void setAiming(bool aiming);

	void update(float delta) override;

	/*!
	 * Get camera's look direction projected onto the XZ plane.
	 * Useful to determine where should the player character face.
	 */
	glm::vec3 getRotationPlane();

protected:
	float _rotationFactor;
	glm::vec3 _movementRotation;
	glm::vec3 _rotationDirection;
	glm::vec3 _rotationOrientation; // yaw, pitch and roll
	/* Orbit is important and complicated.
	   We have the base radius as the default,
	   Current full radius for situations when radius is purposefully shortened
	   (i.e. aiming), current radius when radius is temporarily shortened
	   (i.e. when terrain obstructs the view) and current radius to smooth from. */
	const float _orbitRadiusBase;
	float _orbitRadiusCurrent, _orbitRadiusTarget, _orbitRadiusTargetFull;
	// We smooth followed object's position to create a bit of
	// pleasant camera inertia while moving
	glm::vec3 _orbitOriginCurrent, _orbitOriginTarget;
	/* Shoulder coefficient currently depends on orbit radius to mimic
	   original game's behavior when camera's orbit radius is shortened
	   due to scenery obstructing view */
	float _shoulderCurrent, _shoulderTarget;
	static constexpr float _shoulderCoef = 0.2f;
	glm::vec3 _focusTarget;
	// Since FOV while aiming depends on a weapon, we add in a FOV multiplier
	float _fovBase = _fov, _fovAimMultiplier = 0.75f;
	OrbitalCameraState _state = kCameraDefault;
	// _radiusOverride bool is meant for situations when
	// orbit radius should be temporarily shorter (like during terrain collision)
	bool _radiusOverride;

	/*!
	 * Calculate current camera's position based off 
	 * its current radius and shoulder.
	 * @param radius Radius to calculate the position from
	 */
	glm::vec3 calcOrbitPosition(float radius);
};

} // End of namespace Graphics

#endif //OPENAWE_ORBITALCAMERA_H
