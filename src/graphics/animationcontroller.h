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

#ifndef OPENAWE_ANIMATIONCONTROLLER_H
#define OPENAWE_ANIMATIONCONTROLLER_H

#include <memory>

#include <entt/entt.hpp>

#include "src/graphics/skeleton.h"

namespace Graphics {

class AnimationController;

typedef std::shared_ptr<AnimationController> AnimationControllerPtr;

class AnimationControllerProcess : public entt::process<AnimationControllerProcess, double> {
public:
	AnimationControllerProcess(AnimationControllerPtr &animationController);

	void update(double delta, void *);

private:
	AnimationControllerPtr _animationController;
};

class AnimationController {
public:
	AnimationController(Skeleton &skeleton, float blendTime);

	/*!
	 * Add an animation to this animation controller for usage
	 * \param animation A pointer to the animation
	 */
	void addAnimation(AnimationPtr animation);

	/*!
	 * 
	 * \param id
	 * \param loop
	 */
	void play(const std::string &id, float startTime = 0.0f);

	void update(float time);

private:
	struct AnimationPart;

	void applyAnimation(const AnimationPart &prt, float time, float factor = 1.0f);

	enum EndingBehaviour {
		kNone, // Remove the animation from the current animations
		kLoop, // Repeat the animation from the beginning
		kStay  // Stay at the ending position of the animation
	};

	struct AnimationPart {
		AnimationPtr animation;
		EndingBehaviour ending;
		float startTime;
	};

	struct State {
		AnimationPtr animation;
		float startTime;
	};

	const float _blendTime;
	Skeleton &_skeleton;
	AnimationPart _lastAnimation;
	AnimationPart _currentAnimation;
	std::map<std::string, AnimationPtr> _animations;
};

} // End of namespace Graphics

#endif //OPENAWE_ANIMATIONCONTROLLER_H
