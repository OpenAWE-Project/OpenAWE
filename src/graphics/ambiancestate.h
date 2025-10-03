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

#ifndef OPENAWE_AMBIANCESTATE_H
#define OPENAWE_AMBIANCESTATE_H

#include <glm/glm.hpp>

#include "src/common/readstream.h"

namespace Graphics {

class AmbianceState {
public:
	AmbianceState();
	AmbianceState(Common::ReadStream &ambianceState);

	const glm::vec3 &getAmbientLightColor() const;
	glm::vec3 getAmbientLightIntensity() const;
	float getAmbientLightSaturation() const;

	glm::vec3 getAmbientSHIntensity1() const;
	glm::vec3 getAmbientSHIntensity2() const;
	glm::vec3 getAmbientSHDirection1() const;
	glm::vec3 getAmbientSHDirection2() const;

	float getGroundFogFalloff() const;
	float getGroundFogDensity() const;

	const glm::vec3 &getFogColor() const;
	const glm::vec3 &getFogColorOpposite() const;
	float getFogIntensity() const;
	float getFogIntensityOpposite() const;

	float getSecondarySkyGlowHeight() const;

	float getWeatherCloudiness() const;
	float getCloudThreshold() const;

private:
	glm::vec3 parseVec3(const std::string &str);

	glm::vec3 _ambientLightColor;
	float _ambientLightIntensity;
	float _ambientLightSaturation;

	glm::vec3 _ambientSHIntensity1, _ambientSHIntensity2;
	glm::vec3 _ambientSHDirection1, _ambientSHDirection2;

	float _groundFogFalloff, _groundFogDensity;
	glm::vec3 _fogColor, _fogColorOpposite;
	float _fogIntensity, _fogIntensityOpposite;

	float _secondarySkyGlowHeight;

	float _weatherCloudiness;
};

}

#endif //OPENAWE_AMBIANCESTATE_H
