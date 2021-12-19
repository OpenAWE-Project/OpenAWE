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

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/strutil.h"

#include "functions.h"

namespace AWE::Script {

Functions::Functions(entt::registry &registry) : _registry(registry), _time(0.0f) {

}

std::optional<Variable> Functions::callObject(entt::entity object, const std::string &functionName, std::vector<Variable> parameters) {
	if (object == entt::null) {
		spdlog::error("Cannot call object function {} with invalid object, skipping", functionName);
		return 0;
	}

	Context ctx{
		_registry,
		object,
		*this,
		parameters,
		_time
	};

	auto fun = getFunction(functionName);
	if (fun)
		fun(ctx);
	else
		spdlog::warn("TODO: Implement object script functions {}", functionName);

	return ctx.ret;
}

std::optional<Variable> Functions::callGlobal(const std::string &name,const std::string &functionName, std::vector<Variable> parameters) {
	Context ctx{
			_registry,
			entt::null,
			*this,
			parameters,
			_time
	};

	const std::string globalFunctionName = fmt::format("{}.{}", Common::toUpper(name), functionName);
	auto fun = getFunction(globalFunctionName);
	if (fun)
		fun(ctx);
	else
		spdlog::warn("TODO: Implement global script functions {}", globalFunctionName);

	return ctx.ret;
}

void Functions::setTime(float time) {
	_time = time;
}

Functions::NativeFunction Functions::getFunction(const std::string &name) {
	auto func = _functions.find(name);
	if (func == _functions.end())
		return nullptr;
	return (*func).second;
}

}
