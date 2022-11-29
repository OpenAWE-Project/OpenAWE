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

#ifndef OPENAWE_CONTEXT_H
#define OPENAWE_CONTEXT_H

#include <entt/entt.hpp>

#include "src/common/types.h"

#include "src/awe/types.h"
#include "src/awe/script/functions.h"

namespace AWE::Script {

class Context {
public:
	Context(entt::registry &registry, Functions &functions);

	entt::entity getEntityByGID(const GID &gid);
	void setVariable(entt::entity entity, byte id, Variable variable, std::string &debug);
	Variable getVariable(entt::entity entity, byte id, std::string &debug);

	Functions &getFunctions();

private:
	entt::registry &_registry;
	Functions &_functions;
	std::map<std::string, Functions> _globalObjects;
};

}

#endif //OPENAWE_CONTEXT_H
