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

#include <regex>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/awe/resman.h"
#include "src/awe/binarchive.h"

#include "src/episode.h"

Episode::Episode(entt::registry &registry, entt::scheduler<double> &scheduler, const std::string &world, const std::string &id) :
	ObjectCollection(registry, scheduler),
	_id(id),
	_world(world) {
	std::string episodeFolder = fmt::format("worlds/{}/episodes/{}", world, id);

	loadGIDRegistry(ResMan.getResource(fmt::format("{}/GIDRegistry.txt", episodeFolder)));

	std::unique_ptr<Common::ReadStream> episodeStream(ResMan.getResource(fmt::format("{}/episode.bin", episodeFolder)));
	AWE::BINArchive episode(*episodeStream);
	std::shared_ptr<DPFile> dp = std::make_shared<DPFile>(episode.getResource("dp_episode.bin"));

	spdlog::info("Loading task definitions for {}", id);
	load(episode.getResource("cid_taskdefinition.bin"), kTaskDefinition, dp);

	const auto archives = ResMan.getDirectoryResources(episodeFolder);

	for (const auto &archive: archives) {
		if (
			!std::regex_match(archive, std::regex(".*\\.bin$")) ||
			std::regex_match(archive, std::regex(".*episode\\.bin$"))
		)
			continue;

		spdlog::info("Loading {}", archive);

		std::unique_ptr<Common::ReadStream> tasksStream(ResMan.getResource(archive));
		AWE::BINArchive tasks(*tasksStream);

		if (tasks.hasResource("dp_bytecode.bin") && tasks.hasResource("dp_bytecodeparameters.bin"))
			loadBytecode(
				tasks.getResource("dp_bytecode.bin"),
				tasks.getResource("dp_bytecodeparameters.bin")
			);

		// Search for a dp file
		for (const auto &item: tasks.getDirectoryResources("")) {
			const auto filename = tasks.getResourcePath(item);
			if (std::regex_match(filename, std::regex("dp_[a-z]*\\.bin")) && !std::regex_match(filename, std::regex(".*bytecode(parameters)?.*")))
				dp = std::make_shared<DPFile>(tasks.getResource(filename));
		}

		spdlog::info("Loading attachment containers for {}", id);
		load(tasks.getResource("cid_attachmentcontainer.bin"), kAttachmentContainer, dp);

		// ,--- Load Possible attachment container entities
		spdlog::info("Loading script instances for {}", id);
		load(tasks.getResource("cid_scriptinstance.bin"), kScriptInstance, dp);
		load(tasks.getResource("cid_scriptinstancescript.bin"), kScript, dp);

		spdlog::info("Loading Point Lights for {}", id);
		load(tasks.getResource("cid_pointlight.bin"), kPointLight, dp);

		spdlog::info("Loading ambient lights for {}", id);
		load(tasks.getResource("cid_ambientlight.bin"), kAmbientLight, dp);
		load(tasks.getResource("cid_ambientlightscript.bin"), kScript, dp);

		spdlog::info("Loading Triggers for {}", id);
		load(tasks.getResource("cid_trigger.bin"), kTrigger, dp);
		load(tasks.getResource("cid_triggerscript.bin"), kScript, dp);
		// '---

		spdlog::info("Loading static objects for {}", id);
		load(tasks.getResource("cid_staticobject.bin"), kStaticObject, dp);

		spdlog::info("Loading dynamic objects for {}", id);
		load(tasks.getResource("cid_dynamicobject.bin"), kDynamicObject, dp);
		load(tasks.getResource("cid_dynamicobjectscript.bin"), kDynamicObjectScript, dp);

		spdlog::info("Loading characters for {}", id);
		load(tasks.getResource("cid_character.bin"), kCharacter, dp);
		load(tasks.getResource("cid_characterscript.bin"), kCharacterScript, dp);

		spdlog::info("Loading Spot Lights for {}", id);
		load(tasks.getResource("cid_spotlight.bin"), kSpotLight, dp);

		spdlog::info("Loading Floating Scripts for {}", id);
		load(tasks.getResource("cid_floatingscript.bin"), kFloatingScript, dp);

		spdlog::info("Loading area triggers for {}", id);
		load(tasks.getResource("cid_areatrigger.bin"), kAreaTrigger, dp);
		load(tasks.getResource("cid_areatriggerscript.bin"), kScript, dp);

		load(tasks.getResource("cid_taskcontent.bin"), kTaskContent, dp);

		spdlog::info("Loading task scripts for {}", id);
		load(tasks.getResource("cid_taskscript.bin"), kScript, dp);

		spdlog::info("Loading waypoints for {}", id);
		load(tasks.getResource("cid_waypoint.bin"), kWaypoint, dp);
		load(tasks.getResource("cid_waypointscript.bin"), kScript, dp);

		spdlog::info("Loading key frames for {}", id);
		load(tasks.getResource("cid_keyframe.bin"), kKeyframe, dp);

		spdlog::info("Loading key frame animations for {}", id);
		load(tasks.getResource("cid_keyframeanimation.bin"), kKeyframeAnimation, dp);

		spdlog::info("Loading key framers for {}", id);
		load(tasks.getResource("cid_keyframer.bin"), kKeyframer, dp);
		load(tasks.getResource("cid_keyframerscript.bin"), kScript, dp);

		spdlog::info("Loading key framed objects for {}", id);
		load(tasks.getResource("cid_keyframedobject.bin"), kKeyframedObject, dp);
		load(tasks.getResource("cid_keyframedobjectscript.bin"), kDynamicObjectScript, dp);
	}
}

void Episode::loadLevel(const std::string &id) {
	_levels.emplace_back(std::make_unique<Level>(_registry, _scheduler, id, _world));
}

void Episode::setVisible(bool visible) {
	for (auto &level: _levels) {
		level->setVisible(visible);
	}

	ObjectCollection::setVisible(visible);
}
