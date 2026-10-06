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

#include <map>

#include <glm/glm.hpp>

#include "src/common/exception.h"
#include "src/common/memwritestream.h"

#include "src/awe/terraindatafile.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/std140layout.h"
#include "src/graphics/terrainfactory.h"

#include <spdlog/spdlog.h>

#include "src/graphics/images/surface.h"

namespace Graphics {

struct TerrainMaterial {
	glm::vec4 g_vTexCoordScale1;
	glm::vec4 g_vTexCoordScale2;
};

TerrainFactory::TerrainFactory(unsigned int maxPolygons, unsigned int maxMaps) {
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
	_geoNormalMap = GfxMan.createEmptyTexture2D(kA1RGB5, alignedTextureSize, alignedTextureSize,
	                                            "Terrain Geonormal Map");

	_materialBuffer = GfxMan.createEmptyBuffer(kUniformBuffer, false);

	TerrainMaterial material;
	material.g_vTexCoordScale1 = glm::vec4(16.0);
	material.g_vTexCoordScale2 = glm::vec4(glm::vec2(16.0), glm::vec2(_blendScale));

	_materialBuffer->write(material);
}

void TerrainFactory::loadTerrainData(Common::ReadStream &ld) {
	std::vector<glm::vec2> mapCoords;
	loadTerrainData(&ld, mapCoords);
}

void TerrainFactory::loadTerrainData(Common::ReadStream &ld, Common::ReadStream &hd) {
	std::vector<glm::vec2> mapCoords;
	loadTerrainData(&ld, mapCoords);
	loadTerrainData(&hd, mapCoords);
}

void TerrainFactory::loadTerrainData(Common::ReadStream *terrainDataFile, std::vector<glm::vec2> &mapCoords) {
	AWE::TerrainDataFile terrainData(*terrainDataFile);
	std::vector<uint16_t> indices;
	const auto &vertices = terrainData.getVertices();
	const auto &tilesets = terrainData.getTilesets();

	std::vector<TexturePtr> localTextures;
	for (const auto &texture: terrainData.getTextures()) {
		localTextures.emplace_back(TextureMan.getTexture(texture));
		_textures.emplace_back(TextureMan.getTexture(texture));
	}

	std::map<unsigned int, std::vector<uint16_t>> tilesetIndices;
	std::map<unsigned int, Common::BoundSphere> tilesetBoundSphere;

	/*const auto indexOffset = numVertices;
	auto currentIndex = indexOffset;*/
	// spdlog::info("currentIndex: {}", currentIndex);
	for (const auto &polygon: terrainData.getPolygons()) {
		const auto currentIndexOffset = numVertices;
		if (_bigMapCoords.empty())
			throw CreateException("Exceeded maximum number of big terrain maps");

		const auto atlasPosition = _bigMapCoords.back();
		_bigMapCoords.pop_back();

		const float halfPixel = 1.0f / (2.0f * _potTextureSize);
		const std::vector atlasPositions = {
			atlasPosition + glm::vec2(halfPixel),
			atlasPosition + glm::vec2(_bigMapSize - halfPixel, halfPixel),
			atlasPosition + glm::vec2(_bigMapSize - halfPixel),
			atlasPosition + glm::vec2(halfPixel, _bigMapSize - halfPixel),
		};

		for (unsigned int i = 0; i < polygon.indices.size(); ++i) {
			const auto index = polygon.indices[i];
			const auto vertex = vertices[index];

			_vertexData.writeIEEEFloatLE(vertex.position.x);
			_vertexData.writeIEEEFloatLE(vertex.position.y);
			_vertexData.writeIEEEFloatLE(vertex.position.z);

			_vertexData.writeIEEEFloatLE(vertex.normal.x);
			_vertexData.writeIEEEFloatLE(vertex.normal.y);
			_vertexData.writeIEEEFloatLE(vertex.normal.z);

			for (int j = 0; j < 3; ++j) {
				const auto tangent = polygon.tangents[j][i];

				_vertexData.writeIEEEFloatLE(tangent.x);
				_vertexData.writeIEEEFloatLE(tangent.y);
				_vertexData.writeIEEEFloatLE(tangent.z);
			}

			for (int j = 0; j < 3; ++j) {
				const auto uv = polygon.texCoords[j][i];

				_vertexData.writeIEEEFloatLE(uv.s);
				_vertexData.writeIEEEFloatLE(uv.t);
			}

			_vertexData.writeIEEEFloatLE(atlasPositions[i].s);
			_vertexData.writeIEEEFloatLE(atlasPositions[i].t);

			numVertices++;
		}

		tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset);
		tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset + 1);
		tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset + 2);
		if (polygon.indices.size() == 4) {
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset);
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset + 2);
			tilesetIndices[polygon.tilesetId].emplace_back(currentIndexOffset + 3);
		}

		if (tilesetBoundSphere.find(polygon.tilesetId) == tilesetBoundSphere.end())
			tilesetBoundSphere[polygon.tilesetId] = polygon.boundSphere;

		tilesetBoundSphere[polygon.tilesetId] = Common::combine(tilesetBoundSphere[polygon.tilesetId],
		                                                        polygon.boundSphere);

		// currentIndex += polygon.indices.size();

		mapCoords.emplace_back(atlasPosition);
	}

	if (!terrainData.getPolygons().empty()) {
		for (const auto &tilesetId: tilesetIndices) {
			const auto tileset = tilesets[tilesetId.first];
			const auto indices = tilesetId.second;
			const auto boundSphere = tilesetBoundSphere.at(tilesetId.first);

			for (const auto &index: indices) {
				_indices.emplace_back(index);
			}

			const std::vector<Material::Uniform> materialAttributes = {
				Material::Uniform("g_sColorMaps[0]", localTextures[tileset.colorTiles[0]]),
				Material::Uniform("g_sColorMaps[1]", localTextures[tileset.colorTiles[1]]),
				Material::Uniform("g_sColorMaps[2]", localTextures[tileset.colorTiles[2]]),
				Material::Uniform("g_sNormalMaps[0]", localTextures[tileset.normalTiles[0]]),
				Material::Uniform("g_sNormalMaps[1]", localTextures[tileset.normalTiles[1]]),
				Material::Uniform("g_sNormalMaps[2]", localTextures[tileset.normalTiles[2]]),
				Material::Uniform("g_sBlendMap", _blendMap),
				Material::Uniform("g_sGeoNormalMap", _geoNormalMap),
			};

			TerrainTile terrainTile;
			terrainTile.material = Material("terrain", {"material", "depth"}, materialAttributes);
			terrainTile.material.setBuffer(_materialBuffer);
			terrainTile.indexCount = indices.size();
			terrainTile.indexOffset = indicesOffset;
			terrainTile.boundSphere = boundSphere;

			indicesOffset += terrainTile.indexCount * sizeof(uint16_t);

			_terrainTiles.emplace_back(terrainTile);
		}
	}

	for (const auto &blendMap: terrainData.getBlendMaps()) {
		const auto mapCoord = mapCoords[blendMap.id];

		const auto xoffset = static_cast<unsigned int>(mapCoord.s * _potTextureSize);
		const auto yoffset = static_cast<unsigned int>(mapCoord.t * _potTextureSize);

		assert(xoffset % blendMap.size == 0 && yoffset % blendMap.size == 0);

		Surface blendSurface(blendMap.size, blendMap.size, kA1RGB5);
		auto s = blendSurface.getData<uint16_t>();
		std::ranges::copy(blendMap.data, s.begin());
		_blendMap->load(xoffset, yoffset, std::move(blendSurface));
	}

	for (const auto &geonormalMap: terrainData.getGeonormalMaps()) {
		const auto mapCoord = mapCoords[geonormalMap.id];

		const auto xoffset = static_cast<unsigned int>(mapCoord.s * _potTextureSize);
		const auto yoffset = static_cast<unsigned int>(mapCoord.t * _potTextureSize);

		assert(xoffset % geonormalMap.size == 0 && yoffset % geonormalMap.size == 0);

		Surface blendSurface(geonormalMap.size, geonormalMap.size, kA1RGB5);
		auto s = blendSurface.getData<uint16_t>();
		std::ranges::copy(geonormalMap.data, s.begin());
		_geoNormalMap->load(xoffset, yoffset, std::move(blendSurface));
	}
}

