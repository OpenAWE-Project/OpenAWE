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

#ifndef AWE_GAME_H
#define AWE_GAME_H

#include <memory>

#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

#include "src/awe/script/functions.h"

#include "src/platform/window.h"
#include "src/platform/platform.h"
#include "src/graphics/gfxman.h"

#include "src/video/player.h"

#include "engine.h"
#include "src/global.h"
#include "src/world.h"

class Game {
public:
	bool parseArguments(int argc, char** argv);

	void init();
	void start();

private:
	bool _physicsDebugDraw;
	std::string _path;

	entt::registry _registry;

	std::unique_ptr<Video::Player> _player;

	Platform::Platform _platform;
	std::unique_ptr<Platform::Window> _window;

	std::unique_ptr<Global> _global;
	std::unique_ptr<Engine> _engine;
	std::unique_ptr<AWE::Script::Functions> _functions;
};


#endif //AWE_GAME_H
