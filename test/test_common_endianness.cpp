//
// Created by patrick on 22.07.20.
//

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
