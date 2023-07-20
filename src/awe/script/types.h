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

#ifndef OPENAWE_AWE_SCRIPT_TYPES_H
#define OPENAWE_AWE_SCRIPT_TYPES_H

#include <entt/entt.hpp>
#include <fmt/format.h>

#include <variant>

#include "src/common/bit_cast.h"

#include "src/awe/script/float.h"

namespace AWE::Script {

/*! Bytecode opcodes */
enum Opcode {
	kPush         = 0x01,
	kPushGID      = 0x02,
	kCallGlobal   = 0x03,
	kCallObject   = 0x04,
	kMulFloat     = 0x05, // ?
	kDivFloat     = 0x06, // ?
	kAddFloat     = 0x07, // ?
	kSubFloat     = 0x08, // ?
	kMulInt       = 0x09,
	kDivInt       = 0x0A, // ?
	kAddInt       = 0x0B, // ?
	kSubInt       = 0x0C, // ?
	kRet          = 0x0D,
	kIntToFloat   = 0x0E,
	kSetMember    = 0x0F,
	kGetMember    = 0x10,
	kCmp          = 0x13,
	kJmp          = 0x15,
	kJmpIf        = 0x1A,
	kLogAnd       = 0x1C,
	kLogOr        = 0x1D,
	kLogNot       = 0x1E,
	kLessThen     = 0x20, // ?
	kGreaterThen  = 0x21, // ?
	kLessEqual    = 0x22, // ?
	kGreaterEqual = 0x23, // ?
	kNeq          = 0x24,
	kEq           = 0x25,
};

/*!
 * A simple union type simplifying the handling of numbers which can be integer or float at the same time.
 */
union Number {
	int32_t integer;
	float floatingPoint;

	Number() : integer(0) {}
	Number(int32_t value) : integer(value) {}
	Number(float value) : floatingPoint(value) {}
};

typedef std::variant<
        Number,
        std::string,
        entt::entity
> Variable;

}

/*!
 * Class for formatting a bytecode variable for logging
 */
template<> struct fmt::formatter<AWE::Script::Variable> {
	constexpr auto parse(fmt::format_parse_context &ctx) {
		return ctx.end();
	}

	template<typename FormatContext> auto format(const AWE::Script::Variable &variable, FormatContext& ctx) const {
		switch (variable.index()) {
			case 0: {
				int32_t intValue = std::get<AWE::Script::Number>(variable).integer;
				if (AWE::Script::isFloat(intValue))
					return fmt::format_to(ctx.out(), "{:f}", Common::bit_cast<float>(intValue));
				return fmt::format_to(ctx.out(), "{}", intValue);
			}
			case 1:
				return fmt::format_to(ctx.out(), "\"{}\"", std::get<std::string>(variable));
			case 2: {
				entt::entity entity = std::get<entt::entity>(variable);
				if (entity == entt::null)
					return fmt::format_to(ctx.out(), "<null>");
				else
					return fmt::format_to(ctx.out(), "<{}>", static_cast<unsigned int>(entity));
			}
			default:
				return fmt::format_to(ctx.out(), "<invalid>");

		}
	}
};

#endif //OPENAWE_TYPES_H
