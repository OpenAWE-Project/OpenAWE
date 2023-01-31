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

namespace Common {

/*!
 * Change all letters from the string str to lower case letters
 * \param str the string to change
 * \return the new lower case string
 */
std::string toLower(std::string str);

/*!
 * Change all letters from the string str to upper case letters
 * \param str the string to change
 * \return the new upper case string
 */
std::string toUpper(std::string str);

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
}

#endif //AWE_STRUTIL_H
