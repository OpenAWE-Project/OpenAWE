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

#include <stdexcept>
#include <vector>
#include <regex>

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <glm/gtx/string_cast.hpp>

#include "terraindatafile.h"

namespace AWE {

TerrainDataFile::TerrainDataFile(Common::ReadStream &terrainData) {
	const uint32_t version = terrainData.readUint32LE();
	if (version != 25)
		throw std::runtime_error(fmt::format("Invalid version for terraindata. {} given, only 25 supported", version));

	_hd = terrainData.readByte() != 0;

	if (!_hd) {
		// Read tile names
		const uint32_t numTiles = terrainData.readUint32LE();
		_textures.resize(numTiles);
		for (auto &texture : _textures) {
			uint32_t fileNameLength = terrainData.readUint32LE();
			texture = terrainData.readFixedSizeString(fileNameLength, true);

			// Normalize Path
			texture = std::regex_replace(texture, std::regex("runtimedata\\\\pc"), "d:");
			texture = std::regex_replace(texture, std::regex("\\\\"), "/");
			texture = std::regex_replace(texture, std::regex("d:/data/"), "");
		}

		// Read tilesets
		const uint32_t numTilesets = terrainData.readUint32LE();
		_tilesets.resize(numTilesets);
		for (auto &tileset : _tilesets) {
			tileset.colorTiles[0] = terrainData.readUint16LE();
			tileset.colorTiles[1] = terrainData.readUint16LE();
			tileset.colorTiles[2] = terrainData.readUint16LE();
			tileset.normalTiles[0] = terrainData.readUint16LE();
			tileset.normalTiles[1] = terrainData.readUint16LE();
			tileset.normalTiles[2] = terrainData.readUint16LE();
		}

		// Read vertices
		const uint32_t numVertices = terrainData.readUint32LE();
		_vertices.resize(numVertices);
		for (auto &vertex : _vertices) {
			vertex.position.x = terrainData.readIEEEFloatLE();
			vertex.position.y = terrainData.readIEEEFloatLE();
			vertex.position.z = terrainData.readIEEEFloatLE();

			vertex.normal.x = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
			vertex.normal.y = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
			vertex.normal.z = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;

			vertex.displacementFactor = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
		}

		// Read Polygons
		const uint32_t numPolygons = terrainData.readUint32LE();
		_polygons.resize(numPolygons);
		for (auto &polygon : _polygons) {
			for (int i = 0; i < 4; ++i) {
				const uint32_t index = terrainData.readUint32LE();
				if (index != 0xFFFFFFFF)
					polygon.indices.emplace_back(index);
			}

			const uint8_t numVertexReferences = terrainData.readByte();
			assert(polygon.indices.size() == numVertexReferences);

			glm::u8vec3 binormalSign;
			binormalSign.x = terrainData.readByte();
			binormalSign.y = terrainData.readByte();
			binormalSign.z = terrainData.readByte();

			// Texcoords for maps
			for (auto & texCoord : polygon.texCoords) {
				texCoord.resize(numVertexReferences);
			}
			for (int i = 0; i < numVertexReferences; ++i) {
				for (auto & texCoord : polygon.texCoords) {
					texCoord[i].s = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
					texCoord[i].t = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
				}
			}

			// Skip over all unused tex coordinates
			for (int i = 0; i < 4 - numVertexReferences; ++i) {
				terrainData.skip(16);
			}

			// Tangents
			for (auto & tangent : polygon.tangents) {
				tangent.resize(numVertexReferences);
			}
			for (int i = 0; i < numVertexReferences; ++i) {
				for (auto & tangent : polygon.tangents) {
					tangent[i].x = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
					tangent[i].y = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
					tangent[i].z = static_cast<float>(terrainData.readSint16LE()) / 32767.0f;
				}
			}

			// Skip over all unused tangents
			for (int i = 0; i < 4 - numVertexReferences; ++i) {
				terrainData.skip(18);
			}

			Common::BoundSphere boundSphere;
			polygon.boundSphere.position.x = terrainData.readIEEEFloatLE();
			polygon.boundSphere.position.y = terrainData.readIEEEFloatLE();
			polygon.boundSphere.position.z = terrainData.readIEEEFloatLE();
			polygon.boundSphere.radius = terrainData.readIEEEFloatLE();

			glm::i16vec4 polygonReferences;
			polygonReferences.x = terrainData.readSint16LE();
			polygonReferences.y = terrainData.readSint16LE();
			polygonReferences.z = terrainData.readSint16LE();
			polygonReferences.w = terrainData.readSint16LE();

			assert(polygonReferences.x < static_cast<int>(numPolygons));
			assert(polygonReferences.y < static_cast<int>(numPolygons));
			assert(polygonReferences.z < static_cast<int>(numPolygons));
			assert(polygonReferences.w < static_cast<int>(numPolygons));

			terrainData.skip(1 + 1 + 1 + 1);

			polygon.tilesetId = terrainData.readUint16LE();

			// 0x02 -> ?
			// 0x08 -> ?
			// 0x10 -> ?
			polygon.flags = terrainData.readUint16LE();

			//terrainData.skip(4);
			const auto val = terrainData.readUint32LE();
			terrainData.skip(1 + 1 + 1 + 1);
		}
	} else {
		const uint32_t numPolygons = terrainData.readUint32LE();
		for (unsigned int i = 0; i < numPolygons; ++i) {
			const uint32_t id = terrainData.readUint32LE();

			// Displacement map!
			terrainData.skip(512);

			glm::u32vec3 vec1;
			vec1.x = terrainData.readUint32LE();
			vec1.y = terrainData.readUint32LE();
			vec1.z = terrainData.readUint32LE();
		}

		const uint32_t numUnk = terrainData.readUint32LE();
		for (unsigned int i = 0; i < numUnk; ++i) {
			const uint32_t id = terrainData.readUint32LE();

			terrainData.skip(64);
			terrainData.skip(6);
			terrainData.skip(6);
		}
	}

	// Geonormal maps
	const uint32_t numGeoNormalMaps = terrainData.readUint32LE();
	for (unsigned int i = 0; i < numGeoNormalMaps; i++) {
		Blend blend;
		blend.id = terrainData.readUint32LE();
		blend.size = terrainData.readUint32LE();

		const uint32_t dataSize = std::pow<uint32_t>(blend.size, 2);

		blend.data.resize(dataSize);
		terrainData.read(blend.data.data(), dataSize * 2);

		_geonormalMaps.emplace_back(blend);
	}

	// Blend maps
	const uint32_t numBlendMaps = terrainData.readUint32LE();
	for (unsigned int i = 0; i < numBlendMaps; i++) {
		Blend blend;
		blend.id = terrainData.readUint32LE();
		blend.size = terrainData.readUint32LE();

		const uint32_t dataSize = std::pow<uint32_t>(blend.size, 2);

		blend.data.resize(dataSize);
		terrainData.read(blend.data.data(), dataSize * 2);

		_blendMaps.emplace_back(blend);
	}
}

bool TerrainDataFile::isHighDetail() const {
	return _hd;
}

const std::vector<TerrainDataFile::Tileset> &TerrainDataFile::getTilesets() const {
	return _tilesets;
}

const std::vector<TerrainDataFile::Vertex> &TerrainDataFile::getVertices() const {
	return _vertices;
}

const std::vector<TerrainDataFile::Polygon> &TerrainDataFile::getPolygons() const {
	return _polygons;
}

const std::vector<std::string> &TerrainDataFile::getTextures() const {
	return _textures;
}

const std::vector<TerrainDataFile::Blend> &TerrainDataFile::getGeonormalMaps() const {
	return _geonormalMaps;
}

const std::vector<TerrainDataFile::Blend> &TerrainDataFile::getBlendMaps() const {
	return _blendMaps;
}

}
