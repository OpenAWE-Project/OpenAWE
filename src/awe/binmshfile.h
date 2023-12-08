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

#ifndef OPENAWE_BINMSHFILE_H
#define OPENAWE_BINMSHFILE_H

#include <vector>
#include <map>
#include <variant>

#include "src/common/readstream.h"

namespace AWE {

/*!
 * \brief Class for loading binmsh mesh files
 *
 * This class loads a binmsh file and offers several methods for accessing
 * the data of the mesh.
 */
class BINMSHFile {
public:
	enum CullMode {
		kCullNone  = 0,
		kCullFront = 1,
		kCullBack  = 2
	};

	enum BlendMode {
		kBlendNone = 0,
		kBlendMultiply = 1, // TODO: One of the multiply blending means probably another blending mode!!
		kBlendMultiply2 = 3
	};

	enum AttributeUsage {
		kPosition,
		kTexCoord,
		kNormal,
		kColor,
		kTangent,
		kBoneWeight,
		kBoneIndex
	};

	enum AttributeType {
		kFloat3,
		kShort2F,
		kShort4F,
		kByte4F,
		kByte4I
	};

	typedef std::variant<
		float,
		glm::vec2,
		glm::vec3,
		glm::vec4,
		std::string
	> UniformData;

	struct Uniform {
		std::string name;
		UniformData data;
	};

	struct Attribute {
		AttributeUsage usage;
		AttributeType type;
	};

	struct Material {
		std::string shader;
		std::string name;
		uint32_t properties;
		CullMode cullMode;
		BlendMode blendMode;
		std::vector<Uniform> uniforms;
	};

	struct Mesh {
		Material material;
		uint32_t lod;
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indicesOffset;
		uint32_t indicesCount;
		std::vector<Attribute> attributes;
		std::vector<std::string> boneMap;
	};

	BINMSHFile(Common::ReadStream &binmsh);

	const Common::BoundSphere &getBoundSphere() const;

	const Common::BoundBox &getBoundBox() const;

	const std::vector<byte> &getVertexData() const;

	const std::vector<byte> &getIndexData() const;

	const std::vector<Mesh> &getMeshs() const;

	const std::map<std::string, glm::mat4> &getInverseRestTransforms() const;

protected:
	BINMSHFile();

	void load(Common::ReadStream &binmsh);

private:
	Common::BoundSphere _boundSphere;
	Common::BoundBox _boundBox;

	std::vector<byte> _vertexData;
	std::vector<byte> _indexData;

	std::vector<Mesh> _meshs;

	std::map<std::string, glm::mat4> _inverseRestTransforms;
};

} // End of namespace AWE

#endif //OPENAWE_BINMSHFILE_H
