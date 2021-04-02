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

#ifndef SRC_COMMON_ENDIANNESS_H
#define SRC_COMMON_ENDIANNESS_H

#include <cstdint>

namespace Common {

inline uint64_t swapBytes(uint64_t a) {
	return __builtin_bswap64(a);
}

inline uint32_t swapBytes(uint32_t a) {
	return __builtin_bswap32(a);
}

inline uint16_t swapBytes(uint16_t a) {
	return __builtin_bswap16(a);
}

} // End of namespace Common

#endif //SRC_COMMON_ENDIANNESS_H
