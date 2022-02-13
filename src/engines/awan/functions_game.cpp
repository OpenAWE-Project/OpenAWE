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

#include "src/awe/script/bytecode.h"

#include "src/engines/awan/functions.h"

#include "src/task.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::activateTask(Functions::Context &ctx) {
	const entt::entity taskEntity = ctx.getEntity(0);

	AWE::Script::Context newContext(_registry, *this);

	Task task = _registry.get<Task>(taskEntity);
	AWE::Script::BytecodePtr bytecode = _registry.get<AWE::Script::BytecodePtr>(taskEntity);
	if (bytecode->hasEntryPoint("OnTaskActivate"))
		bytecode->run(newContext, "OnTaskActivate", taskEntity);
}

void Functions::playMusic(Functions::Context &ctx) {
	const entt::entity sound = ctx.getEntity(0);
}

void Functions::getStoryModeRound(Context &ctx) {
	ctx.ret = static_cast<int32_t>(_engine.getStoryModeRound());
}

void Functions::isTrial(Functions::Context &ctx) {
	ctx.ret = false;
}

void Functions::isTrialModeB(Functions::Context &ctx) {
	ctx.ret = false;
}

void Functions::isManuscriptPageUnlocked(Context &ctx) {
	const entt::entity manuscriptPage = ctx.getEntity(0);

	// TODO
	ctx.ret = false;
}

}