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

TimerProcess::TimerProcess(TimerPtr timer, entt::entity timerEntity, entt::registry &registry,
						   AWE::Script::Context &ctx, AWE::Script::BytecodePtr bytecode)
		: _timer(timer), _timerEntity(timerEntity), _registry(registry), _ctx(ctx), _bytecode(bytecode) {

}

void TimerProcess::update(double delta, void *) {
	const auto startBytecodeOffset = _timer->getStartBytecodeOffset();
	const auto endBytecodeOffset = _timer->getEndBytecodeOffset();

	const float created = _timer->getCreated();
	const float startTime = _timer->getStartTime();
	const float duration = _timer->getDuration();
	if (delta - created >= startTime && _timer->getState() == Timer::kNotStarted) {
		if (startBytecodeOffset) {
			_ctx.getFunctions().setTime(startTime);
			_bytecode->run(_ctx, *startBytecodeOffset, _timerEntity);
		}
		_timer->setState(Timer::kStarted);
	} else if (delta - created >= duration + startTime && _timer->getState() == Timer::kStarted) {
		if (endBytecodeOffset) {
			_ctx.getFunctions().setTime(created + startTime + duration);
			_bytecode->run(_ctx, *endBytecodeOffset, _timerEntity);
		}
		_timer->setState(Timer::kStopped);
		succeed();
	}
}

void TimerProcess::succeeded() {
	auto &timers = _registry.get<std::vector<TimerPtr>>(_timerEntity);

	// Remove the timer from the timer vector
	timers.erase(std::remove(timers.begin(), timers.end(), _timer), timers.end());

	// Remove the entire component, if all timers were stopped
	if (timers.empty())
		_registry.remove<std::vector<TimerPtr>>(_timerEntity);
}
