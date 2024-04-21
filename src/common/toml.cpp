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

#include <spdlog/spdlog.h>
#include <toml.hpp>

#include "src/common/toml.h"
#include "src/common/types.h"

namespace Common {

static void readValue(TOML::PropertyMap &props, const std::string &id, const toml::value &value);
static void readTable(TOML::PropertyMap &props, const std::string &id, const toml::table &table);

static TOML::PropertyArray readArray(const toml::array &arr) {
	TOML::PropertyArray array;
	for (const auto &value: arr) {
		switch (value.type()) {
			case toml::value_t::boolean:
				array.emplace_back(value.as_boolean());
				break;

			case toml::value_t::integer:
				array.emplace_back(value.as_integer());
				break;

			case toml::value_t::floating:
				array.emplace_back(value.as_floating());
				break;

			case toml::value_t::string:
				array.emplace_back(value.as_string());
				break;

			default:
				break; // We ignore inner arrays and more advanced stuff since we do not need it
		}
	}

	return array;
}

static void readValue(TOML::PropertyMap &props, const std::string &id, const toml::value &value) {
	switch (value.type()) {
		case toml::value_t::table:
			readTable(props, id, value.as_table());
			break;

		case toml::value_t::boolean:
			props[id] = value.as_boolean();
			break;

		case toml::value_t::integer:
			props[id] = static_cast<long>(value.as_integer());
			break;

		case toml::value_t::floating:
			props[id] = value.as_floating();
			break;

		case toml::value_t::string:
			props[id] = value.as_string();
			break;

		case toml::value_t::array:
			props[id] = readArray(value.as_array());
			break;

		default:
			break;
	}
}

template<typename T>
static long extractInt(const T &v) {
	return std::visit(Common::Overloaded{
		[](const long &v) { return v;},
		[](const double &v) { return static_cast<long>(v);},
		[](const bool &v) { return v ? 1l : 0l;},
		[](const std::string &v) { return std::stol(v);},
		[](const std::vector<long> &v) { return v.empty() ? 0 : v.front();},
		[](const TOML::PropertyArray &v) { return static_cast<long>(v.size());},
	}, v);
}

template<typename T>
static double extractFloat(const T &v) {
	return std::visit(Common::Overloaded{
			[](const long &v) { return static_cast<double>(v);},
			[](const double &v) { return v;},
			[](const bool &v) { return v ? 1.0 : 0.0;},
			[](const std::string &v) { return std::stod(v);},
			[](const TOML::PropertyArray &v) { return static_cast<double>(v.size());},
	}, v);
}

static void readTable(TOML::PropertyMap &props, const std::string &id, const toml::table &table) {
	for (const auto &[k, v] : table) {
		readValue(props, fmt::format("{}.{}", id, k), v);
	}
}

void TOML::read(ReadStream &toml) {
	std::stringstream content;
	while(!toml.eos()) {
		content << toml.readLine() << std::endl;
	}

	const auto root = toml::parse(content);
	for (const auto &[k, v] : root.as_table()) {
		readTable(_props, k, v.as_table());
	}
}

long TOML::getInt(const std::string &id) {
	const auto &v = _props.at(id);
	return extractInt(v);
}

double TOML::getFloat(const std::string &id) {
	const auto &v = _props.at(id);
	return extractFloat(v);
}

bool TOML::getBool(const std::string &id) {
	const auto &v = _props.at(id);
	return std::visit(Common::Overloaded{
		[](const long &v) { return v != 0;},
		[](const double &v) { return v != 0.0;},
		[](const bool &v) { return v;},
		[](const std::string &v) { return !v.empty();},
		[](const PropertyArray &v) { return !v.empty();},
	}, v);
}

std::string TOML::getString(const std::string &id) {
	const auto &v = _props.at(id);
	return std::visit(Common::Overloaded{
		[](const long &v) { return std::to_string(v);},
		[](const double &v) { return std::to_string(v);},
		[](const bool &v) { return std::to_string(v);},
		[](const std::string &v) { return v;},
		[](const PropertyArray &v) { return std::string("");},
	}, v);
}

std::vector<long> TOML::getIntArray(const std::string &id) {
	const auto &arr = std::get<TOML::PropertyArray>(_props.at(id));
	std::vector<long> values;
	for (const auto &item : arr) {
		values.emplace_back(extractInt(item));
	}
	return values;
}

std::vector<double> TOML::getFloatArray(const std::string &id) {
	const auto &arr = std::get<TOML::PropertyArray>(_props.at(id));
	std::vector<double> values;
	for (const auto &item : arr) {
		values.emplace_back(extractFloat(item));
	}
	return values;
}

} // End of namespace Common