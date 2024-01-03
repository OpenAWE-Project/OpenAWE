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

#include "src/character.h"
#include "src/aiprocess.h"

AIProcess::AIProcess(entt::registry &registry, entt::entity character) : _registry(registry), _character(character) {

}

void AIProcess::update(double delta, void *) {
	auto ai = _registry.get<AI>(_character);

	if (ai.cmds.empty())
		return;

	const auto &currentCmd = ai.cmds.front();

	switch (currentCmd.index()) {
		case 0: { // Animate
			const auto animate = std::get<Animate>(currentCmd);
			const auto controller = _registry.get<Graphics::AnimationControllerPtr>(_character);

			auto animation = _registry.get<Graphics::AnimationPtr>(animate.animation);
			controller->play(animation, animate.looping);
			break;
		}
	}
}
