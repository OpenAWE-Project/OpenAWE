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

#include <gtest/gtest.h>

#include "src/common/uuid.h"

TEST(UUID, testNil) {
	Common::UUID uuid = Common::UUID::generateNil();

	EXPECT_TRUE(uuid.isNil());
}

TEST(UUID, testRandom) {
	Common::UUID uuid = Common::UUID::generateRandom();

	EXPECT_FALSE(uuid.isNil());
}

TEST(UUID, testNilComparison) {
	Common::UUID uuid1 = Common::UUID::generateNil();
	Common::UUID uuid2 = Common::UUID::generateNil();

	EXPECT_NE(uuid1, uuid2);
}

TEST(UUID, testEquality) {
	Common::UUID uuid1 = Common::UUID::generateRandom();
	Common::UUID uuid2 = uuid1;
	Common::UUID uuid3 = Common::UUID::generateRandom();

	EXPECT_EQ(uuid1, uuid2);
	EXPECT_NE(uuid1, uuid3);
	EXPECT_NE(uuid2, uuid3);
}

TEST(UUID, testString) {
	Common::UUID uuid1 = Common::UUID::createFromString("ee5f4df0f65e5ed158993fb7bed43885");
	Common::UUID uuid2 = Common::UUID::createFromString("ee5f4#df*0f65e-5ed158993fb-7be/d43885");
	Common::UUID uuid3 = Common::UUID::createFromString("4a-95bd7d1c719bba7a567f6395-df62f9");

	EXPECT_EQ(uuid1, uuid2);
	EXPECT_NE(uuid1, uuid3);
	EXPECT_NE(uuid2, uuid3);
	EXPECT_THROW(Common::UUID::createFromString("Not a uuid"), std::runtime_error);
}
