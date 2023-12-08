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

#include <bitset>

#include "src/common/exception.h"
#include "src/common/strutil.h"

#include "src/awe/path.h"
#include "src/awe/binmshfile.h"

namespace AWE {

BINMSHFile::BINMSHFile(Common::ReadStream &binmsh) {
	load(binmsh);
}

BINMSHFile::BINMSHFile() {
}

void BINMSHFile::load(Common::ReadStream &binmsh) {
	const uint32_t version = binmsh.readUint32LE();

	if (version != 21 && version != 20 && version != 19)
		throw std::runtime_error(fmt::format("Unsupported version {}", version));

	const uint32_t vertexBufferSize = binmsh.readUint32LE();
	const uint32_t indicesCount     = binmsh.readUint32LE();
	const uint32_t indicesType      = binmsh.readUint32LE();

	assert(indicesType == 2);

	const uint32_t flags = binmsh.readUint32LE();

	_vertexData.resize(vertexBufferSize);
	binmsh.read(_vertexData.data(), _vertexData.size());

	_indexData.resize(indicesCount * indicesType);
	binmsh.read(_indexData.data(), _indexData.size());

	uint32_t boneCount = binmsh.readUint32LE();
	std::vector<std::string> boneNames(boneCount);
	for (unsigned int i = 0; i < boneCount; ++i) {
		uint32_t boneNameLength = binmsh.readUint32LE();
		std::string boneName = binmsh.readFixedSizeString(boneNameLength, true);
		boneNames[i] = boneName;

		//glm::mat4 boneTransform;
		glm::mat4x3 boneTransform;

		boneTransform[0][0] = binmsh.readIEEEFloatLE();
		boneTransform[0][1] = binmsh.readIEEEFloatLE();
		boneTransform[0][2] = binmsh.readIEEEFloatLE();
		boneTransform[1][0] = binmsh.readIEEEFloatLE();
		boneTransform[1][1] = binmsh.readIEEEFloatLE();
		boneTransform[1][2] = binmsh.readIEEEFloatLE();
		boneTransform[2][0] = binmsh.readIEEEFloatLE();
		boneTransform[2][1] = binmsh.readIEEEFloatLE();
		boneTransform[2][2] = binmsh.readIEEEFloatLE();

		// translation
		boneTransform[3][0] = binmsh.readIEEEFloatLE();
		boneTransform[3][1] = binmsh.readIEEEFloatLE();
		boneTransform[3][2] = binmsh.readIEEEFloatLE();

		Common::BoundSphere boneBoundSphere{};
		boneBoundSphere.position.x = binmsh.readIEEEFloatLE();
		boneBoundSphere.position.y = binmsh.readIEEEFloatLE();
		boneBoundSphere.position.z = binmsh.readIEEEFloatLE();
		boneBoundSphere.radius = binmsh.readIEEEFloatLE();

		_inverseRestTransforms[boneName] = boneTransform;
	}

	const bool animated = boneCount > 0;

	_boundSphere.position.x = binmsh.readIEEEFloatLE();
	_boundSphere.position.y = binmsh.readIEEEFloatLE();
	_boundSphere.position.z = binmsh.readIEEEFloatLE();
	_boundSphere.radius = binmsh.readIEEEFloatLE();

	_boundBox.xmin = binmsh.readIEEEFloatLE();
	_boundBox.ymin = binmsh.readIEEEFloatLE();
	_boundBox.zmin = binmsh.readIEEEFloatLE();
	_boundBox.xmax = binmsh.readIEEEFloatLE();
	_boundBox.ymax = binmsh.readIEEEFloatLE();
	_boundBox.zmax = binmsh.readIEEEFloatLE();

	assert(_boundBox.xmax >= _boundBox.xmin);
	assert(_boundBox.ymax >= _boundBox.ymin);
	assert(_boundBox.zmax >= _boundBox.zmin);

	const uint32_t globalProperties = animated ? 0x40000000 : 0;

	const uint32_t lodCount = binmsh.readUint32LE();

	const uint32_t materialCount = binmsh.readUint32LE();
	std::vector<Material> materials(materialCount);
	for (auto &material : materials) {
		if (version >= 20) {
			const uint32_t nameLength = binmsh.readUint32LE();
			material.name = binmsh.readFixedSizeString(nameLength);
		}

		const uint32_t shaderNameLength = binmsh.readUint32LE();
		const std::string shaderName = Common::toLower(binmsh.readFixedSizeString(shaderNameLength, true));
		material.shader = std::regex_replace(shaderName, std::regex("\\.rfx"), "");

		material.properties = binmsh.readUint32LE() | globalProperties;
		material.blendMode  = BlendMode(binmsh.readUint32LE());
		material.cullMode   = CullMode(binmsh.readUint32LE());
		const uint32_t materialFlags = binmsh.readUint32LE();

		const std::bitset<32> f(materialFlags);
		const bool castShadow = f[0];
		const bool refractive = f[1];
		const bool specular = f[2];

		const uint32_t numUniforms = binmsh.readUint32LE();
		material.uniforms.resize(numUniforms);
		for (auto &uniform : material.uniforms) {
			const uint32_t uniformNameLength = binmsh.readUint32LE();
			uniform.name = binmsh.readFixedSizeString(uniformNameLength, true);

			const uint32_t dataType = binmsh.readUint32LE();
			switch (dataType) {
				case 0:
					uniform.data = binmsh.readIEEEFloatLE();
					break;
				case 1:
					uniform.data = binmsh.read<glm::vec2>();
					break;
				case 2:
					uniform.data = binmsh.read<glm::vec3>();
					break;
				case 3:
					uniform.data = binmsh.read<glm::vec4>();
					break;
				case 7: {
					uint32_t length = binmsh.readUint32LE();
					uniform.data = AWE::getNormalizedPath(binmsh.readFixedSizeString(length, true));
					break;
				}

				default:
					throw CreateException("Invalid or unknown uniform data type {}", dataType);
			}
		}
	}

	const uint32_t meshCount = binmsh.readUint32LE();
	_meshs.resize(meshCount);
	unsigned int i = 0;
	for (auto &mesh : _meshs) {
		mesh.lod    = binmsh.readUint32LE();
		mesh.vertexCount   = binmsh.readUint32LE();
		mesh.indicesCount  = binmsh.readUint32LE() * 3; // Number of faces
		mesh.vertexOffset  = binmsh.readUint32LE();
		mesh.indicesOffset = binmsh.readUint32LE() * indicesType;
		binmsh.skip(4); // Unknown

		if (version >= 21)
			binmsh.skip(16);

		assert(mesh.lod < lodCount);

		uint8_t vertexAttributeCount = binmsh.readByte();
		mesh.attributes.resize(vertexAttributeCount);
		for (auto &attribute: mesh.attributes) {
			binmsh.skip(1);
			const uint8_t usage = binmsh.readByte();
			const uint8_t type = binmsh.readByte();

			switch (usage) {
				case 0x02:
					attribute.usage = kPosition;
					break;
				case 0x04:
					if (type == 3)
						attribute.usage = kTangent;
					else
						attribute.usage = kColor;
					break;
				case 0x05:
					if (type == 5)
						attribute.usage = kBoneIndex;
					else
						attribute.usage = kBoneWeight;
					break;
				case 0x07:
					attribute.usage = kTexCoord;
					break;
				case 0x08:
					attribute.usage = kNormal;
					break;
				default:
					throw CreateException("Unknown vertex attribute usage {}", type);
			}

			switch (type) {
				case 0:
					attribute.type = kFloat3;
					break;
				case 1:
					attribute.type = kShort4F;
					break;
				case 2:
					attribute.type = kShort2F;
					break;
				case 6: [[fallthrough]];
				case 3: [[fallthrough]];
				case 4:
					attribute.type = kByte4F;
					break;
				case 5:
					attribute.type = kByte4I;
					break;
				default:
					throw CreateException("Unknown vertex attribute type {}", type);
			}
		}

		mesh.material = materials[i];

		const uint32_t boneMapCount = binmsh.readUint32LE();
		mesh.boneMap.resize(boneMapCount);
		for (auto &boneName: mesh.boneMap)
			boneName = boneNames[binmsh.readByte()];

		i++;
	}
}

const Common::BoundSphere &BINMSHFile::getBoundSphere() const {
	return _boundSphere;
}

const Common::BoundBox &BINMSHFile::getBoundBox() const {
	return _boundBox;
}

const std::vector<byte> &BINMSHFile::getVertexData() const {
	return _vertexData;
}

const std::vector<byte> &BINMSHFile::getIndexData() const {
	return _indexData;
}

const std::vector<BINMSHFile::Mesh> &BINMSHFile::getMeshs() const {
	return _meshs;
}

const std::map<std::string, glm::mat4> &BINMSHFile::getInverseRestTransforms() const {
	return _inverseRestTransforms;
}



} // End of namespace AWE