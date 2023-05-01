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

#include "src/video/playerprocess.h"

#include "src/graphics/animationcontroller.h"

#include "src/engine.h"
#include "src/task.h"
#include "src/utils.h"

Engine::Engine(entt::registry &registry,  const LocaleConfig::Config &config, AWE::Script::Functions *functions) :
	_registry(registry),
	_functions(functions),
	_playerController(_registry),
	_localeConfig(config) {
	_context = std::make_unique<AWE::Script::Context>(_registry, *_functions);
	_player = std::make_unique<Video::Player>();
	_videoPlane = std::make_unique<Graphics::FullScreenPlane>();
	_videoPlane->setTexture(_player->getTexture());
}

Engine::~Engine() {
}

AWE::Script::Functions &Engine::getFunctions() {
	return *_functions;
}

AWE::Script::Context &Engine::getScriptContext() {
	return *_context;
}

void Engine::loadVideo(const std::string &videoFile) {
	_player->load(videoFile);
	_player->setAudioTracks({
		0,
		_localeConfig.voiceoverChannel
	});
}

void Engine::playVideo() {
	_videoPlane->show();
	_player->play();
	_scheduler
		.attach<Video::PlayerProcess>(*_player)
		.then([&](auto delta, void *, auto succeed, auto fail){
			_videoPlane->hide();
			succeed();
		});
}

void Engine::update(float time) {
	_playerController.update(time);
}

void Engine::writeConfiguration() {
	if (_configuration)
		_configuration->write();
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

	// Start animation controller process
	auto animControllerView = _registry.view<Graphics::AnimationControllerPtr>();
	for (const auto &controllerEntity: animControllerView) {
		auto controller = _registry.get<Graphics::AnimationControllerPtr>(controllerEntity);

		_scheduler.attach<Graphics::AnimationControllerProcess>(controller);
	}

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

		if (!task.getPlayerCharacter().isNil())
			_playerController.setPlayerCharacter(getEntityByGID(_registry, task.getPlayerCharacter()));

		spdlog::debug("Firing OnTaskActivate on {} {:x}", gid.type, gid.id);
		bytecode->run(*_context, "OnTaskActivate", item);
	}
}

entt::scheduler<double> & Engine::getScheduler() {
	return _scheduler;
}
