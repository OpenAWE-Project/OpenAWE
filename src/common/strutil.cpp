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

#include <sstream>
#include <iostream>

#include <zlib.h>

#include "strutil.h"

namespace Common {

std::string toLower(std::string str) {
	std::stringstream ss;

	for (const auto &c : str) {
		ss << static_cast<char>(std::tolower(c));
	}

	return ss.str();
}

std::string toUpper(std::string str) {
	std::stringstream ss;

	for (const auto &c : str) {
		ss << static_cast<char>(std::toupper(c));
	}

	return ss.str();
}

bool contains(const std::string &str, const std::string &s) {
	return str.find(s) != std::string::npos;
}

std::vector<std::string> split(const std::string &str, const std::regex &split) {
	std::vector<std::string> result;

	std::copy(
			std::sregex_token_iterator(str.begin(), str.end(), split, -1),
			std::sregex_token_iterator(),
			std::back_inserter(result)
	);

	return result;
}

std::string getNormalizedPath(const std::string &path) {
	std::string lower = Common::toLower(path);
	uint64_t pos = std::string::npos;
	while ((pos = lower.find("\\")) != std::string::npos) lower.replace(pos, 1, "/", 1);
	return lower;
}

}
