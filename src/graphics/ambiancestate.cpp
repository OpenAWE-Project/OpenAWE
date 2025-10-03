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

#include "src/common/xml.h"
#include "src/common/strutil.h"

#include "src/graphics/ambiancestate.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Graphics {

AmbianceState::AmbianceState() :
	_ambientLightColor(0.0f),
	_ambientLightIntensity(0.0f),
	_ambientLightSaturation(0.0f) {

}

AmbianceState::AmbianceState(Common::ReadStream &ambianceState) {
	Common::XML xml;
	xml.read(ambianceState);
	const auto &rootNode = xml.getRootNode();

	if (rootNode.name != "ambiance_state")
		throw std::runtime_error("Invalid ambiance state file");

	float shPitch1{}, shHeading1{};
	float shPitch2{}, shHeading2{};
	glm::vec3 shColor1{}, shColor2{};
	float shIntensity1{}, shIntensity2{};

	const auto getFloat = [&] (const std::string &name) {
		if (!rootNode.hasNode(name))
			return 0.0f;
		return rootNode.getNode(name).getFloat("value");
	};

	const auto getVec3 = [&] (const std::string &name) {
		if (!rootNode.hasNode(name))
			return glm::zero<glm::vec3>();
		return parseVec3(rootNode.getNode(name).getString("value"));
	};

	_ambientLightColor = getVec3("ambient_light_color");
	_ambientLightIntensity = getFloat("ambient_light_intensity");
	_ambientLightSaturation = getFloat("ambient_light_saturation");
	_fogColor = getVec3("fog_color");
	_fogColorOpposite = getVec3("fog_color_opposite");
	_fogIntensity = getFloat("fog_intensity");
	_fogIntensityOpposite = getFloat("fog_intensity_opposite");
	_secondarySkyGlowHeight = getFloat("secondary_sky_glow_height");
	_weatherCloudiness = getFloat("weather_cloudiness");

	shPitch1 = glm::radians(getFloat("ambient_light_sh_pitch_1"));
	shPitch2 = glm::radians(getFloat("ambient_light_sh_pitch_2"));
	shHeading1 = glm::radians(getFloat("ambient_light_sh_heading_1"));
	shHeading2 = glm::radians(getFloat("ambient_light_sh_heading_2"));
	shColor1 = getVec3("ambient_light_sh_color_1");
	shColor2 = getVec3("ambient_light_sh_color_2");
	shIntensity1 = getFloat("ambient_light_sh_intensity_1");
	shIntensity2 = getFloat("ambient_light_sh_intensity_2");

	_ambientSHDirection1 = glm::normalize(glm::rotateY(glm::rotateX(glm::vec3{0.0, 0.0, 1.0}, shPitch1), shHeading1));
	_ambientSHDirection2 = glm::normalize(glm::rotateY(glm::rotateX(glm::vec3{0.0, 0.0, 1.0}, shPitch2), shHeading2));
	_ambientSHIntensity1 = shColor1 * shIntensity1;
	_ambientSHIntensity2 = shColor2 * shIntensity2;
}

glm::vec3 AmbianceState::parseVec3(const std::string &str) {
	const auto values = Common::split(str, std::regex(" "));
	return glm::vec3(
			Common::parse<float>(values[0]),
			Common::parse<float>(values[1]),
			Common::parse<float>(values[2])
	);
}

const glm::vec3 &AmbianceState::getAmbientLightColor() const {
	return _ambientLightColor;
}

glm::vec3 AmbianceState::getAmbientLightIntensity() const {
	return _ambientLightIntensity * _ambientLightColor;
}

float AmbianceState::getAmbientLightSaturation() const {
	return _ambientLightSaturation;
}

glm::vec3 AmbianceState::getAmbientSHIntensity1() const {
	return _ambientSHIntensity1;
}

glm::vec3 AmbianceState::getAmbientSHIntensity2() const {
	return _ambientSHIntensity2;
}

glm::vec3 AmbianceState::getAmbientSHDirection1() const {
	return _ambientSHDirection1;
}

glm::vec3 AmbianceState::getAmbientSHDirection2() const {
	return _ambientSHDirection2;
}

const glm::vec3 &AmbianceState::getFogColor() const {
	return _fogColor;
}

const glm::vec3 &AmbianceState::getFogColorOpposite() const {
	return _fogColorOpposite;
}

float AmbianceState::getFogIntensity() const {
	return _fogIntensity;
}

float AmbianceState::getFogIntensityOpposite() const {
	return _fogIntensityOpposite;
}

float AmbianceState::getGroundFogFalloff() const {
	return _groundFogFalloff;
}

float AmbianceState::getGroundFogDensity() const {
	return _groundFogDensity;
}

float AmbianceState::getSecondarySkyGlowHeight() const {
	return _secondarySkyGlowHeight;
}

float AmbianceState::getWeatherCloudiness() const {
	return _weatherCloudiness;
}

float AmbianceState::getCloudThreshold() const {
	return (1.0f - _weatherCloudiness) * 0.8f + 0.1f;
}

}
