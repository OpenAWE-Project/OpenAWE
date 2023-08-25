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
	const auto emptyRegex = std::regex(R"(\s*(#.*)?)", std::regex::ECMAScript);
	const auto sectionRegex = std::regex(R"(\s*\[.+\] *(#.*)?)", std::regex::ECMAScript);
	const auto parameterRegex = std::regex(R"([a-zA-Z0-9_]+\=.*\s*(#.*)?)", std::regex::ECMAScript);

	std::string currentSection;
	while (!ini.eos()) {
		const auto line = ini.readLine();

		if (std::regex_match(line, sectionRegex)) {
			const auto begin = line.find("[");
			const auto end = line.find("]");
			currentSection = line.substr(begin + 1, end - begin - 1);
		} else if (std::regex_match(line, parameterRegex)) {
			const auto parameters = Common::split(line, std::regex("=|#", std::regex::ECMAScript));
			assert(parameters.size() >= 2);

			// Remove trailing whitespaces
			const auto value = std::regex_replace(parameters[1], std::regex("\\s+$", std::regex::ECMAScript), "");

			_data[Id(currentSection, parameters[0])] = value;
		} else if (std::regex_match(line, emptyRegex)) {
			// Do nothing
		} else {
			throw CreateException("Invalid ini line \"{}\"", line);
		}
	}
}

template<>
std::string INIFile::get(const std::string &section, const std::string &parameter) const {
	const auto id = Id(section, parameter);

	const auto iter = _data.find(id);
	if (iter == _data.cend())
		throw CreateException("Parameter {}.{} unavailable", section, parameter);

	return iter->second;
}

template<>
int INIFile::get(const std::string &s, const std::string &p) const {
	return std::stoi(get<std::string>(s, p));
}

template<>
float INIFile::get(const std::string &s, const std::string &p) const {
	return std::stof(get<std::string>(s, p));
}

template<>
glm::vec3 INIFile::get(const std::string &s, const std::string &p) const {
	const auto vecString = get<std::string>(s, p);
	const auto vecStrings = Common::split(vecString, std::regex(" "));

	if (vecStrings.size() != 3)
		throw CreateException("Invalid vector string, expected {} values, got {}", 3, vecStrings.size());

	glm::vec3 vec;
	vec.x = std::stof(vecStrings[0]);
	vec.y = std::stof(vecStrings[1]);
	vec.z = std::stof(vecStrings[2]);

	return vec;
}

} // End of namespace Common