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

#include <cstring>

#include <random>
#include <algorithm>
#include <functional>
#include <regex>
#include <chrono>

#include "uuid.h"

#if OS_MACOS || OS_IOS
#	include <CoreFoundation/CFUUID.h>
#endif

namespace Common {

UUID::UUID(const UUID &uuid) {
	std::memcpy(_id, uuid._id, 16);
}

UUID UUID::createFromString(const std::string &str) {
	UUID uuid;

#if OS_MACOS || OS_IOS
	auto newId = CFUUIDCreateFromString(str);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	uuid._id[0] = bytes.byte0;
	uuid._id[1] = bytes.byte1;
	uuid._id[2] = bytes.byte2;
	uuid._id[3] = bytes.byte3;
	uuid._id[4] = bytes.byte4;
	uuid._id[5] = bytes.byte5;
	uuid._id[6] = bytes.byte6;
	uuid._id[7] = bytes.byte7;
	uuid._id[8] = bytes.byte8;
	uuid._id[9] = bytes.byte9;
	uuid._id[10] = bytes.byte10;
	uuid._id[11] = bytes.byte11;
	uuid._id[12] = bytes.byte12;
	uuid._id[13] = bytes.byte13;
	uuid._id[14] = bytes.byte14;
	uuid._id[15] = bytes.byte15;
#else
	std::string rawStr = std::regex_replace(str, std::regex("[^0-9a-fA-F]"), "");

	if (rawStr.length() != 32)
		throw std::runtime_error("Invalid string");

	for (int i = 0; i < 16; ++i) {
		uuid._id[i] = std::stoi(rawStr.substr(i * 2, 2), nullptr, 16);
	}
#endif

	return uuid;
}

UUID UUID::generateRandom() {
	UUID uuid;
#if OS_MACOS || OS_IOS
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	uuid._id[0] = bytes.byte0;
	uuid._id[1] = bytes.byte1;
	uuid._id[2] = bytes.byte2;
	uuid._id[3] = bytes.byte3;
	uuid._id[4] = bytes.byte4;
	uuid._id[5] = bytes.byte5;
	uuid._id[6] = bytes.byte6;
	uuid._id[7] = bytes.byte7;
	uuid._id[8] = bytes.byte8;
	uuid._id[9] = bytes.byte9;
	uuid._id[10] = bytes.byte10;
	uuid._id[11] = bytes.byte11;
	uuid._id[12] = bytes.byte12;
	uuid._id[13] = bytes.byte13;
	uuid._id[14] = bytes.byte14;
	uuid._id[15] = bytes.byte15;
#else

	do {
		std::generate_n(uuid._id, 16, std::mt19937(std::chrono::system_clock::now().time_since_epoch().count()));
	} while (uuid.isNil());
#endif
	return uuid;
}

UUID UUID::generateNil() {
	UUID uuid;
	std::memset(uuid._id, 0, 16);
	return uuid;
}

bool UUID::isNil() const {
	for (int i = 0; i < 16; ++i) {
		if (_id[i] != 0)
			return false;
	}
	return true;
}

UUID::UUID() {
}

bool UUID::operator==(const UUID &rhs) const {
	if (isNil() || rhs.isNil())
		return false;

	return std::memcmp(_id, rhs._id, 16) == 0;
}

bool UUID::operator!=(const UUID &rhs) const {
	return !(rhs == *this);
}

bool UUID::operator<(const UUID &rhs) const {
	for (int i = 0; i < 16; ++i) {
		if (_id[i] < rhs._id[i])
			return true;
		else if (_id[i] > rhs._id[i])
			return false;
	}
	return false;
	//return memcmp(_id, rhs._id, 16) < 0;
}

}