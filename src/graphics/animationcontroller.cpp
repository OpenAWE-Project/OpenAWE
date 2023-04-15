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

#include <spdlog/spdlog.h>

#include "src/common/exception.h"

#include "src/graphics/animationcontroller.h"

namespace Graphics {

AnimationControllerProcess::AnimationControllerProcess(AnimationControllerPtr &animationController) :
	_animationController(animationController) {
}

void AnimationControllerProcess::update(double delta, void *) {
	_animationController->update(delta);
}

AnimationController::AnimationController(Skeleton &skeleton, float blendTime) :
	_blendTime(blendTime),
	_skeleton(skeleton) {
}

void AnimationController::addAnimation(AnimationPtr animation) {
	_animations[animation->getName()] = animation;
}

void AnimationController::play(const std::string &id, float startTime) {
	const auto nextAnimation = _animations.find(id);
	if (nextAnimation == _animations.end()) {
		spdlog::error("Could not play animation with unknown id {}", id);
		return;
	}

	_lastAnimation = _currentAnimation;
	_currentAnimation = AnimationPart{
		nextAnimation->second,
		kLoop,
		startTime
	};
}

void AnimationController::update(float time) {
	if (!_currentAnimation.animation)
		return;

	const float blendFactor = std::min(time - _currentAnimation.startTime, _blendTime) / _blendTime;
	if (blendFactor >= 1.0f)
		_lastAnimation.animation.reset();

	_skeleton.reset();

	if (_currentAnimation.animation) {
		applyAnimation(_currentAnimation, time, blendFactor);
	}

	if (_lastAnimation.animation) {
		applyAnimation(_lastAnimation, time, 1.0f - blendFactor);
	}
}

void AnimationController::applyAnimation(const AnimationPart &prt, float time, float factor) {
	float currentTime;
	switch (prt.ending) {
		case kLoop:
			currentTime = std::fmod(time - prt.startTime, prt.animation->getDuration());
			break;
		case kStay:
			currentTime = std::max(time, prt.startTime + prt.animation->getDuration());
			break;
		case kNone:
			currentTime = time - prt.startTime;
			if (currentTime - prt.startTime > prt.animation->getDuration()) {
				_lastAnimation = prt;
			}
			break;
	}

	_skeleton.update(
		*prt.animation,
		currentTime,
		factor
	);
}

} // End of namespace Graphics
