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

#ifndef OPENAWE_SUN_H
#define OPENAWE_SUN_H

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Graphics {

/**
 * Calculate the current sun direction based on the time of day, the rotation around the y axis and the latitude.
 *
 * \param timeOfDay The current time of day in the range from 0.0 to 1.0. This represents the time range from 00:00 to
 * 24:00.
 * \param yRotation The rotation around the y axis
 * \param latitude The latitude of the current position
 * \return The current sun direction which depends on the mentioned parameters
 */
static glm::vec3 getSunDir(float timeOfDay, float yRotation, float latitude) {
	// TODO: Do we need time of year here?
	glm::vec3 sunDir{1.0, 0.0, 0.0};

	sunDir = glm::mat3(glm::rotate(0.4092797f, glm::vec3(1.0, 0.0, 0.0))) * sunDir;
	sunDir = glm::mat3(glm::rotate((timeOfDay - (1.0f / 3.0f)) * -2.0f * M_PIf, glm::vec3(0.0, 0.0, 1.0))) * sunDir;
	sunDir = glm::mat3(glm::rotate(glm::radians(latitude), glm::vec3(1.0, 0.0, 0.0))) * sunDir;
	sunDir = glm::mat3(glm::rotate(glm::radians(yRotation), glm::vec3(0.0, 1.0, 0.0))) * sunDir;

	return sunDir;
}

}

#endif //OPENAWE_SUN_H
