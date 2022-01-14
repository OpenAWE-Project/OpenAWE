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
#include <variant>

namespace AWE::Script {

/*! Bytecode opcodes */
enum Opcode {
	kPush         = 0x01,
	kPushGID      = 0x02,
	kCallGlobal   = 0x03,
	kCallObject   = 0x04,
	kMulFloat     = 0x05, // ?
	kMulInt       = 0x09,
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

typedef std::variant<
        int32_t,
        std::string,
        entt::entity
> Variable;

}

#endif //OPENAWE_TYPES_H
