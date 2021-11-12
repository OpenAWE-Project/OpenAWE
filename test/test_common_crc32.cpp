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
#include <cstring>

/*
 * All tested values need to be compatible with zlibs crc32 implementation
 */
#include <zlib.h>

#include <gtest/gtest.h>

#include "src/common/crc32.h"
#include "src/common/types.h"

#include "test/lipsum.h"

TEST(CRC32, simpleValues) {
	const char *testString1 = "Hello World!";
	const char *testString2 = "Hello World :D";
	const char *testString3 = "Nice World!!";

	const auto testHash1 = ::crc32(0, reinterpret_cast<const byte*>(testString1), std::strlen(testString1));
	const auto testHash2 = ::crc32(0, reinterpret_cast<const byte*>(testString2), std::strlen(testString2));
	const auto testHash3 = ::crc32(0, reinterpret_cast<const byte*>(testString3), std::strlen(testString3));

	EXPECT_EQ(Common::crc32(testString1), testHash1);
	EXPECT_EQ(Common::crc32(testString2), testHash2);
	EXPECT_EQ(Common::crc32(testString3), testHash3);

	EXPECT_NE(Common::crc32(testString1), Common::crc32(testString2));
	EXPECT_NE(Common::crc32(testString2), Common::crc32(testString3));
	EXPECT_NE(Common::crc32(testString3), Common::crc32(testString1));
}

TEST(CRC32, complexValues) {
	const auto testHashLipsum = ::crc32(0, reinterpret_cast<const byte *>(kLipsum), std::strlen(kLipsum));

	EXPECT_EQ(Common::crc32(kLipsum), testHashLipsum);
}
