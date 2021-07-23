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

#ifndef AWE_COLLISIONSFILE_H
#define AWE_COLLISIONSFILE_H

#include "src/common/readstream.h"

namespace AWE {

class COLLISIONSFile {
public:
	COLLISIONSFile(Common::ReadStream &collisions);

	const std::vector<glm::vec3> &getPositions();
	const std::vector<uint16_t> &getIndices();

private:
	std::vector<glm::vec3> _positions;
	std::vector<uint16_t> _indices;
};

} // End of namespace AWE


#endif //OPENAWE_COLLISIONSFILE_H
