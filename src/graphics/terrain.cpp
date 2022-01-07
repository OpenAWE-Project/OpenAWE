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
#include <src/common/writefile.h>

#include "src/common/memwritestream.h"

#include "src/awe/terraindatafile.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/terrain.h"
#include "src/graphics/textureman.h"
#include "src/graphics/images/surface.h"

namespace Graphics {

Terrain::Terrain(Common::ReadStream *terrainDataFile) {
	show();

	AWE::TerrainDataFile terrainData(*terrainDataFile);
	std::vector<uint16_t> indices;
	const auto &vertices = terrainData.getVertices();
	const auto &tilesets = terrainData.getTilesets();

	for (const auto &texture : terrainData.getTextures()) {
		_textures.emplace_back(TextureMan.getTexture(texture));
	}

	for (const auto &polygon : terrainData.getPolygons()) {
		Common::DynamicMemoryWriteStream vertexData(true);

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
			}
		}

		std::vector<uint16_t> localIndices;
		for (int i = 0; i + 2 < polygon.indices.size(); i++) {
			localIndices.emplace_back((i * 2) % 4);
			localIndices.emplace_back((i * 2 + 1) % 4);
			localIndices.emplace_back((i * 2 + 2) % 4);
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
			{kTexCoord0,      kVec2F},
			{kTexCoord1,      kVec2F},
			{kTexCoord2,      kVec2F},
			{kTexCoord3,      kVec2F}
		};

		// Generate blend map
		const auto &blend1 = polygon.blend1;
		const auto &blend2 = polygon.blend2;

		assert(blend1.size == blend2.size);

		Common::DynamicMemoryWriteStream blendData(true);
		for (int i = 0; i < blend1.size * blend1.size; ++i) {
			blendData.writeUint16LE(blend1.data[i]);
			blendData.writeUint16LE(blend2.data[i]);
		}

		Surface surface(blend1.size, blend1.size, kRG16);
		std::memcpy(surface.getData(), blendData.getData(), blendData.getLength());
		const auto blendid = GfxMan.createTexture(surface);

		const auto tileset = tilesets[polygon.tilesetId];
		const std::vector<Material::Attribute> materialAttributes = {
			Material::Attribute("g_sColorMaps[0]",  _textures[tileset.colorTile1]),
			Material::Attribute("g_sColorMaps[1]",  _textures[tileset.colorTile2]),
			Material::Attribute("g_sColorMaps[2]",  _textures[tileset.colorTile3]),
			Material::Attribute("g_sNormalMaps[0]", _textures[tileset.normalTile1]),
			Material::Attribute("g_sNormalMaps[1]", _textures[tileset.normalTile2]),
			Material::Attribute("g_sNormalMaps[2]", _textures[tileset.normalTile3]),
			Material::Attribute("g_sBlendMap",      blendid),
			Material::Attribute("g_vTexCoordScale1",glm::vec4(16.0)),
			Material::Attribute("g_vTexCoordScale2",glm::vec4(16.0))
		};

		partMesh.vertexAttributes = GfxMan.createAttributeObject(
			"terrain",
			attributes,
			partMesh.vertexData
		);
		partMesh.renderType = Mesh::kTriangles;
		partMesh.material = Material("terrain", materialAttributes);
		partMesh.material.setCullMode(Material::kNone);
		partMesh.offset = indices.size() * 2;
		partMesh.length = localIndices.size();

		for (const auto &localIndex : localIndices) {
			indices.emplace_back(localIndex);
		}

		_mesh->addPartMesh(partMesh);
	}

	_mesh->setIndices(GfxMan.createBuffer(reinterpret_cast<byte*>(indices.data()), indices.size() * 2, kIndexBuffer));
}

Terrain::~Terrain() {
	/*for (auto &blendMap : _blendMaps) {
		GfxMan.deregisterTexture(blendMap);
	}*/
}

}