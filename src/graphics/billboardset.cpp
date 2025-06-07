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

#include "src/common/memwritestream.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/billboardset.h"
#include "src/graphics/textureman.h"

namespace Graphics {

BillboardSet::BillboardSet(
	const std::string &colorAtlas,
	const std::vector<BillboardType> &types,
	const std::vector<BillboardInstance> &instances
) {
	Common::DynamicMemoryWriteStream vertexData(true);
	Common::DynamicMemoryWriteStream indicesData(true);
	unsigned int index = 0;
	for (const auto &instance: instances) {
		for (int i = 0; i < 4; ++i) {
			vertexData.writeIEEEFloatLE(instance.position.x);
			vertexData.writeIEEEFloatLE(instance.position.y);
			vertexData.writeIEEEFloatLE(instance.position.z);

			vertexData.writeByte(instance.type);
		}

		indicesData.writeUint16LE(0 + index * 4);
		indicesData.writeUint16LE(1 + index * 4);
		indicesData.writeUint16LE(2 + index * 4);
		indicesData.writeUint16LE(1 + index * 4);
		indicesData.writeUint16LE(0 + index * 4);
		indicesData.writeUint16LE(3 + index * 4);
		index++;
	}

	Common::DynamicMemoryWriteStream typesData(true);
	for (const auto &type: types) {
		typesData.writeIEEEFloatLE(type.atlasPosition.s);
		typesData.writeIEEEFloatLE(type.atlasPosition.t);

		typesData.writeIEEEFloatLE(type.atlasSize.s);
		typesData.writeIEEEFloatLE(type.atlasSize.t);

		typesData.writeIEEEFloatLE(type.billboardSize.x);
		typesData.writeIEEEFloatLE(type.billboardSize.y);
	}

	Common::ByteBuffer vertexBuffer(vertexData.getLength());
	Common::ByteBuffer indexBuffer(indicesData.getLength());

	std::memcpy(vertexBuffer.data(), vertexData.getData(), vertexData.getLength());
	std::memcpy(indexBuffer.data(), indicesData.getData(), indicesData.getLength());

	_colorAtlas = TextureMan.getTexture(colorAtlas);

	_mesh->setIndices(GfxMan.createBuffer(std::move(indexBuffer),
										  kIndexBuffer, false));

	const std::vector<VertexAttribute> attributes = {
		{kPosition,       kVec3F},
		{kColor0,      kUByte}
	};

	std::vector<Material::Uniform> materialAttributes;
	for (unsigned int i = 0; i < types.size(); ++i) {
		materialAttributes.emplace_back(Material::Uniform(
			std::format("g_foliageMeshTable[{}].vAtlasPosition", i),
			types[i].atlasPosition
		));
		materialAttributes.emplace_back(Material::Uniform(
			std::format("g_foliageMeshTable[{}].vAtlasSize", i),
			types[i].atlasSize
		));
		materialAttributes.emplace_back(Material::Uniform(
			std::format("g_foliageMeshTable[{}].vBillboardSize", i),
			types[i].billboardSize
		));
	}

	materialAttributes.emplace_back(Material::Uniform("g_sColorMap", _colorAtlas));

	Mesh::PartMesh partMesh;
	partMesh.vertexData = GfxMan.createBuffer(std::move(vertexBuffer), kVertexBuffer, false);

	for (const auto &stage: {"material", "depth"}) {
		partMesh.vertexAttributes[stage] = GfxMan.createAttributeObject(
			"foliagebillboard",
			stage,
			0,
			attributes,
			partMesh.vertexData
		);
	}

	partMesh.renderType = Mesh::kTriangles;
	partMesh.material = Material(
		"foliagebillboard",
		{"material"},
		materialAttributes);
	partMesh.material.setCullMode(Material::kNone);
	partMesh.offset = 0;
	partMesh.length = index * 6;

	if (instances.empty())
		return;

	_mesh->addPartMesh(partMesh);
}

} // End of namespace Graphics
