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

#include <fstream>

#include "src/common/memwritestream.h"

#include "src/awe/terraindatafile.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/terrain.h"

namespace Graphics {

Terrain::Terrain(Common::ReadStream *terrainDataFile) {
	show();

	AWE::TerrainDataFile terrainData(*terrainDataFile);

	Common::DynamicMemoryWriteStream vertexData(true);
	const auto &vertices = terrainData.getVertices();

	for (const auto &vertex : vertices) {
		vertexData.writeIEEEFloatLE(vertex.position.x);
		vertexData.writeIEEEFloatLE(vertex.position.y);
		vertexData.writeIEEEFloatLE(vertex.position.z);

		vertexData.writeIEEEFloatLE(vertex.normal.x);
		vertexData.writeIEEEFloatLE(vertex.normal.y);
		vertexData.writeIEEEFloatLE(vertex.normal.z);
	}

	Mesh::PartMesh partMesh;
	partMesh.vertexDataId = GfxMan.registerVertices(
			vertexData.getData(),
			vertexData.getLength(),
			Common::UUID::generateNil()
	);

	std::vector<VertexAttribute> attributes = {
			{kPosition, kVec3F},
			{kNormal,   kVec3F}
	};
	partMesh.vertexAttributesId = GfxMan.registerVertexAttributes(
			"terrain",
			attributes,
			partMesh.vertexDataId
	);
	partMesh.renderType = Mesh::kPoints;
	partMesh.material = Material("terrain", std::vector<Material::Attribute>());
	partMesh.material.setCullMode(Material::kFront);
	partMesh.offset = 0;
	partMesh.length = vertices.size();

	std::vector<uint16_t> indices;
	for (int i = 0; i < vertices.size(); ++i) {
		indices.emplace_back(i);
	}

	_mesh->setIndices(GfxMan.registerIndices(reinterpret_cast<byte *>(indices.data()), indices.size() * 2));
	_mesh->addPartMesh(partMesh);
}

}