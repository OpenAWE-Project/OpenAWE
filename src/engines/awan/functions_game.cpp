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

#include "src/awe/script/bytecode.h"

#include "src/engines/awan/functions.h"

#include "src/graphics/sky.h"
#include "src/graphics/gfxman.h"

#include "src/sound/soundman.h"
#include "src/sound/audiostreamfactory.h"
#include "src/sound/music.h"

#include "src/task.h"

namespace Engines::AlanWakesAmericanNightmare {

void Functions::activateTask(Functions::Context &ctx) {
	const entt::entity taskEntity = ctx.getEntity(0);

	AWE::Script::Context newContext(_registry, *this);

	Task task = _registry.get<Task>(taskEntity);
	AWE::Script::BytecodePtr bytecode = _registry.get<AWE::Script::BytecodePtr>(taskEntity);
	if (bytecode->hasEntryPoint("OnTaskActivate"))
		bytecode->run(newContext, "OnTaskActivate", taskEntity);
}

void Functions::playMusic(Functions::Context &ctx) {
	const entt::entity sound = ctx.getEntity(0);

	const auto audioStreamFactory = _registry.get<Sound::AudioStreamFactory>(sound);

	const auto view = _registry.view<Sound::MusicPtr>();
	if (view.empty()) {
		spdlog::error("No available music player to play music, skipping");
		return;
	}

	auto music = _registry.get<Sound::MusicPtr>(view.front());
	music->setMusic(audioStreamFactory.createStream());
}

void Functions::getStoryModeRound(Context &ctx) {
	ctx.ret = static_cast<int32_t>(_engine.getStoryModeRound());
}

void Functions::isTrial(Functions::Context &ctx) {
	ctx.ret = false;
}

void Functions::isTrialModeB(Functions::Context &ctx) {
	ctx.ret = false;
}

void Functions::isManuscriptPageUnlocked(Context &ctx) {
	const entt::entity manuscriptPage = ctx.getEntity(0);

	// TODO
	ctx.ret = false;
}

void Functions::setSunYRotation(Functions::Context &ctx) {
	const auto degree = ctx.getFloat(0);

	const auto skyView = _registry.view<Graphics::SkyPtr>();
	if (skyView.empty()) {
		spdlog::error("Tried to set sun y rotation without an available sky");
		return;
	}

	const auto skyPtr = _registry.get<Graphics::SkyPtr>(skyView.front());
	skyPtr->setSunYRotation(degree);
}

void Functions::setTime(Functions::Context &ctx) {
	const auto hours = ctx.getInt(2);
	const auto minutes = ctx.getInt(1);

	const auto skyView = _registry.view<Graphics::SkyPtr>();
	if (skyView.empty()) {
		spdlog::error("Tried to set time of day without an available sky");
		return;
	}

	auto skyPtr = _registry.get<Graphics::SkyPtr>(skyView.front());
	skyPtr->setTimeOfDay(static_cast<float>(hours) / 24.0 + (static_cast<float>(minutes) / 60.0) * (1.0 / 24.0));
}

void Functions::setAmbiance(Functions::Context &ctx) {
	const auto ambiancePresetName = ctx.getString(1);
	const auto fadeTime = ctx.getFloat(0); // TODO

	GfxMan.setAmbianceState(ambiancePresetName);
}

}