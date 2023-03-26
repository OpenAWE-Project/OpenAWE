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

#ifndef OPENAWE_ANIMATION_H
#define OPENAWE_ANIMATION_H

#include <vector>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "src/awe/types.h"

namespace Graphics {

class Animation {
public:
	struct Keyframe {
		float time;
		glm::vec3 position;
		glm::quat rotation;

		Keyframe() : time(0.0f), position(0.0f), rotation(0.0f, glm::vec3(0.0)) {}
	};

	Animation();
	Animation(rid_t rid, const std::string &name = "");

	float getDuration() const;
	const std::string &getName() const;

	bool hasTrackForBone(const std::string &boneName) const;

	glm::mat4 calculateTransformation(const std::string &boneName, float time) const;

private:
	float _duration;
	std::string _name;
	std::map<std::string, std::vector<Keyframe>> _keyframes;
};

} // End of namespace Graphics

#endif //OPENAWE_ANIMATION_H
