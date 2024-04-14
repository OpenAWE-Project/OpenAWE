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

#ifndef AWE_CAMERA_H
#define AWE_CAMERA_H

#include <glm/glm.hpp>

namespace Graphics {

class Camera {
public:
	Camera();

	void setPosition(const glm::vec3 &position);
	void setDirection(const glm::vec3 &direction);

	/**
	 * Get eye position of the camera
	 * @return The eye position of the camera
	 */
	[[nodiscard]] const glm::vec3 &getPosition() const;

	/**
	 * Get direction of the camera
	 * @return The direction of the camera
	 */
	[[nodiscard]] const glm::vec3 &getDirection() const;

	/**
	 * Get the up direction of the camera
	 * @return The up direction of the camera
	 */
	[[nodiscard]] const glm::vec3 &getUp() const;

	glm::mat4 getLookAt() const;

	virtual void update(float time);

protected:
	glm::vec3 _position;
	glm::vec3 _direction;
	glm::vec3 _up;
};

} // End of namespace Graphics

#endif //AWE_CAMERA_H
