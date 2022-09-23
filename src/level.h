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

#ifndef OPENAWE_LEVEL_H
#define OPENAWE_LEVEL_H

#include <glm/glm.hpp>

#include "src/common/readstream.h"

#include "src/graphics/terrain.h"

#include "src/objectcollection.h"

class Level : public ObjectCollection {
public:
	Level(entt::registry &registry, const std::string &id, const std::string &world);

private:
	std::vector<glm::u32vec2> loadCellInfo(Common::ReadStream *cid) const;
	void loadTerrainData(
		Common::ReadStream *terrainDataLD,
		Common::ReadStream *terrainDataHD
	);

	std::vector<std::unique_ptr<Graphics::Terrain>> _terrains;
	const std::string _id, _world;
};


#endif //OPENAWE_LEVEL_H
