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

#ifndef OPENAWE_ANIMATIONTREE_H
#define OPENAWE_ANIMATIONTREE_H

#include <variant>

#include <boost/dynamic_bitset.hpp>

#include "src/common/readstream.h"
#include "src/common/xml.h"

namespace Graphics {

/**
 * \brief Tree for managing Animation states and their transitions
 *
 * This class holds the FSM representing the Animation states of a skeleton with all possible animation states and
 * their transitions and their weight blending. It will be loaded from a subset of Remedys XML format, used in the
 * games.
 */
class AnimationTree {
public:
	struct WeightedAnimation {
		std::vector<float> weights;
		std::string clip;
	};

	explicit AnimationTree(Common::ReadStream &tree);
	explicit AnimationTree(const std::string &file);

	void setFlag(const std::string &name, bool flag);

	bool isDirty() const;
	std::vector<AnimationTree::WeightedAnimation> getCurrentAnimation();

private:
	enum StateType {
		kNested,
		kBlended,
		kSingle
	};

	struct State {
		std::string name;
		StateType type;
		std::string clip;
		std::vector<float> weights;
		boost::dynamic_bitset<uint8_t> flags;
		std::vector<std::string> states;
	};

	void load(Common::ReadStream &tree);
	std::vector<WeightedAnimation> getAnimation(const AnimationTree::State &state) const;

	bool _dirty{};
	std::string _root;
	std::vector<std::string> _flagNames;
	boost::dynamic_bitset<uint8_t> _currentFlags;
	std::map<std::string, State> _states;
};

} // Graphics

#endif //OPENAWE_ANIMATIONTREE_H
