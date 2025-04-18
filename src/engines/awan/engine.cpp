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

#include "src/common/threadpool.h"
#include "src/common/strutil.h"

#include "src/awe/resman.h"
#include "src/awe/script/bytecode.h"

#include "src/engines/awan/engine.h"
#include "src/engines/awan/configuration.h"

#include "src/sound/music.h"

#include "src/task.h"

namespace Engines::AlanWakesAmericanNightmare {

Engine::Engine(entt::registry &registry, const LocaleConfig::Config &config) :
	::Engine(registry, config, new Functions(registry, *this)) {
	_storyModeRound = 1;
	_configuration.reset(new AlanWakesAmericanNightmare::Configuration());
	_configuration->read();
}

void Engine::init() {
	entt::entity musicPlayer = _registry.create();
	_registry.emplace<Sound::MusicPtr>(musicPlayer, std::make_shared<Sound::Music>());

	loadEpisode("round:1 gameworld:scene1_reststop");
}

unsigned int Engine::getStoryModeRound() const {
	return _storyModeRound;
}

void Engine::loadEpisode(const std::string &data) {
	const std::vector<std::string> parameters = Common::split(data, std::regex(" "));
	const std::vector<std::string> episode = Common::split(parameters.back(), std::regex(":"));

	::Engine::loadEpisode(parameters.back());

	// Parse story mode round
	const std::vector<std::string> split = Common::split(parameters.front(), std::regex(":"));
	if (split[0] == "round") {
		_storyModeRound = std::stoul(split[1]);
	} else {
		_storyModeRound = 0;
		return;
	}
}

void Engine::initEpisode() {
	::Engine::initEpisode();

	// Activate starter tasks
	auto taskView = _registry.view<Task, AWE::Script::BytecodePtr>();
	for (const auto &item : taskView) {
		auto gid = _registry.get<GID>(item);
		auto task = _registry.get<Task>(item);
		auto bytecode = _registry.get<AWE::Script::BytecodePtr>(item);

		if (!task.isActiveOnStartupRound(_storyModeRound - 1))
			continue;

		if (!bytecode->hasEntryPoint("OnTaskActivate"))
			continue;

		spdlog::debug("Firing OnTaskActivate on {} {} {:x}", task.getName(), gid.type, gid.id);

		AWE::Script::Context context(_registry, *_functions);
		bytecode->run(context, "OnTaskActivate", item);
	}
}

const char *Engine::getName() const {
	return "Alan Wakes American Nightmare";
}

}
