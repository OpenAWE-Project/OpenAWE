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

#ifndef AWE_STRUTIL_H
#define AWE_STRUTIL_H

#include <string>
#include <vector>
#include <regex>

#include "src/common/exception.h"
#include "src/common/types.h"

#if LIB_CPP
#include <boost/charconv.hpp>
#else
#include <charconv>
#endif

namespace Common {

/*!
 * Change all ascii alpha letters from the string str to lower case letters
 * \param str the string to change
 * \return the new lower case string
 */
constexpr std::string toLower(std::string_view str) {
	std::string result;
	for (const auto &c: str) {
		result += (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	}

	return result;
}

/*!
 * Change all ascii alpha letters from the string str to upper case letters
 * \param str the string to change
 * \return the new upper case string
 */
constexpr std::string toUpper(std::string_view str) {
	std::string result;
	for (const auto &c: str) {
		result += (c >= 'a' && c <= 'z') ? c + ('A' - 'a') : c;
	}

	return result;
}


/*!
 * check if a string contains a certain substring
 * \param str the string to search a substring in
 * \param s the substring to search for
 * \return if the substring is contained in the string
 */
bool contains(const std::string &str, const std::string &s);

/*!
 * check if a string starts with a certain substring
 * \param str the string to search a substring in
 * \param substr the substring to search for
 * \return if the string starts with the substring
 */
bool startsWith(const std::string &str, const std::string &substr);

/*!
 * check if a string ends with a certain substring
 * \param str the string to search a substring in
 * \param s the substring to search for
 * \return if the string ends with the substring
 */
bool endsWith(const std::string &str, const std::string &substr);

/*!
 * substitutes one substring for another in a string
 * \param str the string to replace in
 * \param what the substring to search for
 * \param with the subtring to replace "what" with
 * \return string with replaced occurencies
 */
std::string replace(const std::string &str, const std::string &what, const std::string &with);

/*!
 * Split a string by a given regex
 * \param str the string to split
 * \param split the regex to define the splits
 * \return a vector of split strings
 */
std::vector<std::string> split(const std::string &str, const std::regex &split);

/*!
 * Extract a certain pattern from a string
 * \param str The string to search for a pattern
 * \param pattern The pattern to search for
 * \return The extracted pattern string
 */
std::string extract(const std::string &str, const std::regex &pattern);

/*!
 * Parse the next number literal from a string and throw an exception if a number cannot be parsed. The parsing is
 * independent from any currently loaded locale
 *
 * \tparam T The type of the number to read, needs to be an integral or a floating point number
 * \param str The string from which to parse the number
 * \return The successfully parsed number
 */
template<typename T>
requires std::is_integral_v<T> || std::is_floating_point_v<T>
static T parse(const std::string &str) {
	T v;

#if LIB_CPP
	const auto [_, ec] = boost::charconv::from_chars(str.c_str(), str.c_str() + str.size(), v);
#else
	const auto [_, ec] = std::from_chars(str.c_str(), str.c_str() + str.size(), v);
#endif
	if (ec != std::errc())
		throw CreateException("Error when parsing {} \"{}\": {}", typeid(T).name(), str, static_cast<int>(ec));

	return v;
}

}

#endif //AWE_STRUTIL_H
