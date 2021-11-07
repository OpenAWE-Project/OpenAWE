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

#include <memory>

#include <spdlog/spdlog.h>

#include "src/common/strutil.h"

#include "src/awe/gidregistryfile.h"
#include "src/awe/dpfile.h"
#include "src/awe/resman.h"
#include "src/awe/cidfile.h"

#include "src/global.h"

Global::Global(entt::registry &registry) : ObjectCollection(registry) {
	spdlog::info("Loading global data");

	loadGIDRegistry(ResMan.getResource("global/GIDRegistry.txt"));
	loadBytecode(
			ResMan.getResource("global/dp_bytecode.bin"),
			ResMan.getResource("global/dp_bytecodeparameters.bin")
	);

	auto dp = std::make_shared<DPFile>(ResMan.getResource("global/dp_global.bin"));

	spdlog::info("Loading skeletons");
	load(ResMan.getResource("global/cid_skeleton.bin"), kSkeleton, dp);

	spdlog::info("Loading animations");
	load(ResMan.getResource("global/cid_animation.bin"), kAnimation, dp);

	spdlog::info("Loading notebook pages");
	load(ResMan.getResource("global/cid_notebook_page.bin"), kNotebookPage, dp);

	spdlog::info("Loading sounds");
	load(ResMan.getResource("global/cid_sound.bin"), kSound, dp);

	spdlog::info("Loading character classes");
	load(ResMan.getResource("global/cid_characterclass.bin"), kCharacterClass, dp);
}
