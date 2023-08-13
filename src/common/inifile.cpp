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

#include <regex>

#include "src/common/exception.h"
#include "src/common/inifile.h"
#include "src/common/strutil.h"

namespace Common {

INIFile::INIFile(ReadStream &ini) {
	const auto emptyRegex = std::regex(" *(#.*)?");
	const auto sectionRegex = std::regex(" *\\[.+\\] *(#.*)?");
	const auto parameterRegex = std::regex("[a-zA-Z0-9\\_]+\\=.* *(#.*)?");

	std::string currentSection;
	while (!ini.eos()) {
		const auto line = ini.readLine();

		if (std::regex_match(line, sectionRegex)) {
			const auto begin = line.find("[");
			const auto end = line.find("]");
			currentSection = line.substr(begin + 1, end - begin - 1);
		} else if (std::regex_match(line, parameterRegex)) {
			const auto parameters = Common::split(line, std::regex("=|#"));
			assert(parameters.size() >= 2);

			// Remove trailing whitespaces
			const auto value = std::regex_replace(parameters[1], std::regex(" +$"), "");

			_data[Id(currentSection, parameters[0])] = value;
		} else if (std::regex_match(line, emptyRegex)) {
			// Do nothing
		} else {
			throw CreateException("Invalid ini line \"{}\"", line);
		}
	}
}

int INIFile::getInt(const std::string &section, const std::string &parameter) const {
	const auto id = Id(section, parameter);

	const auto iter = _data.find(id);
	if (iter == _data.cend())
		throw CreateException("Parameter {}.{} unavailable", section, parameter);

	return std::stoi(iter->second);
}

float INIFile::getFloat(const std::string &section, const std::string &parameter) const {
	const auto id = Id(section, parameter);

	const auto iter = _data.find(id);
	if (iter == _data.cend())
		throw CreateException("Parameter {}.{} unavailable", section, parameter);

	return std::stof(iter->second);
}

std::string INIFile::getString(const std::string &section, const std::string &parameter) const {
	const auto id = Id(section, parameter);

	const auto iter = _data.find(id);
	if (iter == _data.cend())
		throw CreateException("Parameter {}.{} unavailable", section, parameter);

	return iter->second;
}

glm::vec3 INIFile::getVec3(const std::string &section, const std::string &parameter) const {
	const auto id = Id(section, parameter);

	const auto iter = _data.find(id);
	if (iter == _data.cend())
		throw CreateException("Parameter {}.{} unavailable", section, parameter);

	const auto vecStrings = Common::split(iter->second, std::regex(" "));

	if (vecStrings.size() != 3)
		throw CreateException("Invalid vec3 string {}", iter->second);

	glm::vec3 v;
	v.x = std::stof(vecStrings[0]);
	v.y = std::stof(vecStrings[1]);
	v.z = std::stof(vecStrings[2]);

	return v;
}

} // End of namespace Common