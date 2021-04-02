//
// Created by patrick on 19.06.20.
//

#include <string>

#include <gtest/gtest.h>

#include "src/common/strutil.h"

TEST(StringUtil, toLower) {
	const std::string testString = "CooL TeSt!#$";
	const std::string testResult = Common::toLower(testString);
	EXPECT_STREQ(testResult.c_str(), "cool test!#$");
}

TEST(StringUtil, toUpper) {
	const std::string testString = "CooL TeSt!#$";
	const std::string testResult = Common::toUpper(testString);
	EXPECT_STREQ(testResult.c_str(), "COOL TEST!#$");
}

TEST(StringUtil, contains) {
	const std::string testString = "This is a cool and awesome sentence.";

	EXPECT_TRUE(Common::contains(testString, "sentence"));
	EXPECT_TRUE(Common::contains(testString, "some"));
	EXPECT_TRUE(Common::contains(testString, "cool"));
	EXPECT_TRUE(Common::contains(testString, "."));

	EXPECT_FALSE(Common::contains(testString, "incredible"));
}

TEST(StringUtil, split) {
	const std::string testString = "multiple words  splitted ";
	const std::vector<std::string> split = Common::split(testString, std::regex("\\s+"));

	ASSERT_EQ(split.size(), 3);

	EXPECT_STREQ(split[0].c_str(), "multiple");
	EXPECT_STREQ(split[1].c_str(), "words");
	EXPECT_STREQ(split[2].c_str(), "splitted");
}

TEST(StringUtil, crc32) {
	const std::string testString1 = "This is a cool and awesome sentence.";
	const std::string testString2 = "This is another cool and awesome sentence.";

	EXPECT_EQ(Common::crc32(testString1), 2513066006);
	EXPECT_EQ(Common::crc32(testString2), 740559520);
}