Terrain* TerrainFactory::createTerrain() {
	// Create vertex buffer
	Common::ByteBuffer vertexDataBuffer(_vertexData.getLength());
	assert(vertexDataBuffer.size() == _vertexData.getLength());
	std::memcpy(vertexDataBuffer.data(), _vertexData.getData(), _vertexData.getLength());
	const auto vertexBuffer = GfxMan.createBuffer(
		std::move(vertexDataBuffer),
		kVertexBuffer,
		false
	);

	// Create full index buffer
	Common::DynamicMemoryWriteStream indexData(true);
	for (const auto &index: _indices) {
		indexData.writeUint16LE(index);
	}

	Common::ByteBuffer indexDataBuffer(indexData.getLength());
	std::memcpy(indexDataBuffer.data(), indexData.getData(), indexData.getLength());
	const auto indexBuffer = GfxMan.createBuffer(
		std::move(indexDataBuffer),
		kIndexBuffer,
		false
	);

	// Create attributes object
	const std::vector<VertexAttribute> attributes = {
		{kPosition, kVec3F},
		{kNormal, kVec3F},
		{kTangent0, kVec3F},
		{kTangent1, kVec3F},
		{kTangent2, kVec3F},
		{kTexCoord0, kVec4F},
		{kTexCoord1, kVec4F},
	};

	std::map<std::string, AttributeObjectPtr> attributeObjects;
	for (const auto &stage: {"material", "depth"}) {
		attributeObjects[stage] = GfxMan.createAttributeObject(
			"terrain", stage, 0,
			attributes,
			vertexBuffer
		);
	}

	// The final terrain mesh
	MeshPtr terrainMesh = std::make_shared<Mesh>();
	terrainMesh->setIndices(indexBuffer);

	// std::size_t offset = 0;

	std::unordered_map<Material, MultiIndex> terrainMaterials;
	for (const auto &tile: _terrainTiles) {
		auto &multiIndex = terrainMaterials[tile.material];
		assert(tile.indexOffset + tile.indexCount * sizeof(uint16_t) <= _indices.size() * sizeof(uint16_t));

		multiIndex.counts.emplace_back(tile.indexCount);
		multiIndex.offsets.emplace_back(tile.indexOffset);
	}

	// Create Part meshs
	std::set<Mesh::PartMesh> terrainParts;
	for (const auto & [material, multiIndex]: terrainMaterials) {
		Mesh::PartMesh partMesh;
		partMesh.renderType = Mesh::kTriangles;
		partMesh.material = material;
		partMesh.material.setCullMode(Material::kBack);

		if (multiIndex.counts.size() > 1)
			partMesh.index = multiIndex;
		else {
			partMesh.index = Index{.offset = multiIndex.offsets.back(), .length = multiIndex.counts.back()};
		}
		partMesh.vertexAttributes = attributeObjects;
		partMesh.vertexData = vertexBuffer;

		terrainMesh->addPartMesh(partMesh);
	}

	return new Terrain(terrainMesh);
}

} // End of namespace Graphics
