//
// Created by patrick on 08.12.23.
//

#include "src/common/exception.h"

#include "src/awe/binfolfile.h"

#include "src/graphics/binfolloader.h"
#include "src/graphics/gfxman.h"

namespace Graphics {

BINFOLLoader::BINFOLLoader() : MeshLoader({".binfol"}){
}

MeshPtr BINFOLLoader::load(Common::ReadStream &stream, std::initializer_list<std::string> stages) const {
	AWE::BINFOLFile binfol(stream);

	const auto &vertexBufferData = binfol.getVertexData();
	const auto &indexBufferData = binfol.getIndexData();

	// Create vertex and index buffer buffer
	const auto vertexBuffer = GfxMan.createBuffer(vertexBufferData.data(), vertexBufferData.size(), kVertexBuffer);
	const auto indexBuffer = GfxMan.createBuffer(indexBufferData.data(), indexBufferData.size(), kIndexBuffer);

	auto finalMesh = std::make_shared<Mesh>();
	finalMesh->setBoundingSphere(binfol.getBoundSphere());
	finalMesh->setInverseRestTransforms(binfol.getInverseRestTransforms());

	for (const auto &mesh: binfol.getMeshs()) {
		if (mesh.lod > 0)
			continue;

		finalMesh->setIndices(indexBuffer);

		Mesh::PartMesh part;
		part.renderType = Mesh::kTriangles;
		part.vertexData = vertexBuffer;
		part.offset = mesh.indicesOffset;
		part.length = mesh.indicesCount;
		part.boneMap = mesh.boneMap;

		const auto &material = mesh.material;

		std::vector<Material::Uniform> uniforms;
		for (const auto &uniform : material.uniforms) {
			std::visit([&](const auto &arg) mutable {
				uniforms.emplace_back(uniform.name, arg);
			}, uniform.data);
		}

		part.material = Material(material.shader, stages, uniforms, material.properties);

		switch (material.cullMode) {
			case AWE::BINMSHFile::kCullNone:
				part.material.setCullMode(Graphics::Material::kNone);
				break;

			case AWE::BINMSHFile::kCullBack:
				part.material.setCullMode(Graphics::Material::kBack);
				break;

			case AWE::BINMSHFile::kCullFront:
				part.material.setCullMode(Graphics::Material::kFront);
				break;

			default:
				throw CreateException("Invalid or unsupported culling mode");
		}

		std::vector<VertexAttribute> attributes;
		unsigned int texCoordId = 0;
		unsigned int colorId = 0;
		for (const auto &attribute: mesh.attributes) {
			AttributeType type;
			VertexDataType dataType;
			switch (attribute.usage) {
				case AWE::BINMSHFile::kPosition:
					type = kPosition;
					break;

				case AWE::BINMSHFile::kTexCoord:
					type = getTexCoord(texCoordId);
					texCoordId++;
					break;

				case AWE::BINMSHFile::kNormal:
					type = kNormal;
					break;

				case AWE::BINMSHFile::kTangent:
					type = kTangent;
					break;

				case AWE::BINMSHFile::kColor:
					type = getColor(colorId);
					colorId++;
					break;

				case AWE::BINMSHFile::kBoneIndex:
					type = kBoneIndex;
					break;

				case AWE::BINMSHFile::kBoneWeight:
					type = kBoneWeight;
					break;
			}

			switch (attribute.type) {
				case AWE::BINMSHFile::kFloat3:
					dataType = kVec3F;
					break;

				case AWE::BINMSHFile::kShort4F:
					dataType = kVec4S;
					break;

				case AWE::BINMSHFile::kShort2F:
					dataType = kVec2S;
					break;

				case AWE::BINMSHFile::kByte4F:
					dataType = kVec4BF;
					break;

				case AWE::BINMSHFile::kByte4I:
					dataType = kVec4BI;
					break;
			}

			attributes.emplace_back(VertexAttribute{type, dataType});
		}

		for (const auto &stage : stages) {
			part.vertexAttributes[stage] = GfxMan.createAttributeObject(
					mesh.material.shader,
					stage,
					mesh.material.properties,
					attributes,
					vertexBuffer,
					mesh.vertexOffset
			);
		}

		finalMesh->addPartMesh(part);
	}

	return finalMesh;
}

} // Graphics