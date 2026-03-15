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

	atm.skip(12); // Seems to be always zero
	_betaRayleigh = atm.read<glm::vec3>();
	_betaMie = atm.read<glm::vec3>();
	_hgFactor = atm.readIEEEFloatLE();

	// Unknown list of vec3s
	atm.skip(256 * sizeof(glm::vec3));

	_atmosphericLUT.reset(atm.readStream());
}

Common::ReadStream &ATMFile::getAtmosphericLUT() {
	return *_atmosphericLUT;
}

const glm::vec3 & ATMFile::getRayleigh() const {
	return _betaRayleigh;
}

const glm::vec3 & ATMFile::getMie() const {
	return _betaMie;
}

float ATMFile::getHGFactor() const {
	return _hgFactor;
}

} // End of namespace AWE