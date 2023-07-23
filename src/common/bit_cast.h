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

#ifndef OPENAWE_BIT_CAST_H
#define OPENAWE_BIT_CAST_H

#include <cstring>

#include <type_traits>

namespace Common {

/*!
 * Casts a value from one type to another by simply copying the memory contents. This only works if both types have the
 * same size and are both trivially copyable. This implementation is based on the c++20 std::bit_cast.
 *
 * @note Taken from https://en.cppreference.com/w/cpp/numeric/bit_cast
 *
 * @tparam To The type from which to cast
 * @tparam From The type to which to cast
 * @param src The source value to cast
 * @return The casted value to the From type
 */
template<class To, typename From>
std::enable_if_t<
	sizeof(To) == sizeof(From) &&
	std::is_trivially_copyable_v<From> &&
	std::is_trivially_copyable_v<To>,
	To
>
bit_cast(const From& src) {
	static_assert(std::is_trivially_constructible_v<To>,
				  "This implementation additionally requires "
				  "destination type to be trivially constructible");

	To dst;
	std::memcpy(&dst, &src, sizeof(To));
	return dst;
}

} // End of namespace Common

#endif //OPENAWE_BIT_CAST_H
