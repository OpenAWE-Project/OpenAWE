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

namespace Engines::AlanWakesAmericanNightmare {

void Functions::callFunction(const std::string &name, Context &ctx) {
	auto func = _functions.find(name);
	if (func == _functions.end()) {
		AWE::Script::Functions::callFunction(name, ctx);
		return;
	} else if (!func->second.func) {
		std::reverse(ctx.parameters.begin(), ctx.parameters.end());
		spdlog::warn(
				"TODO: Implement script functions {}", getFunctionString(name, func->second.signature, ctx.parameters, ctx.dp)
		);
		return;
	}

	auto fun =  (*func).second.func;
	fun(this, ctx);
}

Engine &Functions::getEngine() {
	return _engine;
}

}
