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

entt::entity getEntityByGID(entt::registry &registry, GID gid) {
	static const auto gidView = registry.view<GID>();
	entt::entity finalEntity = entt::null;
	for (const auto &entity : gidView) {
		GID g = registry.get<GID>(entity);
		if (g == gid) {
			finalEntity = entity;
			break;
		}
	}

	return finalEntity;
}

#endif //OPENAWE_ENTT_UTILS_H
