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
#include <regex>

#include <zlib.h>

#include "src/common/exception.h"

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

bool startsWith(const std::string &str, const std::string &substr) {
	return str.find(substr) == 0;
}

bool endsWith(const std::string &str, const std::string &substr) {
	return str.rfind(substr) == (str.length() - substr.length());
}

std::string replace(const std::string &str, const std::string &what, const std::string &with) {
	if (contains(with, what))
		throw CreateException("replace(): replacing {} with {} may create an infinite loop", what, with);
	
	std::string result = str;
	unsigned long long position = std::string::npos;

	while ((position = result.find(what)) != std::string::npos)
		result.replace(position, what.length(), with);

	return result;
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

std::string extract(const std::string &str, const std::regex &pattern) {
	std::smatch match;
	if(std::regex_search(str, match, pattern))
		return match[0];
	else
		return "";
}

}
