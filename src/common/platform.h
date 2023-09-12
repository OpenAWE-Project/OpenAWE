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

#ifndef COMMON_PLATFORM_H
#define COMMON_PLATFORM_H

#include <string>

namespace Common {

/*!
 * An enumeration of all possible languages
 */
enum Language {
	kEnglish,
	kJapanese,
	kGerman,
	kFrench,
	kSpanish,
	kItalian,
	kKorean,
	kPolish,
	kChinese,
	kRussian,
	kCzech,
	kHungarian,

	kUnrecognized
};

/*!
 * Get the currently configured system language. If it is not supported or unrecognized, return
 * english as standard
 *
 * \return The currently configured system language
 */
Language getSystemLanguage();

/*!
 * Get the home directory of the current user depending on the operating system
 *
 * \return The current users home directory
 */
std::string getHomeDirectory();

/*!
 * Get the directory for user data for the current user
 *
 * \return The current users user data directory
 */
std::string getUserDataDirectory();

}

#endif // COMMON_PLATFORM_H
