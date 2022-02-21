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

#ifndef OPENAWE_KEYFRAMER_H
#define OPENAWE_KEYFRAMER_H

#include <vector>
#include <optional>
#include <memory>
#include <map>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "src/awe/types.h"

#include "src/graphics/animation.h"

/*!
 * A key frame, utilized for simple point to point interpolations
 */
struct KeyFrame {
	glm::vec3 position;
	glm::mat3 rotation;
};

/*!
 * Animation which can be run on a keyframer
 */
struct KeyFrameAnimation {
	unsigned int start, end;
	float duration;
	std::optional<GID> nextAnimation;
	std::optional<Graphics::Animation> animation;
};

class KeyFramer;

typedef std::shared_ptr<KeyFramer> KeyFramerPtr;

/*!
 * \brief Class for calculating keyframe animations
 *
 * This class is responsible for calculating keyframe animations. These animations can be build upon either a simple
 * transition between two keyframes or a complex havok based animation
 */
class KeyFramer {
public:
	KeyFramer(
			std::vector<KeyFrame> keyFrames,
			std::map<GID, KeyFrameAnimation> keyframeAnimations,
			unsigned int initialKeyframe
	);

	/*!
	 * Get the current transformation of the keyframer
	 * \return A 4x4 matrix with the current transformation
	 */
	glm::mat4 getTransformation() const;

	glm::mat4 getTranslation() const;
	glm::mat4 getRotation() const;

	bool hasAnimation() const;
	bool isAbsolute() const;

	const std::vector<entt::entity> &getAffectedEntities() const;
	void addAffectedEntity(entt::entity entity);

	/*!
	 * Set a keyframer as parent keyframer
	 * \param parentKeyFramer The keyframer to be set as parent keyframer
	 */
	void setParentKeyFramer(const KeyFramerPtr &parentKeyFramer);

	/*!
	 * Set an animation as the current animation with a starting time
	 * \param keyFrameAnimation The keyframe animation to set
	 * \param time The time to start the animation
	 */
	void setAnimation(const KeyFrameAnimation &keyFrameAnimation, float time);

	/*!
	 * Update the keyframer to the current time
	 * \param time The current time point
	 */
	void update(float time);

private:
	float _start;
	unsigned int _initialKeyframe;
	KeyFramerPtr _parentKeyFramer;
	std::optional<KeyFrameAnimation> _currentAnimation;
	std::vector<entt::entity> _affectedEntities;
	std::vector<KeyFrame> _keyFrames;
	std::map<GID, KeyFrameAnimation> _keyframeAnimations;
	glm::mat4 _transformation, _translation, _rotation;
};

#endif //OPENAWE_KEYFRAMER_H
