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
#include <regex>

#include <fmt/format.h>
#include <CLI/CLI.hpp>

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

enum ObjectIDType {
	kAnimationID           = 34,
	kKeyframerID           = 123,
	kKeyframeAnimationID   = 125,
	kKeyframeID            = 126,
	kAttachmentContainerID = 164,
};

class ObjectID {
public:
	ObjectID() : _value(0) {};
	ObjectID(uint32_t value) : _value(value) {};
	ObjectID(ObjectIDType type, uint32_t id) : _value(type | id << 9) {};

	uint32_t getID() const { return _value >> 9; };
	ObjectIDType getType() const { return ObjectIDType(_value & 0x1FF); };

	bool operator==(const ObjectID &rhs) const {
		return _value == rhs._value;
	}

	bool operator!=(const ObjectID &rhs) const {
		return !(rhs == *this);
	}

	bool operator<(const ObjectID &rhs) const {
		return _value < rhs._value;
	}

	operator bool() const {
		return _value != 0;
	}

private:
	uint32_t _value;
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
	kKeyframer,
	kSound,
	kAttachmentResources,
	kCharacterClothParameters,
	kAnimationParameters,
	kKeyframedObject,
	kKeyframeAnimation,
	kKeyframe,
	kGameEvent,
	kAttachmentContainer,
	kSpotLight,
	kWeapon
};

struct GIDValidator : public CLI::Validator {
	GIDValidator() {
		name_ = "GID";
		func_ = [](const std::string &str) -> std::string {
			if (!std::regex_match(str, std::regex("\\d+:[0-9a-fA-F]{8}")))
				return fmt::format("{} is not a valid GID");

			return "";
		};
	}
};

/*!
 * Class for formatting a GIDs for logging
 */
template<> struct fmt::formatter<GID> {
	constexpr auto parse(fmt::format_parse_context &ctx) {
		return ctx.end();
	}

	template<typename FormatContext> auto format(const GID &gid, FormatContext& ctx) const {
		return fmt::format_to(ctx.out(), "{}:{:x}", gid);
	}
};

#endif //AWE_TYPES_H
