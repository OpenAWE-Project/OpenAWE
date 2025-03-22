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

	for (const auto &child : rootNode.children) {
		const std::string value = child.properties.at("value");

		if (child.name == "ambient_light_color")
			_ambientLightColor = parseVec3(value);
		else if (child.name == "ambient_light_intensity")
			_ambientLightIntensity = Common::parse<float>(value);
		else if (child.name == "ambient_light_saturation")
			_ambientLightSaturation = Common::parse<float>(value);
		else if (child.name == "ground_fog_density")
			_groundFogDensity = Common::parse<float>(value);
		else if (child.name == "ground_fog_falloff")
			_groundFogFalloff = Common::parse<float>(value);
		else if (child.name == "fog_color")
			_fogColor = parseVec3(value);
		else if (child.name == "fog_color_opposite")
			_fogColorOpposite = parseVec3(value);
		else if (child.name == "fog_intensity")
			_fogIntensity = Common::parse<float>(value);
		else if (child.name == "fog_intensity_opposite")
			_fogIntensityOpposite = Common::parse<float>(value);
		else if (child.name == "secondary_sky_glow_height")
			_secondarySkyGlowHeight = Common::parse<float>(value);
		else if (child.name == "weather_cloudiness")
			_weatherCloudiness = Common::parse<float>(value);
	}
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

float AmbianceState::getAmbientLightIntensity() const {
	return _ambientLightIntensity;
}

float AmbianceState::getAmbientLightSaturation() const {
	return _ambientLightSaturation;
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