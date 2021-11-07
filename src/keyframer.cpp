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

#include <map>

#include <spdlog/spdlog.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "src/keyframer.h"

KeyFramer::KeyFramer(std::vector<KeyFrame> keyFrames,  std::map<GID, KeyFrameAnimation> keyframeAnimations, unsigned int initialKeyframe) :
	_keyFrames(keyFrames),
	_keyframeAnimations(keyframeAnimations),
	_initialKeyframe(initialKeyframe),
	_start(0.0f),
	_transformation(glm::identity<glm::mat4>()) {
}

void KeyFramer::setAnimation(const KeyFrameAnimation &keyFrameAnimation, float time) {
	_currentAnimation = keyFrameAnimation;
	_start = time;
}

void KeyFramer::update(float time) {
	if (!_currentAnimation) {
		_transformation = glm::translate(glm::identity<glm::mat4>(), _keyFrames[_initialKeyframe].position) * glm::mat4(_keyFrames[_initialKeyframe].rotation);
		return;
	}

	_transformation = glm::identity<glm::mat4>();

	if (_currentAnimation->animation) {
		// TODO: Complex animations are not currently working
		/*const auto havokAnimation = _currentAnimation->animation;

		_transformation *= havokAnimation->calculateTransformation(
			"Bone00",
			std::min(delta - _start, havokAnimation->getDuration())
		);

		if (delta - _start > havokAnimation->getDuration()) {
			if (_currentAnimation->nextAnimation)
				setAnimation(_keyframeAnimations[*_currentAnimation->nextAnimation], delta);
		}*/
	} else {
		const float duration = _currentAnimation->duration;
		const auto startFrame = _keyFrames[_currentAnimation->start];
		const auto endFrame = _keyFrames[_currentAnimation->end];
		const float factor = std::min(time - _start, duration) / duration;

		glm::vec3 position = glm::mix(
			startFrame.position,
			endFrame.position,
			factor
		);

		glm::mat3 rotation = glm::toMat3(glm::slerp(
			glm::toQuat(startFrame.rotation),
			glm::toQuat(endFrame.rotation),
			factor
		));

		_transformation = glm::identity<glm::mat4>();
		_transformation *= glm::translate(glm::identity<glm::mat4>(), position) * glm::mat4(rotation);

		if (time - _start > duration) {
			if (_currentAnimation->nextAnimation)
				setAnimation(_keyframeAnimations[*_currentAnimation->nextAnimation], time);
		}
	}
}

bool KeyFramer::hasAnimation() const {
	return _currentAnimation.has_value();
}

bool KeyFramer::isAbsolute() const {
	return !_currentAnimation->animation;
}

void KeyFramer::setParentKeyFramer(const KeyFramerPtr &parentKeyFramer) {
	_parentKeyFramer = parentKeyFramer;
}

glm::mat4 KeyFramer::getTransformation() const {
	glm::mat4 transformation = glm::identity<glm::mat4>();

	if (_parentKeyFramer)
		transformation *= _parentKeyFramer->getTransformation();
	transformation *= _transformation;

	return transformation;
}




