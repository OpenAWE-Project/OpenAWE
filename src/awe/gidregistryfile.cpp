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

#include <regex>

#include "src/common/strutil.h"
#include "src/common/endianness.h"

#include "gidregistryfile.h"

namespace AWE {

GIDRegistryFile::GIDRegistryFile(Common::ReadStream &gid) {
	const std::regex comma(",");

	while (!gid.eos()) {
		std::string line = gid.readLine();

		std::vector<std::string> split = Common::split(line, comma);
		GID newGid;
		newGid.type = std::stoi(split[0]);
		newGid.id = std::stoul(Common::toLower(split[1]), nullptr, 16);
		newGid.id = Common::swapBytes(newGid.id);

		_strings[newGid] = split[2];
	}
}

std::string GIDRegistryFile::getString(GID gid) const {
	if (gid.type == 0 && gid.id == 0)
		return "";

	return _strings.at(gid);
}

}
