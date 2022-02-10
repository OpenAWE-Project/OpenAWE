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

#include "src/timerprocess.h"

TimerProcess::TimerProcess(Engine &engine, entt::registry &registry) : _engine(engine), _registry(registry) {
}

void TimerProcess::update(float delta, void *) {
	const auto timerView = _registry.view<std::vector<TimerPtr>, AWE::Script::BytecodePtr>();

	for (const auto &view: timerView) {
		auto timers = _registry.get<std::vector<TimerPtr>>(view);
		auto bytecode = _registry.get<AWE::Script::BytecodePtr>(view);

		for (const auto &timer: timers) {
			const auto startBytecodeOffset = timer->getStartBytecodeOffset();
			const auto endBytecodeOffset = timer->getEndBytecodeOffset();

			const float created = timer->getCreated();
			const float startTime = timer->getStartTime();
			const float duration = timer->getDuration();
			if (delta - created >= startTime && timer->getState() == Timer::kNotStarted) {
				if (startBytecodeOffset)
					bytecode->run(_engine.getScriptContext(), *startBytecodeOffset, view);
				timer->setState(Timer::kStarted);
			} else if (delta - created >= duration + startTime && timer->getState() == Timer::kStarted) {
				if (endBytecodeOffset)
					bytecode->run(_engine.getScriptContext(), *endBytecodeOffset, view);
				timer->setState(Timer::kStopped);
			}
		}

		// Remove all timers which are stopped
		std::remove_if(timers.begin(), timers.end(), [](const auto &timer) -> bool {
			return timer->getState() == Timer::kStopped;
		});

		// Remove the entire component, if all timers were stopped
		if (timers.empty())
			_registry.remove<std::vector<TimerPtr>>(view);
	}
}
