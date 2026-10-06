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

#ifndef OPENAWE_TERRAINFACTORY_H
#define OPENAWE_TERRAINFACTORY_H

#include <vector>

#include <glm/glm.hpp>

#include "src/common/memwritestream.h"
#include "src/common/readstream.h"

#include "src/graphics/terrain.h"
#include "src/graphics/texture.h"

namespace Graphics {

class TerrainFactory {
public:
	/*!
	 * Construct a new terrain factory using some limits
	 * \param maxPolygons The maximum number of polygons this terrain can contain
	 * \param maxMaps The maximum number of  displacement maps used in this terrain
	 */
	TerrainFactory(
		unsigned int maxPolygons = 58000,
		unsigned int maxMaps = 8500
	);

	void loadTerrainData(Common::ReadStream &ld);
	void loadTerrainData(Common::ReadStream &ld, Common::ReadStream &hd);

	/*!
	 * Load a new terrain data file into the terrain. It will be combined with existing data
	 * \param terrainDataFile The terrain data file to load
	 */
	void loadTerrainData(Common::ReadStream *terrainDataFile, std::vector<glm::vec2> &mapCoords);

	Terrain* createTerrain();

private:
	struct TerrainTile {
		uint32_t indexOffset;
		uint32_t indexCount;
		Material material;
		Common::BoundSphere boundSphere;
	};

	std::size_t numVertices{};
	std::size_t indicesOffset{};

	float _blendScale;
	float _potTextureSize;
	float _bigMapSize;

	std::vector<glm::vec2> _bigMapCoords;
	std::vector<uint16_t> _indices;

	TexturePtr _blendMap;
	TexturePtr _geoNormalMap;
	BufferPtr _materialBuffer;
	std::vector<TexturePtr> _textures;
	std::vector<TerrainTile> _terrainTiles;
	Common::DynamicMemoryWriteStream _vertexData{true};
};

} // End of namespace Graphics

#endif //OPENAWE_TERRAINFACTORY_H
