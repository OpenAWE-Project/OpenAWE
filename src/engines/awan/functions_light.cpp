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

#include "src/graphics/light.h"

#include "src/engines/awan/functions.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::enablePointLight(Context &ctx) {
	const auto pointlightEntity = ctx.thisEntity;
	const bool enable = ctx.getInt(0) != 0;

	_registry.get<Graphics::Light>(pointlightEntity).setEnabled(enable);
}

} // End of namespace Engines::AlanWakesAmericanNightmare
