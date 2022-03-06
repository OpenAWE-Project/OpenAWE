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

#include <iostream>
#include <regex>
#include <bitset>

#include <fmt/format.h>

#include "src/common/exception.h"
#include "src/common/strutil.h"

#include <src/common/memwritestream.h>
#include <src/awe/resman.h>
#include <src/graphics/images/tex.h>
#include <src/common/writefile.h>
#include "mesh_binmsh.h"
#include "gfxman.h"
#include "textureman.h"

namespace Graphics {

BINMSHMesh::BINMSHMesh(Common::ReadStream *binmsh) {
	load(binmsh);
}

BINMSHMesh::BINMSHMesh() {
}

void BINMSHMesh::load(Common::ReadStream *binmsh) {
	uint32_t version = binmsh->readUint32LE();

	if (version != 21 && version != 20 && version != 19)
		throw std::runtime_error(fmt::format("Unsupported version {}", version));

	uint32_t vertexBufferSize = binmsh->readUint32LE();
	uint32_t indicesCount     = binmsh->readUint32LE();
	uint32_t indicesType      = binmsh->readUint32LE();

	assert(indicesType == 2);

	uint32_t flags = binmsh->readUint32LE();

	std::vector<byte> vertexBuffer(vertexBufferSize);
	binmsh->read(vertexBuffer.data(), vertexBuffer.size());
	BufferPtr buffer = GfxMan.createBuffer(vertexBuffer.data(), vertexBuffer.size(), kVertexBuffer);

	byte *indicesData = new byte[indicesCount * indicesType];
	binmsh->read(indicesData, indicesCount * indicesType);
	_indices = GfxMan.createBuffer(indicesData, indicesCount * indicesType, kIndexBuffer);
	delete [] indicesData;

	uint32_t boneCount = binmsh->readUint32LE();
	for (int i = 0; i < boneCount; ++i) {
		uint32_t boneNameLength = binmsh->readUint32LE();
		std::string boneName = binmsh->readFixedSizeString(boneNameLength, true);

		glm::mat4x3 boneTransform;
		glm::mat3x4 boneTransform2;

		boneTransform2[0][0] = binmsh->readIEEEFloatLE();
		boneTransform2[0][1] = binmsh->readIEEEFloatLE();
		boneTransform2[0][2] = binmsh->readIEEEFloatLE();
		boneTransform2[0][3] = binmsh->readIEEEFloatLE();
		boneTransform2[1][0] = binmsh->readIEEEFloatLE();
		boneTransform2[1][1] = binmsh->readIEEEFloatLE();
		boneTransform2[1][2] = binmsh->readIEEEFloatLE();
		boneTransform2[1][3] = binmsh->readIEEEFloatLE();
		boneTransform2[2][0] = binmsh->readIEEEFloatLE();

		// translation
		boneTransform2[2][1] = binmsh->readIEEEFloatLE();
		boneTransform2[2][2] = binmsh->readIEEEFloatLE();
		boneTransform2[2][3] = binmsh->readIEEEFloatLE();

		Common::BoundSphere boneBoundSphere{};
		boneBoundSphere.position.x = binmsh->readIEEEFloatLE();
		boneBoundSphere.position.y = binmsh->readIEEEFloatLE();
		boneBoundSphere.position.z = binmsh->readIEEEFloatLE();
		boneBoundSphere.radius = binmsh->readIEEEFloatLE();

		_initialPose[boneName] = boneTransform;
	}

	Common::BoundSphere boundSphere{};
	boundSphere.position.x = binmsh->readIEEEFloatLE();
	boundSphere.position.y = binmsh->readIEEEFloatLE();
	boundSphere.position.z = binmsh->readIEEEFloatLE();
	boundSphere.radius = binmsh->readIEEEFloatLE();

	Common::BoundBox boundBox{};
	boundBox.xmin = binmsh->readIEEEFloatLE();
	boundBox.ymin = binmsh->readIEEEFloatLE();
	boundBox.zmin = binmsh->readIEEEFloatLE();
	boundBox.xmax = binmsh->readIEEEFloatLE();
	boundBox.ymax = binmsh->readIEEEFloatLE();
	boundBox.zmax = binmsh->readIEEEFloatLE();

	assert(boundBox.xmax >= boundBox.xmin);
	assert(boundBox.ymax >= boundBox.ymin);
	assert(boundBox.zmax >= boundBox.zmin);

	uint32_t lodCount = binmsh->readUint32LE();

	uint32_t materialCount = binmsh->readUint32LE();
	std::vector<Material> materials(materialCount);
	for (auto &material : materials) {
		if (version >= 20) {
			uint32_t nameLength = binmsh->readUint32LE();
			std::string name = binmsh->readFixedSizeString(nameLength);
		}

		uint32_t shaderNameLength = binmsh->readUint32LE();
		std::string shaderName    = binmsh->readFixedSizeString(shaderNameLength, true);
		shaderName = Common::toLower(shaderName);
		shaderName = std::regex_replace(shaderName, std::regex("\\.rfx"), "");

		//binmsh->skip(4);
		uint32_t properties = binmsh->readUint32LE();
		uint32_t blendMode = binmsh->readUint32LE();
		uint32_t cullMode = binmsh->readUint32LE();
		uint32_t materialFlags = binmsh->readUint32LE();

		std::bitset<32> f(materialFlags);
		bool castShadow = f[0];
		bool refractive = f[1];
		bool specular = f[2];

		uint32_t numAttributes = binmsh->readUint32LE();
		std::vector<Material::Attribute> attributes;
		for (int j = 0; j < numAttributes; ++j) {
			uint32_t attributeNameLength = binmsh->readUint32LE();
			std::string attributeName = binmsh->readFixedSizeString(attributeNameLength, true);

			uint32_t dataType = binmsh->readUint32LE();
			switch (dataType) {
				case 0: {
					float v1 = binmsh->readIEEEFloatLE();

					attributes.emplace_back(Material::Attribute(attributeName, glm::vec1(v1)));
					break;
				}
				case 1: {
					float v1 = binmsh->readIEEEFloatLE();
					float v2 = binmsh->readIEEEFloatLE();

					attributes.emplace_back(Material::Attribute(attributeName, glm::vec2(v1, v2)));
					break;
				}
				case 2: {
					float v1 = binmsh->readIEEEFloatLE();
					float v2 = binmsh->readIEEEFloatLE();
					float v3 = binmsh->readIEEEFloatLE();

					attributes.emplace_back(Material::Attribute(attributeName, glm::vec3(v1, v2, v3)));
					break;
				}
				case 3: {
					float v1 = binmsh->readIEEEFloatLE();
					float v2 = binmsh->readIEEEFloatLE();
					float v3 = binmsh->readIEEEFloatLE();
					float v4 = binmsh->readIEEEFloatLE();

					attributes.emplace_back(Material::Attribute(attributeName, glm::vec4(v1, v2, v3, v4)));
					break;
				}
				case 7: {
					uint32_t length = binmsh->readUint32LE();
					std::string file = binmsh->readFixedSizeString(length, true);
					file = std::regex_replace(file, std::regex("runtimedata\\\\pc"), "d:");
					file = std::regex_replace(file, std::regex("\\\\"), "/");
					file = std::regex_replace(file, std::regex("d:/data/"), "");

					attributes.emplace_back(Material::Attribute(attributeName, TextureMan.getTexture(file)));
					break;
				}
			}
		}

		material = Material(shaderName, properties, attributes);

		switch (cullMode) {
			case 2:
				material.setCullMode(Material::kBack);
				break;
			case 0:
				material.setCullMode(Material::kNone);
				break;
			default:
				throw std::runtime_error("Invalid Cull Mode");
		}

		switch (blendMode) {
			case 0:
				material.setBlendMode(kNone);
				break;
			case 3:
				material.setBlendMode(kMultiply);
				break;
			default:
				throw Common::Exception("Invalid blend mode {}", blendMode);
		}
	}

	uint32_t meshCount = binmsh->readUint32LE();
	for (int i = 0; i < meshCount; ++i) {
		Mesh mesh;

		uint32_t meshLayer    = binmsh->readUint32LE();
		uint32_t vertexCount  = binmsh->readUint32LE();
		uint32_t faceCount    = binmsh->readUint32LE();
		uint32_t vertexOffset = binmsh->readUint32LE();
		uint32_t faceOffset   = binmsh->readUint32LE();
		binmsh->skip(4); // Unknown

		if (version >= 21)
			binmsh->skip(16);

		assert(meshLayer < lodCount);

		uint8_t vertexAttributeCount = binmsh->readByte();
		std::vector<VertexAttribute> attributes;
		attributes.resize(vertexAttributeCount);
		unsigned int uvs = 0;
		for (int j = 0; j < vertexAttributeCount; ++j) {
			VertexAttribute &vertexAttribute = attributes[j];
			binmsh->skip(1);
			uint8_t componentType = binmsh->readByte();
			uint8_t dataType = binmsh->readByte();

			switch (componentType) {
				case 0x02:
					vertexAttribute.component = kPosition;
					break;
				case 0x04:
					vertexAttribute.component = kColor;
					break;
				case 0x05:
					if (dataType == 5)
						vertexAttribute.component = kBoneIndex;
					else
						vertexAttribute.component = kBoneWeight;
					break;
				case 0x07:
					vertexAttribute.component = getTexCoord(uvs);
					uvs += 1;
					break;
				case 0x08:
					vertexAttribute.component = kNormal;
					break;
				default:
					std::cout << "Unknown" << std::endl;
					break;
			}

			switch (dataType) {
				case 0:
					vertexAttribute.dataType = kVec3F;
					break;
				case 1:
					vertexAttribute.dataType = kVec4S;
					break;
				case 2:
					vertexAttribute.dataType = kVec2S;
					break;
				case 6:
				case 3:
				case 4:
					vertexAttribute.dataType = kVec4BF;
					break;
				case 5:
					vertexAttribute.dataType = kVec4BI;
					break;
				default:
					std::cout << "Unknown" << std::endl;
					break;
			}
		}

		uint32_t boneMapCount = binmsh->readUint32LE();
		binmsh->skip(boneMapCount);

		if (meshLayer != 0)
			continue;

		_meshs.emplace_back();

		size_t lastPos = binmsh->pos();

		binmsh->seek(lastPos);

		_meshs[i].renderType = kTriangles;
		_meshs[i].vertexData = buffer;
		_meshs[i].vertexAttributes = GfxMan.createAttributeObject(
				materials[i].getShaderName(),
				attributes,
				buffer,
				vertexOffset
		);
		_meshs[i].material = materials[i];
		_meshs[i].offset = faceOffset * indicesType;
		_meshs[i].length = faceCount * 3;
	}
}
}
