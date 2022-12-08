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

#ifndef AWE_MODEL_H
#define AWE_MODEL_H

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/common/readstream.h"

#include "src/graphics/material.h"
#include "src/graphics/mesh.h"
#include "src/graphics/animation.h"

#include "src/awe/types.h"

namespace Graphics {

class Model;
class Skeleton;

typedef std::shared_ptr<Model> ModelPtr;

class Model {
public:
	Model(rid_t rid);
	Model(const std::string &path);
	Model(MeshPtr mesh);
	virtual ~Model();

	void show();
	void hide();

	void setTransform(const glm::mat4 &transform);
	glm::mat4 getTransform() const;
	glm::mat4 getInverseTransform() const;

	MeshPtr getMesh() const;
	void setSkeleton(const Skeleton &skeleton);
	const Skeleton &getSkeleton() const;
	bool hasSkeleton() const;

protected:
	Model();

	MeshPtr _mesh;
	std::unique_ptr<Skeleton> _skeleton;

	glm::mat4 _transform;
	glm::mat4 _invTransform;
};

}

#endif //AWE_MODEL_H
