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

#ifndef OPENAWE_TERRAIN_H
#define OPENAWE_TERRAIN_H

#include "src/graphics/model.h"

namespace Graphics {

/*!
 * \brief Class for handling an open world terrain
 *
 * This class is responsible for handling a rather big terrain ustilized in both Remedy games
 */
class Terrain : public Model {
public:
	/*!
	 * Construct a new terrain using some limits
	 * \param maxPolygons The maximum number of polygons this terrain can contain
	 * \param maxMaps The maximum number of  displacement maps used in this terrain
	 */
	Terrain(
		unsigned int maxPolygons = 58000,
		unsigned int maxMaps = 8500
	);

	/*!
	 * Load a new terrain data file into the terrain. It will be combined with existing data
	 * @param terrainDataFile The terrain data file to load
	 */
	void loadTerrainData(Common::ReadStream *terrainDataFile);

private:
	float _blendScale;
	std::vector<TexturePtr> _textures;
	TexturePtr _blendMap;
	TexturePtr _geoNormalMap;
	std::vector<Common::UUID> _blendMaps;
	std::vector<glm::vec2> _mapCoords;
};

}

#endif //OPENAWE_TERRAIN_H
