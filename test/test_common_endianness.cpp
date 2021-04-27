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

#include <string>

#include <gtest/gtest.h>

#include "src/common/endianness.h"

TEST(Endianness, swapBytes) {
	const uint16_t test16bit = 0x1234;
	const uint32_t test32bit = 0x12345678;
	const uint64_t test64bit = 0x1234567890ABCDEF;

	EXPECT_EQ(Common::swapBytes(test16bit), 0x3412);
	EXPECT_EQ(Common::swapBytes(test32bit), 0x78563412);
	EXPECT_EQ(Common::swapBytes(test64bit), 0xEFCDAB9078563412);
}
