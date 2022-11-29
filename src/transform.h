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

#ifndef OPENAWE_TRANSFORM_H
#define OPENAWE_TRANSFORM_H

#include <optional>

#include <glm/glm.hpp>

#include "src/keyframer.h"

class Transform;

typedef std::shared_ptr<Transform> TransformPtr;

class Transform {
public:
	Transform() = default;
	Transform(glm::vec3 translation, glm::mat3 rotation);

	void setKeyFrameOffset(glm::vec3 position, glm::mat3 rotation);

	void setTranslation(const glm::vec3 &translation);
	void setRotation(const glm::mat3 &rotation);

	void setKeyFramerTransform(const glm::mat4 &keyFrameTransform, bool absolute);

	glm::mat4 getTransformation() const;

	glm::vec3 getTranslation() const;
	glm::mat3 getRotation() const;

private:
	glm::vec3 _translation;
	glm::vec3 _scale;
	glm::mat3 _rotation;

	bool _absoluteKeyFramer;
	glm::mat4 _keyFramerTransform;
	glm::mat4 _localToParent, _parentToLocal;
};


#endif //OPENAWE_TRANSFORM_H
