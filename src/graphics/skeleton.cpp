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

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "src/common/readstream.h"
#include "src/common/exception.h"

#include "src/graphics/skeleton.h"

#include "src/awe/havokfile.h"
#include "src/awe/resman.h"

namespace Graphics {

Skeleton::Skeleton(rid_t rid) {
	std::unique_ptr<Common::ReadStream> havok(ResMan.getResource(rid));
	if (!havok)
		throw Common::Exception("Havok file for animation not found with the rid {:x}", rid);

	AWE::HavokFile havokFile(*havok);
	const auto animationContainer = havokFile.getAnimationContainer();
	if (animationContainer.skeletons.empty())
		throw Common::Exception("No animations in havok file");

	const auto skeleton = havokFile.getSkeleton(animationContainer.skeletons[0]);
	_name = skeleton.name;

	for (const auto &bone : skeleton.bones) {
		Bone newBone {
			bone.name,
			bone.parentIndex,
			bone.position,
			bone.rotation
		};

		_bones.emplace_back(newBone);
	}

	for (const auto &bone: _bones) {
		_transformations.insert(std::make_pair(bone.name, glm::identity<glm::mat4>()));
	}
}

Skeleton::Skeleton(const Skeleton &skeleton) {
	_name = skeleton.getName();
	_bones = skeleton._bones;

	for (const auto &bone: _bones) {
		_transformations.insert(std::make_pair(bone.name, glm::identity<glm::mat4>()));
	}
}

Skeleton &Skeleton::operator=(const Skeleton &skeleton) {
	_name = skeleton._name;
	_bones = skeleton._bones;

	for (const auto &bone: _bones) {
		_transformations.insert(std::make_pair(bone.name, glm::identity<glm::mat4>()));
	}

	return *this;
}

void Skeleton::reset() {
	for (auto &transformation: _transformations) {
		transformation.second = glm::zero<glm::mat4>();
	}
}

void Skeleton::resetDefault() {
	for (auto &transformation: _transformations) {
		transformation.second = glm::identity<glm::mat4>();
	}
}

void Skeleton::update(const Animation &animation, float time, float factor, const std::vector<float> &weights) {
	std::vector<glm::mat4> animationTransforms(_bones.size());

	for (unsigned int i = 0; i < _bones.size(); ++i) {
		const auto &bone = _bones[i];
		if (animation.hasTrackForBone(bone.name))
			// If the animation defines a track for the bone, calculate the transformation
			animationTransforms[i] = animation.calculateTransformation(bone.name, time);
		else
			// If the animation defines no track for the bone, calculate the transformation
			animationTransforms[i] =
				glm::translate(bone.translation) *
				glm::toMat4(bone.rotation);
	}

	for (unsigned int i = 0; i < _bones.size(); ++i) {
		const auto &bone = _bones[i];
		const auto weight = weights.empty() ? 1.0f : weights[std::min<size_t>(i, weights.size() - 1)];
		auto transformation = glm::identity<glm::mat4>();

		// Move the point back to its child position
		short parentIndex = bone.parent;
		while (parentIndex >= 0) {
			const auto &parentBone = _bones[parentIndex];

			transformation =
					animationTransforms[parentIndex] *
					transformation;

			parentIndex = parentBone.parent;
		}

		// Move the point according to the transformation
		transformation *= animationTransforms[i];

		// Move the point to the skeleton origin
		const auto inverseTransform = _inverseTransform.find(bone.name);
		if (inverseTransform != _inverseTransform.end())
			transformation *= inverseTransform->second;

		_transformations[bone.name] += weight * factor * transformation;
	}
}

const std::string &Skeleton::getName() const {
	return _name;
}

std::vector<glm::mat4x3> Skeleton::getSkinningMatrices(std::vector<std::string> indices) const {
	std::vector<glm::mat4x3> transformation(indices.size());
	for (unsigned int i = 0; i < indices.size(); ++i) {
		transformation[i] = _transformations.at(indices[i]);
	}

	for (unsigned int i = 0; i < 64 - transformation.size(); ++i) {
		transformation.emplace_back(glm::identity<glm::mat3x4>());
	}
	
	return transformation;
}

void Skeleton::setInverseTransform(const std::map<std::string, glm::mat4> &inverseTransform) {
	_inverseTransform = inverseTransform;
}

} // End of namespace Graphics
