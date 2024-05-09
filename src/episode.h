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

#ifndef OPENAWE_EPISODE_H
#define OPENAWE_EPISODE_H

#include "src/objectcollection.h"
#include "src/level.h"

class Episode : public ObjectCollection {
public:
	Episode(entt::registry &registry, entt::scheduler<double> &scheduler, const std::string &id, const std::string &world);

	void loadLevel(const std::string &id);

	void setVisible(bool visible) override;

private:
	const std::string _id, _world;

	std::vector<std::unique_ptr<Level>> _levels;
};


#endif //OPENAWE_EPISODE_H
