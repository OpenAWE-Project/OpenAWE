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

#include "src/common/exception.h"

#include "src/engines/awan/functions.h"

#include "src/timer.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::startTimer(Context &ctx) {
	const entt::entity caller = ctx.thisEntity;

	const uint32_t stopEntryPoint = ctx.getInt(0);
	const float duration = ctx.getFloat(1);
	const uint32_t startEntryPoint = ctx.getInt(2);

	if (caller == entt::null) {
		spdlog::warn("Cannot start timer on invalid entity, skipping");
		return;
	}

	auto &timers = ctx.registry.get_or_emplace<std::vector<TimerPtr>>(caller);
	auto timer = timers.emplace_back(std::make_shared<Timer>(ctx.time, duration));
	if (stopEntryPoint != 0xFFFFFFFF)
		timer->setEndBytecodeOffset(stopEntryPoint);
	if (startEntryPoint != 0xFFFFFFFF)
		timer->setStartBytecodeOffset(startEntryPoint);
}

void Functions::startTimerWithDuration(Functions::Context &ctx) {
	const entt::entity caller = ctx.thisEntity;

	const float startTime = ctx.getFloat(0);
	const uint32_t stopEntryPoint = ctx.getInt(1);
	const float duration = ctx.getFloat(2);
	const uint32_t startEntryPoint = ctx.getInt(3);

	if (caller == entt::null) {
		spdlog::warn("Cannot start timer on invalid entity, skipping");
		return;
	}

	auto &timers = ctx.registry.get_or_emplace<std::vector<TimerPtr>>(caller);
	auto timer = timers.emplace_back(std::make_shared<Timer>(ctx.time, startTime, duration));
	if (stopEntryPoint != 0xFFFFFFFF)
		timer->setEndBytecodeOffset(stopEntryPoint);
	if (startEntryPoint != 0xFFFFFFFF)
		timer->setStartBytecodeOffset(startEntryPoint);
}

} // End of namespace Engines::AlanWakesAmericanNightmare
