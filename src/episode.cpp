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

#include "src/awe/resman.h"
#include "src/awe/binarchive.h"

#include "src/episode.h"

Episode::Episode(entt::registry &registry, const std::string &world, const std::string &id) :
	ObjectCollection(registry),
	_id(id),
	_world(world) {
	std::string episodeFolder = fmt::format("worlds/{}/episodes/{}", world, id);

	loadGIDRegistry(ResMan.getResource(fmt::format("{}/GIDRegistry.txt", episodeFolder)));

	std::unique_ptr<Common::ReadStream> episodeStream(ResMan.getResource(fmt::format("{}/episode.bin", episodeFolder)));
	AWE::BINArchive episode(*episodeStream);
	DPFile dp(episode.getResource("dp_episode.bin"));

	spdlog::info("Loading task definitions for {}", id);
	load(episode.getResource("cid_taskdefinition.bin"), dp);

	std::unique_ptr<Common::ReadStream> tasksStream(ResMan.getResource(fmt::format("{}/tasks.bin", episodeFolder)));
	AWE::BINArchive tasks(*tasksStream);

	loadBytecode(
			tasks.getResource("dp_bytecode.bin"),
			tasks.getResource("dp_bytecodeparameters.bin")
	);

	dp = DPFile(tasks.getResource("dp_task.bin"));

	spdlog::info("Loading static objects for {}", id);
	load(tasks.getResource("cid_staticobject.bin"), dp);

	spdlog::info("Loading dynamic objects for {}", id);
	load(tasks.getResource("cid_dynamicobject.bin"), dp);
	load(tasks.getResource("cid_dynamicobjectscript.bin"), dp);

	spdlog::info("Loading characters for {}", id);
	load(tasks.getResource("cid_character.bin"), dp);
	load(tasks.getResource("cid_characterscript.bin"), dp);

	spdlog::info("Loading script instances for {}", id);
	load(tasks.getResource("cid_scriptinstance.bin"), dp);
	load(tasks.getResource("cid_scriptinstancescript.bin"), dp);

	spdlog::info("Loading Point Lights for {}", id);
	load(tasks.getResource("cid_pointlight.bin"), dp);

	spdlog::info("Loading Floating Scripts for {}", id);
	load(tasks.getResource("cid_floatingscript.bin"), dp);

	spdlog::info("Loading Triggers for {}", id);
	load(tasks.getResource("cid_trigger.bin"), dp);
	//load(tasks.getResource("cid_triggerscript.bin"), dp);

	spdlog::info("Loading area triggers for {}", id);
	load(tasks.getResource("cid_areatrigger.bin"), dp);
	load(tasks.getResource("cid_areatriggerscript.bin"), dp);

	load(tasks.getResource("cid_taskcontent.bin"), dp);

	spdlog::info("Loading task scripts for {}", id);
	load(tasks.getResource("cid_taskscript.bin"), dp);

	spdlog::info("Loading waypoints for {}", id);
	load(tasks.getResource("cid_waypoint.bin"), dp);
	load(tasks.getResource("cid_waypointscript.bin"), dp);
}

void Episode::loadLevel(const std::string &id) {
	_levels.emplace_back(std::make_unique<Level>(_registry, id, _world));
}
