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

#ifndef OPENAWE_SKELETON_H
#define OPENAWE_SKELETON_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "src/awe/types.h"

#include "src/graphics/animation.h"

namespace Graphics {

class PointList;

/*!
 * \brief Class for handling skeletons for skeletal animation
 *
 * This class handles a set of bones, used to calculate the current bone transformation matrices of them and provide
 * them in list of reduced skinning matrices.
 */
class Skeleton {
public:
	Skeleton() = default;

	/*!
	 * Load a skeleton from a havok file specifiede by a resource id
	 *
	 * \param rid The resource id of the havok file to load
	 */
	Skeleton(rid_t rid);
	Skeleton(const Skeleton &skeleton);

	Skeleton &operator=(const Skeleton &skeleton);

	/*!
	 * Reset all bone transformations to zero, to enable the application of new transforms
	 */
	void reset();

	/*!
	 * Resets the bone transformations to the default T or A Pose.
	 */
	void resetDefault();

	/*!
	 * Update the state of the skeleton with a specific animation and a specific time
	 *
	 * \param animation The animation with which to modify the skeleton
	 * \param time The time at which to apply the animation
	 * \param factor The factor on which the skeleton will be modified
	 * \param weights The weights of how to apply the animation weighted to the bones
	 */
	void update(const Animation &animation, float time, float factor = 1.0f, const std::vector<float> &weights = {});

	/*!
	 * Get the name of the skeleton
	 *
	 * \return The name of the skeleton
	 */
	const std::string &getName() const;

	/*!
	 * Get the skinning matrices for a certain set of bones
	 *
	 * \param indices The bone names to get the skinning matrices for
	 * \return A vector of the requested skinning matrices
	 */
	std::vector<glm::mat4x3> getSkinningMatrices(std::vector<std::string> indices) const;

	/*!
	 * Set the inverse transform matrices for this skeleton
	 *
	 * \param inverseTransform A map which associates a bone name with its inverse transformation matrix
	 */
	void setInverseTransform(const std::map<std::string, glm::mat4> &inverseTransform);

private:
	struct Bone {
		std::string name;
		short parent;
		glm::vec3 translation;
		glm::quat rotation;
	};

	std::vector<Bone> _bones;
	std::map<std::string, glm::mat4> _transformations;
	std::map<std::string, glm::mat4> _inverseTransform;
	std::string _name;
};

} // End of namespace Graphics

#endif //OPENAWE_SKELETON_H
