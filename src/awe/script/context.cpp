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

#include <spdlog/spdlog.h>

#include <utility>

#include "src/awe/script/context.h"
#include "src/awe/script/bytecode.h"
#include "src/awe/script/variablestore.h"

namespace AWE::Script {

AWE::Script::Context::Context(entt::registry &registry, Functions &functions) : _registry(registry), _functions(functions) {
}

entt::entity AWE::Script::Context::getEntityByGID(const GID &gid) {
	entt::entity result = entt::null;
	auto gidView = _registry.view<GID>();

	for (const auto &entity : gidView) {
		GID gid2 = _registry.get<GID>(entity);
		if (gid2 == gid) {
			result = entity;
			break;
		}
	}

	if (result == entt::null)
		spdlog::warn("Entity {} {:x} not found, returning null entity", gid.type, gid.id);

	return result;
}

Functions &Context::getFunctions() {
	return _functions;
}

void Context::setVariable(entt::entity entity, byte id, Variable variable, std::string &debug) {
	const auto variableStore = _registry.get<VariableStorePtr>(entity);
	debug = variableStore->getDebugName(id);
	variableStore->setVariable(id, std::move(variable));
}

Variable Context::getVariable(entt::entity entity, byte id, std::string &debug) {
	const auto variableStore = _registry.get<VariableStorePtr>(entity);
	debug = variableStore->getDebugName(id);
	return variableStore->getVariable(id);
}

}
