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
 * \brief Class for parsing ini files
 *
 * This class helps parsing the ini file format and retrieving specific information from it. The ini file format is a
 * simple text format mainly used for configuration files. This variant specifies a section which groups values together
 * and single parameters which are specified by an equal sign. Furthermore it allows comments with the character #
 * \code{.ini}
 * # This is a comment
 *
 * [Section1]
 * parameter1=value1 # This is another comment
 *
 * [Section2]
 * parameter1=1.0
 * parameter2=1.0 2.0 3.0
 * \endcode
 */
class INIFile {
public:
	INIFile(Common::ReadStream &ini);

	/*!
	 * Get a certain value from the ini file into a specific type, given by the template T. It throws an exception if
	 * the specific parameter is unavailable or for some reason not convertable to the requested type.
	 * \tparam T The type of the value to return
	 * \param section The section from which to get the value
	 * \param parameter The parameter name of the value
	 * \return The requested value in the type T
	 */
	template<typename T> T get(const std::string &section, const std::string &parameter) const;

private:
	typedef std::tuple<std::string, std::string> Id;

	std::map<Id, std::string> _data;
};

/*!
 * Template specialization to load string values
 * \param s The section of the value
 * \param p The parameter name of the value
 * \return A string containing the value
 */
template<> std::string INIFile::get(const std::string &s, const std::string &p) const;

/*!
 * Template specialization to load int values
 * \param s The section of the value
 * \param p The parameter name of the value
 * \return An int variable containing the value
 */
template<> int INIFile::get(const std::string &s, const std::string &p) const;

/*!
 * Template specialization to load float values
 * \param s The section of the value
 * \param p The parameter name of the value
 * \return An int variable containing the value
 */
template<> float INIFile::get(const std::string &s, const std::string &p) const;

/*!
 * Template specialization to load vec3 values
 * \param s The section of the value
 * \param p The parameter name of the value
 * \return A vec3 variable containing the values
 */
template<> glm::vec3 INIFile::get(const std::string &s, const std::string &p) const;

} // End of namespace Common

#endif //OPENAWE_INIFILE_H
