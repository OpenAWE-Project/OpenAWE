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

#include "src/awe/script/functions.h"
#include "src/awe/script/bytecode.h"

namespace AWE::Script {

void Functions::sendCustomEvent(Context &ctx) {
	const entt::entity caller = ctx.thisEntity;
	const std::string eventName = ctx.getString(0);

	if (caller == entt::null) {
		spdlog::warn("Cannot call custom event without entity, skipping");
		return;
	}

	auto bytecodeIter = ctx.registry.try_get<AWE::Script::BytecodePtr>(caller);
	if (!bytecodeIter) {
		spdlog::error("Bytecode for custom event {} not found", eventName);
		return;
	}

	auto bytecode = *bytecodeIter;
	if (!bytecode->hasEntryPoint(eventName)) {
		spdlog::warn("Custom event {} not found, skipping", eventName);
		return;
	}

	AWE::Script::Context newContext(ctx.functions._registry, ctx.functions);
	bytecode->run(newContext, eventName, caller);
}

}
