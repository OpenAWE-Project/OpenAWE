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

#ifndef OPENAWE_FREECAMERA_H
#define OPENAWE_FREECAMERA_H

#include "src/graphics/camera.h"

namespace Graphics {

class FreeCamera : public Camera {
public:
	FreeCamera();

	float getMovementFactor() const;

	void setMovementFactor(float movementFactor);

	float getRotationFactor() const;

	void setRotationFactor(float rotationFactor);

	void update(float delta) override;

protected:
	float _movementFactor;
	glm::vec3 _movementDirection;
	float _rotationFactor;
	glm::vec3 _movementRotation;
	glm::vec3 _rotationAttitude; // yaw, pitch and roll
	bool _clearDirection, _clearRotation;
};

} // End of namespace Graphics

#endif //OPENAWE_FREECAMERA_H
