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

#include <fmt/format.h>

#include "terraindatafile.h"

namespace AWE {

TerrainDataFile::TerrainDataFile(Common::ReadStream &terrainData) {
	uint32_t version = terrainData.readUint32LE();
	if (version != 25)
		throw std::runtime_error(fmt::format("Invalid version for terraindata. {} given, only 25 supported", version));

	bool highDetail = terrainData.readByte() != 0;

	if (!highDetail) {
		// Read tile names
		uint32_t numTiles = terrainData.readUint32LE();
		_textures.resize(numTiles);
		for (auto &texture : _textures) {
			uint32_t fileNameLength = terrainData.readUint32LE();
			texture = terrainData.readFixedSizeString(fileNameLength, true);
		}

		// Read tilesets
		uint32_t numTilesets = terrainData.readUint32LE();
		std::vector<Tileset> tilesets(numTilesets);
		for (auto &tileset : tilesets) {
			tileset.colorTile1 = terrainData.readUint16LE();
			tileset.colorTile2 = terrainData.readUint16LE();
			tileset.colorTile3 = terrainData.readUint16LE();
			tileset.normalTile1 = terrainData.readUint16LE();
			tileset.normalTile2 = terrainData.readUint16LE();
			tileset.normalTile3 = terrainData.readUint16LE();
		}

		// Read vertices
		uint32_t numVertices = terrainData.readUint32LE();
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
		uint32_t numPolygons = terrainData.readUint32LE();
		_polygons.resize(numPolygons);
		for (auto &polygon : _polygons) {
			for (int i = 0; i < 4; ++i) {
				uint32_t index = terrainData.readUint32LE();
				if (index != 0xFFFFFFFF)
					polygon.indices.emplace_back(index);
			}

			uint8_t numVertexReferences = terrainData.readByte();
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
				terrainData.skip(24);
			}

			Common::BoundSphere boundSphere;
			polygon.boundSphere.position.x = terrainData.readIEEEFloatLE();
			polygon.boundSphere.position.y = terrainData.readIEEEFloatLE();
			polygon.boundSphere.position.z = terrainData.readIEEEFloatLE();
			polygon.boundSphere.radius = terrainData.readIEEEFloatLE();

			glm::u16vec4 polygonReferences;
			polygonReferences.x = terrainData.readUint16LE();
			polygonReferences.y = terrainData.readUint16LE();
			polygonReferences.z = terrainData.readUint16LE();
			polygonReferences.w = terrainData.readUint16LE();

			glm::u8vec4 grassMap;
			grassMap.x = terrainData.readByte();
			grassMap.y = terrainData.readByte();
			grassMap.z = terrainData.readByte();
			grassMap.w = terrainData.readByte();

			polygon.tilesetId = terrainData.readUint16LE();

			// 0x2 -> ?
			polygon.flags = terrainData.readUint16LE();

			terrainData.skip(4);
			terrainData.skip(1 + 1 + 1 + 1);
		}
	} else {
		uint32_t numPolygons = terrainData.readUint32LE();
		for (int i = 0; i < numPolygons; ++i) {
			uint32_t val1 = terrainData.readUint32LE();

			// Displacement map!
			terrainData.skip(512);

			glm::u32vec3 vec1;
			vec1.x = terrainData.readUint32LE();
			vec1.y = terrainData.readUint32LE();
			vec1.z = terrainData.readUint32LE();
		}

		uint32_t numUnknown2 = terrainData.readUint32LE();
		for (int i = 0; i < numUnknown2; ++i) {
			uint32_t val1 = terrainData.readUint32LE();
			// Grass Map
		}
	}

	uint32_t numColorBlend = terrainData.readUint32LE();
	_colorBlends.resize(numColorBlend);
	for (auto &blend : _colorBlends) {
		blend.polygonId = terrainData.readUint32LE();
		blend.size = terrainData.readUint32LE();

		uint32_t dataSize = std::pow(blend.size, 2);

		blend.data.resize(dataSize);
		terrainData.read(blend.data.data(), dataSize * 2);
	}

	/*uint32_t numNormalBlend = terrainData.readUint32LE();
	_normalBlends.resize(numNormalBlend);
	for (auto &blend : _normalBlends) {
		blend.polygonId = terrainData.readUint32LE();
		blend.size = terrainData.readUint32LE();

		uint32_t dataSize = std::pow(blend.size, 2);

		blend.data.resize(dataSize);
		terrainData.read(blend.data.data(), dataSize);
	}*/
}

const std::vector<TerrainDataFile::Vertex> &TerrainDataFile::getVertices() {
	return _vertices;
}

const std::vector<TerrainDataFile::Polygon> &TerrainDataFile::getPolygons() {
	return _polygons;
}

const std::vector<std::string> &TerrainDataFile::getTextures() {
	return _textures;
}

}
