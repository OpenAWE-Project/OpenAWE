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

#ifndef OPENAWE_TOML_H
#define OPENAWE_TOML_H

#include <map>
#include <variant>
#include <vector>

#include "src/common/readstream.h"

namespace Common {

/*!
 * \brief Class for reading toml files
 *
 * This class is responsible for reading a toml file and giving access to its information. The toml format is similar to
 * ini files and saves key value pairs organized in sections. This class doesn't support all possible abilities of toml
 * files for simplicity. Propertys can be accessed by using the section names in the hierarchy separated by points.
 */
class TOML {
public:
	typedef std::vector<std::variant<
	        long,
			double,
			bool,
			std::string
	>> PropertyArray;

	typedef std::variant<
			long,
			double,
			bool,
			std::string,
			PropertyArray
	> PropertyValue;

	typedef std::map<
			std::string,
			PropertyValue
	> PropertyMap;

	/*!
	 * Read a toml file from a read stream and parse it into this TOML object
	 * \param toml The tomls stream to read
	 */
	void read(Common::ReadStream &toml);

	/*!
	 * Read a property as integer value with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to an integer
	 */
	long getInt(const std::string &id);

	/*!
	 * Read a property as float value with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to a float
	 */
	double getFloat(const std::string &id);

	/*!
	 * Read a property as bool value with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to a bool
	 */
	bool getBool(const std::string &id);

	/*!
	 * Read a property as string with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to a string
	 */
	std::string getString(const std::string &id);

	/*!
	 * Read a property as integer array value with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to an integer array
	 */
	std::vector<long> getIntArray(const std::string &id);

	/*!
	 * Read a property as float array value with an id string
	 * \param id the id of the value to read
	 * \return The read value converted to a float array
	 */
	std::vector<double> getFloatArray(const std::string &id);

	/*!
	 * Read a property as glm vector value with an id string converting a float array
	 * \param id the id of the value to read
	 * \return The read value converted to a glm vector
	 */
	template<typename T> T getVec(const std::string &id) {
		const auto arr = getFloatArray(id);
		glm::vec3 v;

		T vec;
		for (int i = 0; i < vec.length(); ++i)
			vec[i] = arr[i];

		return vec;
	}

private:
	PropertyMap _props;
};

} // End of namespace Common

#endif //OPENAWE_TOML_H
