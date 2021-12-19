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

#include "src/common/strutil.h"

#include "src/engine.h"
#include "src/task.h"

Engine::Engine(entt::registry &registry, AWE::Script::Functions *functions) : _registry(registry), _functions(functions) {
	_context = std::make_unique<AWE::Script::Context>(_registry, *_functions);
}

AWE::Script::Functions &Engine::getFunctions() {
	return *_functions;
}

AWE::Script::Context &Engine::getScriptContext() {
	return *_context;
}

Configuration &Engine::getConfiguration() {
	return *_configuration;
}

void Engine::loadEpisode(const std::string &data) {
	std::vector<std::string> parameters = Common::split(data, std::regex(" "));
	std::vector<std::string> episode = Common::split(parameters.back(), std::regex(":"));

	std::string worldName = episode[0];
	std::string episodeName = episode[1];

	if (!_world || _world->getName() != worldName) {
		_world = std::make_unique<World>(_registry, worldName);
		_world->loadGlobal();
	}

	_world->loadEpisode(episodeName);

	// Call OnInit on every object
	auto bytecodeView = _registry.view<AWE::Script::BytecodePtr>();
	for (const auto &item : bytecodeView) {
		auto gid = _registry.get<GID>(item);
		auto bytecode = _registry.get<AWE::Script::BytecodePtr>(item);

		if (!bytecode->hasEntryPoint("OnInit"))
			continue;

		spdlog::debug("Firing OnInit on {} {:x}", gid.type, gid.id);
		bytecode->run(*_context, "OnInit", item);
	}

	// Activate starter tasks
	auto taskView = _registry.view<Task, AWE::Script::BytecodePtr>();
	for (const auto &item : taskView) {
		auto gid = _registry.get<GID>(item);
		auto task = _registry.get<Task>(item);
		auto bytecode = _registry.get<AWE::Script::BytecodePtr>(item);

		if (!task.isActiveOnStartup())
			continue;

		spdlog::debug("Firing OnTaskActivate on {} {:x}", gid.type, gid.id);
		bytecode->run(*_context, "OnTaskActivate", item);
	}
}
