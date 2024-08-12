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

#ifndef OPENAWE_ATMFILE_H
#define OPENAWE_ATMFILE_H

#include <memory>

#include "src/common/readstream.h"

namespace AWE {

/*!
 * \brief Reader for atmosphere files
 *
 * This class handles the loading of atm files, which contain parameters for the atmospheric rendering
 */
class ATMFile {
public:
	ATMFile(Common::ReadStream &atm);

	/*!
	 * Return a stream to the atmospheric lut texture in the atm format
	 *
	 * \return A reference to the atmospheric lut stream in the tex format
	 */
	Common::ReadStream &getAtmosphericLUT();

private:
	std::vector<glm::vec3> _stars;
	std::unique_ptr<Common::ReadStream> _atmosphericLUT;
};

} // End of namespace AWE

#endif //OPENAWE_ATMFILE_H
