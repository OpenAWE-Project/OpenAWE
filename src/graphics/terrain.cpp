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

#include <cstring>

#include <fstream>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/writefile.h"
#include "src/common/memwritestream.h"

#include "src/awe/terraindatafile.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/terrain.h"
#include "src/graphics/textureman.h"
#include "src/graphics/images/surface.h"
#include "src/common/endianness.h"

namespace Graphics {

Terrain::Terrain(unsigned int maxPolygons, unsigned int maxMaps) {
	const unsigned int textureSize = std::ceil(std::sqrt((maxPolygons + maxMaps * 4) * 64));
	const unsigned int alignedTextureSize = textureSize + (128 - textureSize % 128);

	unsigned int e = 0;
	while (std::exp2(e) < alignedTextureSize) {
		++e;
	}

	const auto potTextureSize = static_cast<unsigned int>(std::exp2(e));
	_blendScale = static_cast<float>(potTextureSize) / static_cast<float>(alignedTextureSize);

	_blendMap = GfxMan.createEmptyTexture2D(kA1RGB5, alignedTextureSize, alignedTextureSize);
	_geoNormalMap = GfxMan.createEmptyTexture2D(kA1RGB5, alignedTextureSize, alignedTextureSize);
}

void Terrain::loadTerrainData(Common::ReadStream *terrainDataFile) {
	AWE::TerrainDataFile terrainData(*terrainDataFile);
	std::vector<uint16_t> indices;
	const auto &vertices = terrainData.getVertices();
	const auto &tilesets = terrainData.getTilesets();
	const auto &blendMaps = terrainData.getBlendMaps();

	for (const auto &texture : terrainData.getTextures()) {
		_textures.emplace_back(TextureMan.getTexture(texture));
	}

	for (const auto &polygon : terrainData.getPolygons()) {
		Common::DynamicMemoryWriteStream vertexData(true);

		float minBlendS = FLT_MAX;
		float minBlendT = FLT_MAX;
		float maxBlendS = std::numeric_limits<float>::min();
		float maxBlendT = std::numeric_limits<float>::min();

		for (int i = 0; i < polygon.indices.size(); ++i) {
			const auto index = polygon.indices[i];
			const auto vertex = vertices[index];

			vertexData.writeIEEEFloatLE(vertex.position.x);
			vertexData.writeIEEEFloatLE(vertex.position.y);
			vertexData.writeIEEEFloatLE(vertex.position.z);

			vertexData.writeIEEEFloatLE(vertex.normal.x);
			vertexData.writeIEEEFloatLE(vertex.normal.y);
			vertexData.writeIEEEFloatLE(vertex.normal.z);

			for (int j = 0; j < 4; ++j) {
				const auto uv = polygon.texCoords[j][i];

				vertexData.writeIEEEFloatLE(uv.s);
				vertexData.writeIEEEFloatLE(uv.t);

				//spdlog::info("{} {} {} {}", j, i, uv.s, uv.t);

				if (j == 3) {
					minBlendS = std::min(uv.s, minBlendS);
					minBlendT = std::min(uv.t, minBlendT);
					maxBlendS = std::max(uv.s, maxBlendS);
					maxBlendT = std::max(uv.t, maxBlendT);
				}
			}
		}

		Mesh::PartMesh partMesh;
		partMesh.vertexData = GfxMan.createBuffer(
			vertexData.getData(),
			vertexData.getLength(),
			kVertexBuffer
		);

		const std::vector<VertexAttribute> attributes = {
			{kPosition,       kVec3F},
			{kNormal,         kVec3F},
			{kTexCoord0,      kVec4F},
			{kTexCoord1,      kVec4F},
		};

		_mapCoords.emplace_back(minBlendS, minBlendT);

		const auto tileset = tilesets[polygon.tilesetId];
		const std::vector<Material::Attribute> materialAttributes = {
			Material::Attribute("g_sColorMaps[0]",  _textures[tileset.colorTiles[0]]),
			Material::Attribute("g_sColorMaps[1]",  _textures[tileset.colorTiles[1]]),
			Material::Attribute("g_sColorMaps[2]",  _textures[tileset.colorTiles[2]]),
			Material::Attribute("g_sNormalMaps[0]", _textures[tileset.normalTiles[0]]),
			Material::Attribute("g_sNormalMaps[1]", _textures[tileset.normalTiles[1]]),
			Material::Attribute("g_sNormalMaps[2]", _textures[tileset.normalTiles[2]]),
			Material::Attribute("g_sBlendMap",      _blendMap),
			Material::Attribute("g_vTexCoordScale1",glm::vec4(16.0)),
			Material::Attribute("g_vTexCoordScale2",glm::vec4(glm::vec2(16.0), glm::vec2(_blendScale)))
		};

		for (const auto &stage: {"material"}) {
			partMesh.vertexAttributes[stage] = GfxMan.createAttributeObject(
				"terrain", stage,
				attributes,
				partMesh.vertexData
			);
		}
		partMesh.renderType = Mesh::kTriangleFan;
		partMesh.material = Material("terrain", {"material"}, materialAttributes);
		partMesh.material.setCullMode(Material::kBack);
		partMesh.offset = 0;
		partMesh.length = polygon.indices.size();

		_mesh->addPartMesh(partMesh);
	}

	for (const auto &blendMap : terrainData.getBlendMaps()) {
		const auto mapCoord = _mapCoords[blendMap.id];

		const auto xoffset = static_cast<unsigned int>(mapCoord.s * 4096.0);
		const auto yoffset = static_cast<unsigned int>(mapCoord.t * 4096.0);

		Surface blendSurface(blendMap.size, blendMap.size, kA1RGB5);
		std::memcpy(blendSurface.getData(), blendMap.data.data(), blendMap.data.size() * sizeof(uint16_t));
		_blendMap->load(xoffset, yoffset, blendSurface);
	}
}

}