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

#include "src/awe/binarchive.h"
#include "src/awe/terraindatafile.h"
#include "src/awe/resman.h"

#include "src/level.h"

Level::Level(entt::registry &registry, const std::string &id, const std::string &world) : ObjectCollection(registry), _id(id), _world(world) {
	std::string levelFolder = fmt::format("worlds/{}/levels/{}", world, id);

	loadGIDRegistry(ResMan.getResource(fmt::format("{}/GIDRegistry.txt", levelFolder)));

	std::unique_ptr<Common::ReadStream> globalStream(ResMan.getResource(fmt::format("{}/Global.bin", levelFolder)));
	AWE::BINArchive global(*globalStream);

	load(global.getResource("cid_staticobject.bin"), kStaticObject);

	std::unique_ptr<Common::ReadStream> persistentStream(ResMan.getResource(fmt::format("{}/Persistent.bin", levelFolder)));
	AWE::BINArchive persistent(*persistentStream);

	loadBytecode(
			persistent.getResource("dp_bytecode.bin"),
			persistent.getResource("dp_bytecodeparameters.bin")
	);

	auto dp = std::make_shared<DPFile>(persistent.getResource("dp_persistent.bin"));

	load(persistent.getResource("cid_dynamicobject.bin"), kDynamicObject, dp);
	load(persistent.getResource("cid_dynamicobjectscript.bin"), kDynamicObjectScript, dp);
	load(persistent.getResource("cid_character.bin"), kCharacter, dp);
	load(persistent.getResource("cid_characterscript.bin"), kCharacterScript, dp);
	load(persistent.getResource("cid_floatingscript.bin"), kFloatingScript, dp);

	const auto cellInfo = loadCellInfo(global.getResource("cid_cellinfo.bin"));
	for (const auto &info : cellInfo) {
		const std::string ldName = fmt::format("LD{:0>3}_{:0>3}", info.x, info.y);
		const std::string hdName = fmt::format("HD{:0>3}_{:0>3}", info.x, info.y);

		AWE::BINArchive ldCell(fmt::format("{}/{}.bin", levelFolder, ldName));
		AWE::BINArchive hdCell(fmt::format("{}/{}.bin", levelFolder, hdName));
		AWE::BINArchive ldCellResources(fmt::format("{}/{}.resources", levelFolder, ldName));
		AWE::BINArchive hdCellResources(fmt::format("{}/{}.resources", levelFolder, hdName));

		//DPFile dphd(persistent.getResource("dp_hdcell.bin"));
		load(hdCell.getResource("cid_staticobject.bin")/*, dphd*/, kStaticObject);

		load(ldCell.getResource("cid_staticobject.bin"), kStaticObject);
		//loadTerrainData(ldCell.getResource("cid_terraindata.bin"));
	}
}

void Level::loadTerrainData(Common::ReadStream *terrainData) {
	std::unique_ptr<Common::ReadStream> terrainDataStream(terrainData);
	_terrains.emplace_back(std::make_unique<Graphics::Terrain>(terrainDataStream.get()));
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
