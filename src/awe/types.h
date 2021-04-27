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

#ifndef AWE_TYPES_H
#define AWE_TYPES_H

#include <cstdint>

#include <tuple>

enum GameEngine {
	kAlanWake,
	kAlanWakesAmericanNightmare,
	kQuantumBreak
};

struct GID {
	uint32_t type;
	uint32_t id;

	[[nodiscard]] bool isNil() const {
		return type == 0 && id == 0;
	}

	bool operator==(const GID &rhs) const {
		return std::tie(type, id) == std::tie(rhs.type, rhs.id);
	}

	bool operator!=(const GID &rhs) const {
		return !(rhs == *this);
	}

	bool operator<(const GID &rhs) const {
		return std::tie(type, id) < std::tie(rhs.type, rhs.id);
	}
};

typedef uint32_t rid_t;

enum ObjectType {
	kRID,
	kCellInfo,
	kSkeleton,
	kSkeletonSetup,
	kNotebookPage,
	kDynamicObjectScript,
	kStaticObject,
	kDynamicObject,
	kCharacter,
	kCharacterClass,
	kCharacterScript,
	kScriptVariables,
	kAnimation,
	kAmbientLight,
	kScriptInstance,
	kScript,
	kTrigger,
	kAreaTrigger,
	kPhysicsMaterial,
	kFileInfoMetadata,
	kResourceMetadata,
	kFoliageMeshMetadata,
	kHavokAnimationMetadata,
	kMeshMetadata,
	kParticleSystemMetadata,
	kTextureMetadata,
	kAABB,
	kPointLight,
	kFloatingScript,
	kWaypoint,
	kTaskDefinition,
	kTaskContent,
	kKeyFramer,
	kSound,
	kAttachmentResources,
	kCharacterClothParameters,
	kAnimationParameters
};

#endif //AWE_TYPES_H
