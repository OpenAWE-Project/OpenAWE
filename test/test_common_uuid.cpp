//
// Created by patrick on 02.09.20.
//

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
