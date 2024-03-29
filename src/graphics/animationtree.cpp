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

	//std::vector<std::string> flagNames;
	std::map<std::string, boost::dynamic_bitset<uint8_t>> flags;
	for (const Common::XML::Node &animationFlags : root.getNodes("Object_AnimationFlags")) {
		const auto flagsName = animationFlags.getNode("Name").content;
		const auto flagsData = Common::split(animationFlags.getNode("Flags").content, std::regex(","));
		const auto noFlagNames = _flagNames.empty();

		assert(flagsData.size() % 2 == 0);

		boost::dynamic_bitset<uint8_t> flagBits;
		for (size_t i = 0; i < flagsData.size() / 2; ++i) {
			const auto name = flagsData[i * 2];
			const auto flag = std::stol(flagsData[i * 2 + 1]) != 0;

			flagBits.push_back(flag);
			if (noFlagNames)
				_flagNames.emplace_back(name);
		}

		flags[flagsName] = flagBits;
	}

	_currentFlags.resize(_flagNames.size());

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
		if (node.hasNode("Flags") && Common::toLower(node.getNode("Flags").content) != "_null_")
			state.flags = flags.at(node.getNode("Flags").content);

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
			for (const auto &stateName : state.states) {
				const auto nestedState = _states.at(stateName);
				if (nestedState.flags.empty())
					return getAnimation(nestedState);

				if (_currentFlags == nestedState.flags) {
					const auto lowerState = getAnimation(nestedState);
					return {{state.weights, lowerState.front().clip}};
				}
			}
			break;
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

void AnimationTree::setFlag(const std::string &name, bool flag) {
	const auto nameIter = std::find(_flagNames.begin(), _flagNames.end(), name);
	if (nameIter == _flagNames.end())
		throw CreateException("Flag \"{}\" does not exist", name);

	const auto index = std::distance(_flagNames.begin(), nameIter);
	_dirty = true;
	_currentFlags[index] = flag;
}

bool AnimationTree::isDirty() const {
	return _dirty;
}

} // End of namespace Graphics