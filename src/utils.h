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

#ifndef OPENAWE_ENTT_UTILS_H
#define OPENAWE_ENTT_UTILS_H

#include <entt/entt.hpp>

#include "src/awe/types.h"

/*!
 * Simple relationship component for entt as sggested by: https://skypjack.github.io/2019-06-25-ecs-baf-part-4/
 */
struct Relationship {
	entt::entity parent{entt::null};
	std::vector<entt::entity> children;
};

/*!
 * Get the entity associated with a specific global id
 * \param registry The registry from which to search for the entity
 * \param gid The global id to search for
 * \return The entity with the given global id or entt::null if the entity was not found
 */
entt::entity getEntityByGID(entt::registry &registry, GID gid);

#endif //OPENAWE_ENTT_UTILS_H
