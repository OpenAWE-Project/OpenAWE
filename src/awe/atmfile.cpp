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

#include <vector>

#include "src/common/exception.h"

#include "src/awe/atmfile.h"

namespace AWE {

ATMFile::ATMFile(Common::ReadStream &atm) {
	const auto version = atm.readUint32LE();
	if (version != 1)
		throw CreateException("Invalid atmosphere file version. Expected 1, got {}", version);

	glm::vec3 v1, v2, v3;
	v1.x = atm.readIEEEFloatLE();
	v1.y = atm.readIEEEFloatLE();
	v1.z = atm.readIEEEFloatLE();

	v2.x = atm.readIEEEFloatLE();
	v2.y = atm.readIEEEFloatLE();
	v2.z = atm.readIEEEFloatLE();

	v3.x = atm.readIEEEFloatLE();
	v3.y = atm.readIEEEFloatLE();
	v3.z = atm.readIEEEFloatLE();

	_stars.resize(256); // ?
	for (auto &star : _stars) {
		star.x = atm.readIEEEFloatLE();
		star.y = atm.readIEEEFloatLE();
		star.z = atm.readIEEEFloatLE();
	}

	_atmosphericLUT.reset(atm.readStream());
}

} // End of namespace AWE