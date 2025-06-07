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

#include "src/awe/binarchive.h"
#include "src/awe/terraindatafile.h"
#include "src/awe/resman.h"

#include "src/level.h"

Level::Level(entt::registry &registry, entt::scheduler<double> &scheduler, const std::string &id, const std::string &world) :
	ObjectCollection(registry, scheduler),
	_terrain(std::make_unique<Graphics::Terrain>()),
	_id(id),
	_world(world) {
	spdlog::info("Loading level {}", id);
	std::string levelFolder = std::format("worlds/{}/levels/{}", world, id);

	_terrain->setLabel(std::format("terrain_{}", _id));

	loadGIDRegistry(ResMan.getResource(std::format("{}/GIDRegistry.txt", levelFolder)));

	std::unique_ptr<Common::ReadStream> globalStream(ResMan.getResource(std::format("{}/Global.bin", levelFolder)));
	AWE::BINArchive global(*globalStream);

	std::unique_ptr<Common::ReadStream> persistentStream(ResMan.getResource(std::format("{}/Persistent.bin", levelFolder)));
	AWE::BINArchive persistent(*persistentStream);

	loadBytecode(
			persistent.getResource("dp_bytecode.bin"),
			persistent.getResource("dp_bytecodeparameters.bin")
	);

	auto dp = std::make_shared<DPFile>(persistent.getResource("dp_persistent.bin"));

	spdlog::info("Loading attachment containers for {}", id);
	load(persistent.getResource("cid_attachmentcontainer.bin"), kAttachmentContainer, dp);

	// ,--- Load Possible attachment container resources
	spdlog::info("Loading script instances for {}", id);
	load(persistent.getResource("cid_scriptinstance.bin"), kScriptInstance, dp);
	load(persistent.getResource("cid_scriptinstancescript.bin"), kScript, dp);

	spdlog::info("Loading Point Lights for {}", id);
	load(persistent.getResource("cid_pointlight.bin"), kPointLight, dp);

	spdlog::info("Loading ambient lights for {}", id);
	load(persistent.getResource("cid_ambientlight.bin"), kAmbientLight, dp);
	load(persistent.getResource("cid_ambientlightscript.bin"), kScript, dp);

	spdlog::info("Loading Triggers for {}", id);
	load(persistent.getResource("cid_trigger.bin"), kTrigger, dp);
	load(persistent.getResource("cid_triggerscript.bin"), kScript, dp);
	// '---

	spdlog::info("Loading static objects for {}", id);
	load(global.getResource("cid_staticobject.bin"), kStaticObject);

	spdlog::info("Loading dynamic objects for {}", id);
	load(persistent.getResource("cid_dynamicobject.bin"), kDynamicObject, dp);
	load(persistent.getResource("cid_dynamicobjectscript.bin"), kDynamicObjectScript, dp);

	spdlog::info("Loading characters for {}", id);
	load(persistent.getResource("cid_character.bin"), kCharacter, dp);
	load(persistent.getResource("cid_characterscript.bin"), kCharacterScript, dp);

	spdlog::info("Loading floating scripts for {}", id);
	load(persistent.getResource("cid_floatingscript.bin"), kFloatingScript, dp);

	spdlog::info("Loading key frames for {}", id);
	load(persistent.getResource("cid_keyframe.bin"), kKeyframe, dp);

	spdlog::info("Loading key frame animations for {}", id);
	load(persistent.getResource("cid_keyframeanimation.bin"), kKeyframeAnimation, dp);

	spdlog::info("Loading key framers for {}", id);
	load(persistent.getResource("cid_keyframer.bin"), kKeyframer, dp);
	load(persistent.getResource("cid_keyframerscript.bin"), kScript, dp);

	spdlog::info("Loading key framed objects for {}", id);
	load(persistent.getResource("cid_keyframedobject.bin"), kKeyframedObject, dp);
	load(persistent.getResource("cid_keyframedobjectscript.bin"), kDynamicObjectScript, dp);

	const auto cellInfo = loadCellInfo(global.getResource("cid_cellinfo.bin"));
	for (const auto &info : cellInfo) {
		const std::string ldName = std::format("LD{:0>3}_{:0>3}", info.x, info.y);
		const std::string hdName = std::format("HD{:0>3}_{:0>3}", info.x, info.y);

		AWE::BINArchive ldCell(std::format("{}/{}.bin", levelFolder, ldName));
		AWE::BINArchive hdCell(std::format("{}/{}.bin", levelFolder, hdName));
		AWE::BINArchive ldCellResources(std::format("{}/{}.resources", levelFolder, ldName));
		AWE::BINArchive hdCellResources(std::format("{}/{}.resources", levelFolder, hdName));

		//DPFile dphd(persistent.getResource("dp_hdcell.bin"));
		load(hdCell.getResource("cid_staticobject.bin"), kStaticObject);
		load(ldCell.getResource("cid_staticobject.bin"), kStaticObject);
		loadTerrainData(
			ldCell.getResource("cid_terraindata.bin"),
			hdCell.getResource("cid_terraindata.bin")
		);

		std::unique_ptr<Common::ReadStream> terrainCollisions(ResMan.getResource(std::format("{}/{}.collisions", levelFolder, hdName)));
		assert(terrainCollisions);
		loadTerrainCollisions(terrainCollisions.get());

		loadFoliageData(hdCell.getResource("cid_foliagedata.bin"));
		loadFoliageData(ldCell.getResource("cid_foliagedata.bin"));
	}

	_terrain->finalize();
	_terrain->show();
}

void Level::loadTerrainData(Common::ReadStream *terrainDataLD, Common::ReadStream *terrainDataHD) {
	std::unique_ptr<Common::ReadStream> terrainDataStreamLD(terrainDataLD);
	std::unique_ptr<Common::ReadStream> terrainDataStreamHD(terrainDataHD);

	assert(terrainDataLD);
	assert(terrainDataHD);

	_terrain->loadTerrainData(*terrainDataStreamLD, *terrainDataStreamHD);
}

std::vector<glm::u32vec2> Level::loadCellInfo(Common::ReadStream *cid) const {
	std::unique_ptr<Common::ReadStream> cellInfoStream(cid);
	std::vector<glm::u32vec2> cell;
	AWE::CIDFile cidFile(*cellInfoStream, kCellInfo);
	for (const auto &container : cidFile.getContainers()) {
		const auto &cellInfo = std::any_cast<AWE::Templates::CellInfo>(container);
		cell.emplace_back(glm::u32vec2(cellInfo.x, cellInfo.y));
	}
	return cell;
}

void Level::setVisible(bool visible) {
	_terrain->setVisible(visible);

	ObjectCollection::setVisible(visible);
}
