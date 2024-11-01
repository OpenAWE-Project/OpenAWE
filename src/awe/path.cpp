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

#include "src/common/strutil.h"

#include "src/awe/path.h"

namespace AWE {

std::string getNormalizedPath(const std::string &path) {
	std::string lower = Common::toLower(path);
	if (Common::startsWith(lower, "runtimedata\\pc")) 
		lower = Common::replace(lower, "runtimedata\\pc", "d:");
	lower = Common::replace(lower, "\\", "/");
	if (Common::startsWith(lower, "d:/data/")) 
		lower = Common::replace(lower, "d:/data/", "");
	return lower;
}

std::string removeDrivePrefix(const std::string &path) {
	if (path.size() > 2 && path[1] == ':' && path[2] == '/') {
		return path.substr(3);
	}
	return path;
}

} // End of namespace AWE
