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

#ifndef COMMON_UUID_H
#define COMMON_UUID_H

#include <cstdint>

#include <string>

#include "src/common/types.h"

namespace Common {

class UUID {
public:
	UUID();
	UUID(const UUID&);

	static UUID createFromString(const std::string &);

	static UUID generateRandom();
	static UUID generateNil();

	bool isNil() const;

	bool operator==(const UUID &rhs) const;
	bool operator!=(const UUID &rhs) const;

	bool operator<(const UUID &rhs) const;

private:
	byte _id[16];
};

}

#endif // COMMON_UUID_H
