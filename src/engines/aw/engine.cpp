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

#include "src/engines/aw/engine.h"
#include "src/engines/aw/functions.h"

#include "src/sound/music.h"

namespace Engines::AlanWake {

Engine::Engine(entt::registry &registry, const LocaleConfig::Config &config) :
	::Engine(registry, config, new Functions(registry, *this)) {
}

void Engine::init() {
	entt::entity musicPlayer = _registry.create();
	_registry.emplace<Sound::MusicPtr>(musicPlayer, std::make_shared<Sound::Music>());

	loadEpisode("gameworld:level_01");
}

const char *Engine::getName() const {
	return "Alan Wake";
}

}
