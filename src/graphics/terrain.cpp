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

#include "src/common/exception.h"
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
    setLabel("Terrain");

	const unsigned int textureSize = std::ceil(std::sqrt((maxPolygons + maxMaps * 4) * 64));
	const unsigned int alignedTextureSize = textureSize + (128 - textureSize % 128);

	unsigned int e = 0;
	while (std::exp2(e) < alignedTextureSize) {
		++e;
	}

	_potTextureSize = static_cast<float>(std::exp2(e));
	_blendScale = static_cast<float>(_potTextureSize) / static_cast<float>(alignedTextureSize);
	_bigMapSize = 16.0f / (static_cast<float>(_potTextureSize));

	_bigMapCoords.resize(maxPolygons);
	unsigned int bigMapsHeight = 0;
	const unsigned int slotsInRow = alignedTextureSize / 16;
	for (unsigned int i = 0; i < _bigMapCoords.size(); ++i) {
		_bigMapCoords[i].x = static_cast<float>(i % slotsInRow) * _bigMapSize;
		_bigMapCoords[i].y = static_cast<float>(i / slotsInRow) * _bigMapSize;
	}

	std::reverse(_bigMapCoords.begin(), _bigMapCoords.end());

	_blendMap = GfxMan.createEmptyTexture2D(kA1RGB5, alignedTextureSize, alignedTextureSize, "Terrain Blend Map");
	_geoNormalMap = GfxMan.createEmptyTexture2D(kA1RGB5, alignedTextureSize, alignedTextureSize, "Terrain Geonormal Map");
}

void Terrain::loadTerrainData(Common::ReadStream &ld) {
	std::vector<glm::vec2> mapCoords;
	loadTerrainData(&ld, mapCoords);
}

void Terrain::loadTerrainData(Common::ReadStream &ld, Common::ReadStream &hd) {
	std::vector<glm::vec2> mapCoords;
	loadTerrainData(&ld, mapCoords);
	loadTerrainData(&hd, mapCoords);
}

