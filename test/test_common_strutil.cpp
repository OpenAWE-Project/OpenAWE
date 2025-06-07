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

#include "src/common/exception.h"
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

TEST(StringUtil, startsWith) {
	const std::string testString = "This is a cool and awesome sentence.";

	EXPECT_TRUE(Common::startsWith(testString, "This"));
	EXPECT_FALSE(Common::startsWith(testString, "sentence."));
	EXPECT_FALSE(Common::startsWith(testString, "awesome"));
}

TEST(StringUtil, endsWith) {
	const std::string testString = "This is a cool and awesome sentence.";

	EXPECT_FALSE(Common::endsWith(testString, "This"));
	EXPECT_TRUE(Common::endsWith(testString, "sentence."));
	EXPECT_FALSE(Common::endsWith(testString, "awesome"));
}

TEST(StringUtil, replace) {
	const std::string testString = std::string("This is a cool and awesome sentence.");
	EXPECT_THROW(Common::replace(testString, "cool", "super cool"), Common::Exception);
	std::string testResult = Common::replace(testString, "incredible", "outstanding");
	EXPECT_STREQ(testString.c_str(), testResult.c_str());

	testResult = Common::replace(testString, " ", "/");
	EXPECT_STREQ(testResult.c_str(), "This/is/a/cool/and/awesome/sentence.");
	testResult = Common::replace(testString, "This is a", "Yet another");
	EXPECT_STREQ(testResult.c_str(), "Yet another cool and awesome sentence.");
	testResult = Common::replace(testString, "sentence.", "phrase!");
	EXPECT_STREQ(testResult.c_str(), "This is a cool and awesome phrase!");
}

TEST(StringUtil, extract) {
	const std::string testString1 = "This is a cooool and awesome sentence.";
	const std::string testString2 = "This is a not a cool and awesome sentence.";

	const auto testResult1 = Common::extract(testString1, std::regex("co+l", std::regex::ECMAScript));
	const auto testResult2 = Common::extract(testString2, std::regex("good", std::regex::ECMAScript));
	EXPECT_STREQ(testResult1.c_str(), "cooool");
	EXPECT_TRUE(testResult2.empty());
}

TEST(StringUtil, parse) {
	const std::string testInt = "42";
	const std::string testFloat = "32.66";
	const std::string testInvalidInt = "int";
	const std::string testInvalidFloat = "f32.66";

	EXPECT_EQ(Common::parse<int>(testInt), 42);
	EXPECT_FLOAT_EQ(Common::parse<float>(testFloat), 32.66);

	EXPECT_ANY_THROW(Common::parse<int>(testInvalidInt));
	EXPECT_ANY_THROW(Common::parse<float>(testInvalidFloat));
}

TEST(StringUtil, join) {
	const std::vector<int> testValues = {1, 2, 3, 4,5, 6, 7, 8, 9, 0};

	const auto testResult = Common::join(testValues, ";");

	EXPECT_STREQ(testResult.c_str(), "1;2;3;4;5;6;7;8;9;0");
}
