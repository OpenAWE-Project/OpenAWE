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

#include <assert.h>
#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include <src/graphics/gfxman.h>
#include <src/awe/terraindatafile.h>

#include "world.h"
#include "src/awe/resman.h"
#include "src/awe/binarchive.h"
#include "src/awe/cidfile.h"
#include "src/awe/gidregistryfile.h"
#include "src/awe/havokfile.h"
#include "src/awe/script/collection.h"

World::World(entt::registry &registry, const std::string &name) :
	ObjectCollection(registry),
	_name(name)
{
	std::string filename = fmt::format("globaldb/{}.xml", _name);

	std::unique_ptr<Common::ReadStream> worldStream(ResMan.getResource(filename));
	if (!worldStream)
		throw std::runtime_error("Invalid world name");

	_world = std::make_unique<WorldFile>(*worldStream);

	assert(_world->getWorldName() == _name);
}

const std::string &World::getName() const {
	return _name;
}

void World::loadGlobal() {
	spdlog::info("Loading global data from {}", _name);
	std::string globalFolder = fmt::format("worlds/{}/episodes/global", _name);

	if (!ResMan.hasDirectory(globalFolder)) {
		spdlog::debug("Cannot find global data for {}", _name);
		return;
	}

	loadGIDRegistry(ResMan.getResource(fmt::format("{}/GIDRegistry.txt", globalFolder)));

	std::string globalArchive;
	if (ResMan.hasResource(globalFolder + "/tasks.bin"))
		globalArchive = globalFolder + "/tasks.bin";
	else if (ResMan.hasResource(globalFolder + "/root.bin"))
		globalArchive = globalFolder + "/root.bin";
	else
		throw std::runtime_error("global world archive not found");

	AWE::BINArchive tasks(globalArchive);

	auto dp = std::make_shared<DPFile>(tasks.getResource("dp_task.bin"));

	loadBytecode(
			tasks.getResource("dp_bytecode.bin"),
			tasks.getResource("dp_bytecodeparameters.bin")
	);

	spdlog::info("Loading static objects for {}", _name);
	load(tasks.getResource("cid_staticobject.bin"), kStaticObject, dp);

	spdlog::info("Loading dynamic objects for {}", _name);
	load(tasks.getResource("cid_dynamicobject.bin"), kDynamicObject, dp);
	load(tasks.getResource("cid_dynamicobjectscript.bin"), kDynamicObjectScript, dp);

	spdlog::info("Loading characters for {}", _name);
	load(tasks.getResource("cid_character.bin"), kCharacter, dp);
	load(tasks.getResource("cid_characterscript.bin"), kCharacterScript, dp);
}

void World::loadEpisode(const std::string &id) {
	spdlog::info("Loading episode {} from {}", id, _name);
	WorldFile::Level level = _world->getLevel(id);

	_currentEpisode = std::make_unique<Episode>(_registry, _name, id);
	for (const auto &fileName : level.fileNames) {
		_currentEpisode->loadLevel(fileName);
	}
}
