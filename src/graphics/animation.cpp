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

#include <memory>
#include <stdexcept>

#include <algorithm>

#include <spdlog/spdlog.h>

#include <fmt/format.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "src/common/readstream.h"
#include "src/common/exception.h"

#include "src/awe/resman.h"
#include "src/awe/havokfile.h"

#include "src/graphics/animation.h"

namespace Graphics {

Animation::Animation() {
}

Animation::Animation(rid_t rid) {
	std::unique_ptr<Common::ReadStream> havok(ResMan.getResource(rid));
	if (!havok)
		throw Common::Exception(fmt::format("Havok file for animation not found with the rid {:x}", rid));

	AWE::HavokFile havokFile(*havok);
	const auto animationContainer = havokFile.getAnimationContainer();
	if(animationContainer.animations.empty())
		throw std::runtime_error("No animations in havok file");

	const auto animation = havokFile.getAnimation(animationContainer.animations[0]);
	_duration = animation.duration;

	std::map<size_t, std::string> trackToBoneName;
	for (const auto &toTrack : animation.boneToTrack) {
		trackToBoneName[toTrack.second] = toTrack.first;
	}

	const float durationBlock = animation.blockDuration;
	float blockOffset = 0.0f;
	for (const auto &trackBlock : animation.tracks) {
		for (int i = 0; i < trackBlock.size(); ++i) {
			const auto track = trackBlock[i];
			const size_t numFrames = std::max(track.positions.size(), track.rotations.size());
			std::vector<Keyframe> keyframes(numFrames);

			if (!track.positions.empty())
				for (int j = 0; j < numFrames; ++j) {
					keyframes[j].position = track.positions[j % track.positions.size()];
				}

			if (!track.rotations.empty())
				for (int j = 0; j < numFrames; ++j) {
					keyframes[j].rotation = track.rotations[j % track.rotations.size()];
				}

			for (int j = 0; j < numFrames; ++j) {
				keyframes[j].time =	animation.frameDuration	* static_cast<float>(j)	+ blockOffset;
			}

			for (const auto &keyframe : keyframes) {
				const std::string &boneName = trackToBoneName[i];
				spdlog::trace(
					"Keyframe {} {:.5f} {} {}",
					boneName,
					keyframe.time,
					glm::to_string(keyframe.rotation),
					glm::to_string(keyframe.position)
				);
				_keyframes[boneName].emplace_back(keyframe);
			}
		}

		blockOffset += durationBlock;
	}
}

float Animation::getDuration() const {
	return _duration;
}

bool Animation::hasTrackForBone(const std::string &boneName) const {
	return _keyframes.find(boneName) != _keyframes.end();
}

glm::mat4 Animation::calculateTransformation(const std::string &name, float time) const {
	auto transform = glm::identity<glm::mat4>();

	const float factor = std::clamp(time, 0.0f, _duration) / _duration;

	if (_keyframes.find(name) == _keyframes.end())
		return transform;

	const std::vector<Keyframe> &keyframes = _keyframes.at(name);
	if (keyframes.size() == 1) {
		const Keyframe staticKeyframe = keyframes[0];
		transform *= glm::translate(staticKeyframe.position);
		transform *= glm::toMat4(staticKeyframe.rotation);
	}

	Keyframe lastKeyframe;
	for (const auto &keyframe : keyframes) {
		if (time < keyframe.time) {
			transform *= glm::translate(
				glm::identity<glm::mat4>(),
				glm::mix(
					lastKeyframe.position,
					keyframe.position,
					factor
				)
			);

			transform *= glm::toMat4(glm::slerp(
				lastKeyframe.rotation,
				keyframe.rotation,
				factor
			));
			break;
		}

		lastKeyframe = keyframe;
	}

	return transform;
}

} // End of namespace Graphics
