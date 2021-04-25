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

#ifndef OPENAWE_FOLIAGEDATAFILE_H
#define OPENAWE_FOLIAGEDATAFILE_H

#include <glm/glm.hpp>

#include "src/common/readstream.h"
#include "src/common/types.h"

namespace AWE{

/*!
 * \brief Class for parsing foliage data files
 *
 * This class is responsible for parsing foliage data files which hold information about, where certain foliages are
 * placed. It also contains information about the bounding box of all foliages.
 */
class FoliageDataFile {
public:
	/*!
	 * An instance of a foliage positioned on the scene
	 */
	struct Instance {
		glm::vec3 position;
		uint8_t foliageId;
	};

	/*!
	 * Parse a foliage data file by giving the corresponding stream as parameter
	 *
	 * \param foliageData the stream of the foliage data file
	 */
	FoliageDataFile(Common::ReadStream &foliageData);

	const std::vector<std::string> &getFoliages() const;

	const std::vector<Instance> &getInstances() const;

	const Common::BoundBox &getAabb() const;

private:
	std::vector<std::string> _foliages;
	std::vector<Instance> _instances;
	Common::BoundBox _aabb;
};

}

#endif //OPENAWE_FOLIAGEDATAFILE_H
