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

#include "src/graphics/animationcontroller.h"
#include "src/graphics/animation.h"
#include "src/graphics/model.h"

#include <spdlog/spdlog.h>

#include "src/aiprocess.h"

AIProcess::AIProcess(entt::registry &registry, entt::entity character) : _registry(registry), _character(character) {

}

void AIProcess::update(double now, void *) {
	// Entities can be destroyed while their process is still attached to the
	// scheduler (world switches clear the registry); bail out instead of
	// dereferencing a dead entity handle.
	if (!_registry.valid(_character)) {
		succeed();
		return;
	}

	// Not every character entity carries an AI component; nothing to sequence.
	auto *ai = _registry.try_get<AI>(_character);
	if (!ai) {
		succeed();
		return;
	}

	// If there is currently command get the next available one
	if (!_currentCmd) {
		if (ai->cmds.empty())
			return;

		_currentCmd = ai->cmds.front();
		startCommand(now);
	}

	// After we have potentially started a command update it
	switch (_currentCmd->index()) {
		case 0: {
			const auto animate = std::get<Animate>(*_currentCmd);
			const auto animation = _registry.try_get<Graphics::AnimationPtr>(animate.animation);
			if (!animation) {
				spdlog::warn("AIProcess: Animate command references a dead animation entity, dropping it");
				_currentCmd.reset();
				ai->cmds.pop_front();
				break;
			}
			// _cmdStarted is stamped from the scheduler clock (the same clock as
			// now); the previous code stamped it from Animate::startTime, which
			// comes from the script clock, mixing two different time domains.
			if (now - _cmdStarted >= (*animation)->getDuration() && !animate.looping) {
				_currentCmd.reset();
				ai->cmds.pop_front();
			}
			break;
		}
	}
}

void AIProcess::startCommand(double now) {
	switch (_currentCmd->index()) {
		case 0: { // Animate
			const auto animate = std::get<Animate>(*_currentCmd);
			const auto controller = _registry.try_get<Graphics::AnimationControllerPtr>(_character);
			const auto animation = _registry.try_get<Graphics::AnimationPtr>(animate.animation);
			if (!controller || !animation) {
				spdlog::warn("AIProcess: cannot start Animate command, missing animation controller or animation, dropping it");
				_currentCmd.reset();
				_registry.try_get<AI>(_character)->cmds.pop_front();
				break;
			}

			_cmdStarted = now;
			(*controller)->play(*animation, animate.looping, animate.startTime);
			break;
		}
	}
}
