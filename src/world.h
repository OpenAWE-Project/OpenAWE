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

#ifndef AWE_WORLD_H
#define AWE_WORLD_H

#include <memory>

#include <glm/glm.hpp>
#include <src/awe/script/collection.h>

#include "src/graphics/model.h"
#include "src/awe/worldfile.h"
#include "src/objectcollection.h"
#include "src/global.h"
#include "src/level.h"
#include "src/episode.h"

class World : public ObjectCollection {
public:
	World(entt::registry &registry, entt::scheduler<double> &scheduler, const std::string &name);

	const std::string &getName() const;

	void loadGlobal();
	void loadEpisode(const std::string &id);

private:
	const std::string _name;
	std::unique_ptr<WorldFile> _world;
	std::unique_ptr<Episode> _currentEpisode;
};


#endif //AWE_WORLD_H
