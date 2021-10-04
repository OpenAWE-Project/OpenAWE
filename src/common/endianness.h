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

#define MANUAL_BSWAP_16(x) ( \
	((x & 0xFF00) >> 8) |       \
	((x & 0x00FF) << 8)         \
	)

#define MANUAL_BSWAP_32(x) ( \
	((x & 0x000000FF) << 24) |  \
	((x & 0x0000FF00) << 8) |   \
	((x & 0x00FF0000) >> 8) |   \
	((x & 0xFF000000) >> 24)    \
	)

#define MANUAL_BSWAP_64(x) ( \
	((x & 0x00000000000000FF) << 56) | \
	((x & 0x000000000000FF00) << 40) | \
	((x & 0x0000000000FF0000) << 24) | \
	((x & 0x00000000FF000000) << 8) |  \
	((x & 0x000000FF00000000) >> 8) |  \
	((x & 0x0000FF0000000000) >> 24) | \
	((x & 0x00FF000000000000) >> 40) | \
	((x & 0xFF00000000000000) >> 56)   \
	)

namespace Common {

inline uint64_t swapBytes(uint64_t a) {
#if HAS_BUILTIN_BSWAP64
	return __builtin_bswap64(a);
#elif HAS_BYTESWAP_UINT64
	return _byteswap_uint64(a);
#else
	return MANUAL_BSWAP_64(a);
#endif
}

inline uint32_t swapBytes(uint32_t a) {
#if HAS_BUILTIN_BSWAP32
	return __builtin_bswap32(a);
#elif HAS_BYTESWAP_ULONG
	return _byteswap_ulong(a);
#else
	return MANUAL_BSWAP_32(a);
#endif
}

inline uint16_t swapBytes(uint16_t a) {
#if HAS_BUILTIN_BSWAP16
	return __builtin_bswap16(a);
#elif HAS_BYTESWAP_USHORT
	return _byteswap_ushort(a);
#else
	return MANUAL_BSWAP_16(a);
#endif
}

} // End of namespace Common

#endif //SRC_COMMON_ENDIANNESS_H
