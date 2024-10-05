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

#ifndef OPENAWE_SKY_H
#define OPENAWE_SKY_H

#include "model.h"

namespace Graphics {

class Sky;

typedef std::shared_ptr<Sky> SkyPtr;

/*!
 * \brief Class representing the sky in the scenery
 *
 * This class represents the sky and manages all geometry, textures and uniform
 * values necessary for generating the sky according to the variables.
 */
class Sky {
public:
	Sky();

	/*!
	 * Get the mesh of the sky
	 *
	 * \return The mesh of the sky
	 */
	[[nodiscard]] const MeshPtr &getSkyMesh() const;

	/*!
	 * Get all special uniforms which are not statically added to the mesh and can change
	 *
	 * \return The non-mesh uniforms for the sky
	 */
	[[nodiscard]] const std::vector<Material::Uniform> &getUniforms() const;

	/*!
	 * Set rotation around the Y Axis of the sun in degrees
	 *
	 * \param angle The angle of the rotation of the sun around the Y Axis in degree
	 */
	void setSunYRotation(float angle);

	/*!
	 * Get the current sun rotation around the Y Axis
	 *
	 * \return Get the current sun rotation around the Y Axis
	 */
	[[nodiscard]] float getSunYRotation() const;

	/*!
	 * Set the time of day for the sky. It should be given in a range of [0, 1] which represents the time from 0:00 to
	 * 24:00
	 *
	 * \param timeOfDay The time of day for the current sky
	 */
	void setTimeOfDay(float timeOfDay);

	/*!
	 * Get the current time of day in a range of [0, 1] which equals to the time range of 0:00 to 24:00
	 *
	 * \return The timof day in range between 0 and 1
	 */
	[[nodiscard]] float getTimeOfDay() const;

	/*!
	 * The latitude of the placeb influencing the sun direction
	 *
	 * \param latitude The latitude for the sun direction in degrees
	 */
	void setLatitude(float latitude);

	/*!
	 * Get the current latitude of the current place. This influences the sun direction
	 *
	 * \return The latitude in degrees
	 */
	[[nodiscard]] float getLatitude() const;

	/*!
	 * Calculate the current sun direction
	 * @return
	 */
	[[nodiscard]] glm::vec3 getSunDir() const;

private:
	float _sunYRotation;
	float _timeOfDay;
	float _latitude;
	Graphics::MeshPtr _sky;
	std::vector<Material::Uniform> _uniforms;
};

} // End of namespace Graphics

#endif //OPENAWE_SKY_H
