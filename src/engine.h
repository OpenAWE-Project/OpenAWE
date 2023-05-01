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

#ifndef AWE_ENGINE_H
#define AWE_ENGINE_H

#include <entt/entt.hpp>

#include "src/awe/script/functions.h"

#include "src/graphics/fullscreenplane.h"

#include "src/video/player.h"

#include "src/configuration.h"
#include "src/localeconfig.h"
#include "src/world.h"
#include "src/playercontroller.h"

class Engine {
public:
	Engine(entt::registry &registry, const LocaleConfig::Config &, AWE::Script::Functions *);
	virtual ~Engine();

	virtual void init() = 0;
	virtual const char *getName() const = 0;
	AWE::Script::Functions &getFunctions();
	AWE::Script::Context &getScriptContext();
	entt::scheduler<double> & getScheduler();

	void loadVideo(const std::string &videoFile);
	void playVideo();
	void stopVideo();

	void update(float time);

	void writeConfiguration();
	Configuration &getConfiguration();

	virtual void loadEpisode(const std::string &data);

protected:
	entt::registry &_registry;
	entt::scheduler<double> _scheduler;
	std::unique_ptr<AWE::Script::Functions> _functions;
	std::unique_ptr<Configuration> _configuration;

private:
	std::unique_ptr<World> _world;
	std::unique_ptr<AWE::Script::Context> _context;
	std::unique_ptr<Video::Player> _player;
	std::unique_ptr<Graphics::FullScreenPlane> _videoPlane;
	PlayerController _playerController;
	LocaleConfig::Config _localeConfig;
};


#endif //AWE_ENGINE_H
