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

#ifndef AWE_MESH_H
#define AWE_MESH_H

#include <memory>

#include "src/common/uuid.h"

#include "src/graphics/material.h"
#include "src/graphics/buffer.h"
#include "src/graphics/attributeobject.h"

namespace Graphics {

class Mesh;

typedef std::shared_ptr<Mesh> MeshPtr;

class Mesh {
public:
	enum RenderType {
		kTriangles,
		kQuads,
		kLines,
		kPoints,
		kPatches
	};

	struct PartMesh {
		BufferPtr vertexData;
		AttributeObjectPtr vertexAttributes;
		RenderType renderType;
		Material material;
		bool wireframe{false};
		unsigned int offset{}, length{};
	};

	Mesh();

	[[nodiscard]] BufferPtr getIndices() const;

	[[nodiscard]] const std::vector<PartMesh> &getMeshs() const;
	const std::map<std::string, glm::mat3x4> & getInitialJointPositions() const;

	void setIndices(BufferPtr indices);
	void addPartMesh(const PartMesh &partMesh);

protected:
	std::vector<PartMesh> _meshs;
	std::map<std::string, glm::mat3x4> _initialPose;

	BufferPtr _indices;
};

} // End of namespace Graphics

#endif //AWE_MESH_H
