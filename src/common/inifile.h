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

#ifndef OPENAWE_INIFILE_H
#define OPENAWE_INIFILE_H

#include <map>

#include <glm/glm.hpp>

#include "src/common/readstream.h"
#include "src/common/exception.h"
#include "src/common/strutil.h"

namespace Common {

/*!
 * \brief
 */
class INIFile {
public:
	INIFile(Common::ReadStream &ini);

	template<typename T> T get(const std::string &section, const std::string &parameters) const;

	int getInt(const std::string &section, const std::string &parameter) const;
	float getFloat(const std::string &section, const std::string &parameter) const;
	std::string getString(const std::string &section, const std::string &parameter) const;
	glm::vec3 getVec3(const std::string &section, const std::string &parameter) const;

private:
	typedef std::tuple<std::string, std::string> Id;

	std::map<Id, std::string> _data;
};

/*template<> double INIFile::get<double>(const std::string &s, const std::string &p) const {
	return std::stod(_data.at(Id(s, p)));
}

template<> long INIFile::get<long>(const std::string &s, const std::string &p) const {
	return std::stol(_data.at(Id(s, p)));
}*/

/*template<size_t L> glm::vec<L, float> INIFile::get(const std::string &s, const std::string &p) const {
	const auto vecString = _data.at(Id(s, p));
	const auto vecStrings = Common::split(vecString, std::regex(" "));

	if (vecStrings.size() != L)
		throw CreateException("Invalid vector string, expected {} values, got {}", L, vecStrings.size());

	glm::vec<L, float> vec;
	for (int i = 0; i < L; ++i) {
		vec[i] = std::stof(vecStrings[i]);
	}

	return vec;
}*/

} // End of namespace Common

#endif //OPENAWE_INIFILE_H