void Terrain::loadTerrainData(Common::ReadStream *terrainDataFile, std::vector<glm::vec2> &mapCoords) {
	AWE::TerrainDataFile terrainData(*terrainDataFile);
	std::vector<uint16_t> indices;
	const auto &vertices = terrainData.getVertices();
	const auto &tilesets = terrainData.getTilesets();
	const auto &blendMaps = terrainData.getBlendMaps();

	std::vector<TexturePtr> localTextures;
	for (const auto &texture : terrainData.getTextures()) {
		localTextures.emplace_back(TextureMan.getTexture(texture));
		_textures.emplace_back(TextureMan.getTexture(texture));
	}

	std::map<unsigned int, std::vector<uint16_t>> tilesetIndices;
	Common::DynamicMemoryWriteStream vertexData(true);

	unsigned int currentIndex = 0;
	for (const auto &polygon : terrainData.getPolygons()) {
		if (_bigMapCoords.empty())
			throw CreateException("Exceeded maximum number of big terrain maps");

		const auto atlasPosition = _bigMapCoords.back();
		_bigMapCoords.pop_back();

		const float halfPixel = 1.0f / (2.0f * _potTextureSize);
		const std::vector<glm::vec2> atlasPositions = {
			atlasPosition + glm::vec2(halfPixel),
			atlasPosition + glm::vec2(_bigMapSize - halfPixel, halfPixel),
			atlasPosition + glm::vec2(_bigMapSize - halfPixel),
			atlasPosition + glm::vec2(halfPixel, _bigMapSize - halfPixel),
		};

		for (unsigned int i = 0; i < polygon.indices.size(); ++i) {
			const auto index = polygon.indices[i];
			const auto vertex = vertices[index];

			vertexData.writeIEEEFloatLE(vertex.position.x);
			vertexData.writeIEEEFloatLE(vertex.position.y);
			vertexData.writeIEEEFloatLE(vertex.position.z);

			vertexData.writeIEEEFloatLE(vertex.normal.x);
			vertexData.writeIEEEFloatLE(vertex.normal.y);
			vertexData.writeIEEEFloatLE(vertex.normal.z);

			for (int j = 0; j < 3; ++j) {
				const auto uv = polygon.texCoords[j][i];

				vertexData.writeIEEEFloatLE(uv.s);
				vertexData.writeIEEEFloatLE(uv.t);
			}

			vertexData.writeIEEEFloatLE(atlasPositions[i].s);
			vertexData.writeIEEEFloatLE(atlasPositions[i].t);
		}

		tilesetIndices[polygon.tilesetId].emplace_back(currentIndex);
		tilesetIndices[polygon.tilesetId].emplace_back(currentIndex + 1);
		tilesetIndices[polygon.tilesetId].emplace_back(currentIndex + 2);
		if (polygon.indices.size() == 4) {
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndex);
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndex + 2);
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndex + 3);
		}

		currentIndex += polygon.indices.size();

		mapCoords.emplace_back(atlasPosition);
	}

	const auto vertexBuffer = GfxMan.createBuffer(
		vertexData.getData(),
		vertexData.getLength(),
		kVertexBuffer
	);
	
	currentIndex = _indices.size();
	for (const auto &tilesetId: tilesetIndices) {
		const auto tileset = tilesets[tilesetId.first];
		const auto indices = tilesetId.second;

		for (const auto &index: indices) {
			_indices.emplace_back(index);
		}

		const std::vector<Material::Uniform> materialAttributes = {
			Material::Uniform("g_sColorMaps[0]",   localTextures[tileset.colorTiles[0]]),
			Material::Uniform("g_sColorMaps[1]",   localTextures[tileset.colorTiles[1]]),
			Material::Uniform("g_sColorMaps[2]",   localTextures[tileset.colorTiles[2]]),
			Material::Uniform("g_sNormalMaps[0]",  localTextures[tileset.normalTiles[0]]),
			Material::Uniform("g_sNormalMaps[1]",  localTextures[tileset.normalTiles[1]]),
			Material::Uniform("g_sNormalMaps[2]",  localTextures[tileset.normalTiles[2]]),
			Material::Uniform("g_sBlendMap",       _blendMap),
			Material::Uniform("g_vTexCoordScale1", glm::vec4(16.0)),
			Material::Uniform("g_vTexCoordScale2", glm::vec4(glm::vec2(16.0), glm::vec2(_blendScale)))
		};

		const std::vector<VertexAttribute> attributes = {
			{kPosition,  kVec3F},
			{kNormal,    kVec3F},
			{kTexCoord0, kVec4F},
			{kTexCoord1, kVec4F},
		};

		Mesh::PartMesh partMesh;

		for (const auto &stage: {"material", "depth"}) {
			partMesh.vertexAttributes[stage] = GfxMan.createAttributeObject(
				"terrain", stage, 0,
				attributes,
				vertexBuffer
			);
		}
		partMesh.renderType = Mesh::kTriangles;
		partMesh.vertexData = vertexBuffer;
		partMesh.material = Material("terrain", {"material", "depth"}, materialAttributes);
		partMesh.material.setCullMode(Material::kBack);
		partMesh.offset = currentIndex * 2;
		partMesh.length = indices.size();

		currentIndex += indices.size();

		_mesh->addPartMesh(partMesh);
	}

	for (const auto &blendMap : terrainData.getBlendMaps()) {
		const auto mapCoord = mapCoords[blendMap.id];

		const auto xoffset = static_cast<unsigned int>(mapCoord.s * _potTextureSize);
		const auto yoffset = static_cast<unsigned int>(mapCoord.t * _potTextureSize);

		assert(xoffset % blendMap.size == 0 && yoffset % blendMap.size == 0);

		Surface blendSurface(blendMap.size, blendMap.size, kA1RGB5);
		std::memcpy(
			blendSurface.getData(),
			blendMap.data.data(),
			blendMap.data.size() * sizeof(uint16_t)
		);
		_blendMap->load(xoffset, yoffset, blendSurface);
	}
}

void Terrain::finalize() {
	Common::DynamicMemoryWriteStream indexData(true);
	
	for (const auto &index: _indices) {
		indexData.writeUint16LE(index);
	}
	
	const auto indexBuffer = GfxMan.createBuffer(
		indexData.getData(),
		indexData.getLength(),
		kIndexBuffer
	);

	_mesh->setIndices(indexBuffer);
	_indices.clear();
}

}