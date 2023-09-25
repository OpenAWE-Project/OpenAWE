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
#include "src/common/exception.h"

#include "functions.h"

namespace AWE::Script {

Functions::Functions(entt::registry &registry, entt::scheduler<double> &scheduler) : _registry(registry), _scheduler(scheduler), _time(0.0f) {

}

std::optional<Variable> Functions::callObject(
		entt::entity object,
		const std::string &functionName,
		std::vector<Variable> parameters,
		const std::shared_ptr<DPFile> &dp
) {
	if (object == entt::null) {
		spdlog::error("Cannot call object function {} with invalid object, skipping", functionName);
		return 0;
	}

	Context ctx{
		object,
		dp,
		std::move(parameters)
	};

	callFunction(functionName, ctx);

	return ctx.ret;
}

std::optional<Variable> Functions::callGlobal(
		const std::string &name,
		const std::string &functionName,
		std::vector<Variable> parameters,
		const std::shared_ptr<DPFile> &dp
) {
	Context ctx{
			entt::null,
			dp,
			std::move(parameters)
	};

	const std::string globalFunctionName = fmt::format("{}.{}", Common::toUpper(name), functionName);
	callFunction(globalFunctionName, ctx);

	return ctx.ret;
}

void Functions::setTime(float time) {
	_time = time;
}

std::string Functions::getFunctionString(
		const std::string &name,
		const std::vector<ParameterType> &signature,
		const std::vector<Variable> &parameters,
		const std::shared_ptr<DPFile> &dp
) const {
	std::vector<std::string> parameterValues;
	for (size_t i = 0; i < parameters.size(); ++i) {
		std::string valueString;

		switch (signature[i]) {
			case kFloat:
				valueString = fmt::format("{:f}", std::get<Number>(parameters[i]).floatingPoint);
				break;

			case kInt:
				valueString = fmt::format("{}", std::get<Number>(parameters[i]).integer);
				break;

			case kBool:
				valueString = fmt::format("{}", static_cast<bool>(std::get<Number>(parameters[i]).integer));
				break;

			case kString:
				valueString = fmt::format("\"{}\"", dp->getString(std::get<Number>(parameters[i]).integer));
				break;

			case kEntity: {
				const auto entity = static_cast<unsigned int>(std::get<entt::entity>(parameters[i]));
				if (entity == entt::null)
					valueString = "<null>";
				else
					valueString = fmt::format("<{}>", entity);
				break;
			}

			default:
				valueString = "<invalid>";
				break;
		}

		parameterValues.emplace_back(valueString);
	}

	return fmt::format("{}({})", name, fmt::join(parameterValues, ", "));
}

void Functions::callFunction(const std::string &name, Context &ctx) {
	auto func = _functions.find(name);
	if (func == _functions.end()) {
		std::reverse(ctx.parameters.begin(), ctx.parameters.end());
		spdlog::warn("TODO: Implement script functions {}", name);
		return;
	} else if (!func->second.func) {
		std::reverse(ctx.parameters.begin(), ctx.parameters.end());
		spdlog::warn(
				"TODO: Implement script functions {}", getFunctionString(name, func->second.signature, ctx.parameters, ctx.dp)
		);
		return;
	}

	auto fun = (*func).second.func;
	assert(fun);
	fun(this, ctx);
}

}
