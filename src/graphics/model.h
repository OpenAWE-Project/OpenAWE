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

#include <boost/uuid/uuid.hpp>

#include "src/common/readstream.h"

#include "src/graphics/material.h"
#include "src/graphics/mesh.h"

#include "src/awe/types.h"

namespace Graphics {

class Model;

typedef std::shared_ptr<Model> ModelPtr;

class Model {
public:
	Model(rid_t rid);
	Model(const std::string &path);
	Model(MeshPtr mesh);
	virtual ~Model();

	void show();
	void hide();

	glm::mat3 &getRotation();
	glm::vec3 &getPosition();
	glm::vec3 &getScale();

	MeshPtr getMesh() const;

protected:
	Model();

	MeshPtr _mesh;

	glm::vec3 _position;
	glm::vec3 _scale;
	glm::mat3 _rotation;
};

}

#endif //AWE_MODEL_H
