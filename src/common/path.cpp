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
#include "src/common/path.h"

using std::string;

namespace Common {

string getNormalizedPath(const string &path) {
	string lower = Common::toLower(path);
	uint64_t pos = string::npos;
	// replace runtimedata\pc with d:
	if (lower.find("runtimedata\\pc") == 0) lower.replace(0, 14, "d:");
	// replace \ with /
	while ((pos = lower.find("\\")) != string::npos) lower.replace(pos, 1, "/", 1);
	// remove d:/data from the start of the path
	if (lower.find("d:/data/") == 0) lower.replace(0, 8, "");
	return lower;
}

} // End of namespace Common
