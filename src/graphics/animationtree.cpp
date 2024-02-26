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

#include "src/common/xml.h"
#include "src/common/strutil.h"
#include "src/common/exception.h"
#include "src/common/readfile.h"

#include "src/graphics/animationtree.h"

namespace Graphics {

AnimationTree::AnimationTree(Common::ReadStream &tree) : _dirty(true) {
	load(tree);
}

AnimationTree::AnimationTree(const std::string &file) : _dirty(true) {
	std::unique_ptr<Common::ReadStream> tree = std::make_unique<Common::ReadFile>(file);
	if (!tree)
		throw CreateException("File {} not found", file);

	load(*tree);
}

void AnimationTree::load(Common::ReadStream &tree) {
	Common::XML xml;
	xml.read(tree);

	const auto &root = xml.getRootNode();
	if (root.name != "AnimationStateGraph")
		throw CreateException("Invalid animation graph file expected \"AnimationStateGraph\", got \"{}\"", root.name);

	std::map<std::string, std::vector<float>> weightSets;
	for (const Common::XML::Node &blendWeights: root.getNodes("Object_AnimationBlendWeights")) {
		const auto weightsName = blendWeights.getNode("Name").content;
		const auto weights = Common::split(blendWeights.getNode("Weights").content, std::regex(","));

		assert(weights.size() % 2 == 0);

		std::vector<float> weightValues;
		for (size_t i = 0; i < weights.size() / 2; ++i) {
			const auto name = weights[i * 2];
			const auto weight = std::stol(weights[i * 2 + 1]);

			weightValues.emplace_back(static_cast<float>(weight) / 255.0f);
		}

		weightSets[weightsName] = weightValues;
	}

	for (const Common::XML::Node &node: root.getNodes("Object_AnimationState")) {
		State state{};

		state.name = node.getNode("Name").content;
		if (node.hasNode("AnimationClip"))
			state.clip = node.getNode("AnimationClip").content;
		if (node.hasNode("BlendWeights") && node.content != "_null_")
			state.weights = weightSets[node.getNode("BlendWeights").content];
		if (node.hasNode("Root") && Common::toLower(node.getNode("Root").content) == "true")
			_root = state.name;

		const auto typeString = node.getNode("Type").content;
		if (typeString == "Nested")
			state.type = kNested;
		else if (typeString == "Blended")
			state.type = kBlended;
		else if (typeString == "Single")
			state.type = kSingle;
		else
			throw CreateException("Invalid animation state type {}", typeString);

		for (const auto &nestedState : node.getNodes("AnimationState")) {
			state.states.emplace_back(nestedState.getString("Name"));
		}

		_states[state.name] = state;
	}
}

std::vector<AnimationTree::WeightedAnimation> AnimationTree::getCurrentAnimation() {
	_dirty = false;
	return getAnimation(_states.at(_root));
}

std::vector<AnimationTree::WeightedAnimation> AnimationTree::getAnimation(const AnimationTree::State &state) const {
	switch (state.type) {
		case kNested: {
			// TODO: In the future, this should be selected by condition
			return getAnimation(_states.at(state.states.front()));
		}

		case kBlended: {
			std::vector<WeightedAnimation> anims;
			for (const auto &childStateName: state.states) {
				const auto childAnims = getAnimation(_states.at(childStateName));
				anims.insert(anims.end(), childAnims.cbegin(), childAnims.cend());
			}

			return anims;
		}

		case kSingle: {
			return {{state.weights, state.clip}};
		}
	}

	return {};
}

bool AnimationTree::isDirty() const {
	return _dirty;
}

} // End of namespace Graphics