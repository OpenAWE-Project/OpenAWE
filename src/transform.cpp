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

#include <algorithm>
#include <utility>

#include "transform.h"

Transform::Transform(glm::vec3 translation, glm::mat3 rotation) :
	_translation(translation),
	_rotation(rotation),
	_parentTransform(glm::identity<glm::mat4>()),
	_absoluteKeyFramer(false),
	_keyFramerTransform(glm::identity<glm::mat4>()) {
}

void Transform::setKeyFrameOffset(glm::vec3 position, glm::mat3 rotation) {
	_localToParent = glm::translate(glm::identity<glm::mat4>(), position) * glm::mat4(rotation);
	_parentToLocal = glm::inverse(_localToParent);
}

glm::vec3 Transform::getTranslation() const {
	return _translation;
}

glm::mat3 Transform::getRotation() const {
	return _rotation;
}

void Transform::setTranslation(const glm::vec3 &translation) {
	_translation = translation;
}

void Transform::setRotation(const glm::mat3 &rotation) {
	_rotation = rotation;
}

void Transform::setKeyFramerTransform(const glm::mat4 &keyFramerTransform, bool absolute) {
	_keyFramerTransform = glm::identity<glm::mat4>();
	_absoluteKeyFramer = absolute;
	if (!absolute) {
		_keyFramerTransform *= _parentToLocal;
	}
	_keyFramerTransform *= keyFramerTransform;
	_keyFramerTransform *= _localToParent;
}

glm::mat4 Transform::getTransformation() const {
	auto transform = _parentTransform;

	if (!_absoluteKeyFramer)
		transform *= glm::translate(transform, _translation) * glm::mat4(_rotation);
	transform *= _keyFramerTransform;

	return transform;
}

void Transform::setParentTransform(const glm::mat4 &parentTransform) {
	_parentTransform = parentTransform;
}
