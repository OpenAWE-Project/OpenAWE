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

#include "src/engines/awan/functions.h"

#include "src/character.h"
#include "src/utils.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::aiAddAnimate(Functions::Context &ctx) {
	const auto animation = ctx.getEntity(0);

	auto &ai = _registry.get<AI>(ctx.thisEntity);

	ai.cmds.emplace_back(Animate{animation, false, _time});
}

void Functions::aiAddAnimateLooping(Functions::Context &ctx) {
	const auto animation = ctx.getEntity(0);

	auto &ai = _registry.get<AI>(ctx.thisEntity);

	ai.cmds.emplace_back(Animate{animation, true, _time});
}

} // End of namespace Engines::AlanWakesAmericanNightmare
