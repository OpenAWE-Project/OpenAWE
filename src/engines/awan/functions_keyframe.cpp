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

#include "src/keyframer.h"
#include "src/keyframerprocess.h"

#include "src/engines/awan/functions.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::animate(Functions::Context &ctx) {
	const entt::entity caller = ctx.thisEntity;

	if (caller == entt::null) {
		spdlog::warn("Cannot animate invalid entity, skipping");
		return;
	}

	const entt::entity animation = ctx.getEntity(0);

	const auto keyFramer = _registry.get<KeyFramerPtr>(caller);
	const auto &keyFrameAnimation = _registry.get<KeyFrameAnimation>(animation);

	const bool isAlreadyPlaying = keyFramer->hasAnimation();
	keyFramer->setAnimation(keyFrameAnimation, _time);

	if (!isAlreadyPlaying)
		_scheduler.attach<KeyFramerProcess>(caller, _registry);
	else
		spdlog::warn("Couldn't start keyframer because it is already running");
}

} // End of namespace Engines::AlanWakesAmericanNightmare
