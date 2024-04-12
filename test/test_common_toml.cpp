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

#include "src/common/memreadstream.h"
#include "src/common/toml.h"

static const auto kTOMLSample =
		"# Top Level comment\n"
		"\n"
		"[test1]\n"
		"test_param1=1\n"
		"test_param2=2   \n" // Test the removal of trailing whitespaces
		"  \n"
		"[test2]  # Another Section\n"
		"test_param1=-1\n"
		"test_param2= \"Test$%&\"# Another nonsense comment\n"
		"test_param3=2.75  # Test parameter comment\n" // Test another removal of trailing whitspaces
		"test_param4=[2.75, 1.0, 3.4]   #This is a vec3 parameter\n"
		"test_param5=[2, 1.0, 3]\n"
		"test_param6 = \"cool2cool\"\n"
		"test_param7 =false\n"
		"test_param8=true\n"
;

TEST(TOML, parse) {
	Common::MemoryReadStream tomlStream(kTOMLSample, strlen(kTOMLSample));

	Common::TOML toml;
	toml.read(tomlStream);

	EXPECT_EQ(toml.getInt("test1.test_param1"), 1);
	EXPECT_EQ(toml.getInt("test1.test_param2"), 2);
	EXPECT_EQ(toml.getFloat("test1.test_param1"), 1.0);
	EXPECT_EQ(toml.getFloat("test1.test_param2"), 2.0);

	EXPECT_EQ(toml.getInt("test2.test_param1"), -1);
	EXPECT_EQ(toml.getFloat("test2.test_param1"), -1.0);
	EXPECT_EQ(toml.getString("test2.test_param2"), std::string("Test$%&"));
	EXPECT_EQ(toml.getInt("test2.test_param3"), 2);
	EXPECT_EQ(toml.getFloat("test2.test_param3"), 2.75);

	const auto floatArray1 = toml.getFloatArray("test2.test_param4");
	ASSERT_EQ(floatArray1.size(), 3);
	EXPECT_EQ(floatArray1.at(0), 2.75);
	EXPECT_EQ(floatArray1.at(1), 1.0);
	EXPECT_EQ(floatArray1.at(2), 3.4);

	const auto floatArray2 = toml.getFloatArray("test2.test_param5");
	ASSERT_EQ(floatArray2.size(), 3);
	EXPECT_EQ(floatArray2.at(0), 2);
	EXPECT_EQ(floatArray2.at(1), 1.0);
	EXPECT_EQ(floatArray2.at(2), 3);

	const auto intArray1 = toml.getIntArray("test2.test_param4");
	ASSERT_EQ(intArray1.size(), 3);
	EXPECT_EQ(intArray1.at(0), 2);
	EXPECT_EQ(intArray1.at(1), 1);
	EXPECT_EQ(intArray1.at(2), 3);

	const auto intArray2 = toml.getIntArray("test2.test_param5");
	ASSERT_EQ(intArray2.size(), 3);
	EXPECT_EQ(intArray2.at(0), 2);
	EXPECT_EQ(intArray2.at(1), 1);
	EXPECT_EQ(intArray2.at(2), 3);

	const auto floatVec1 = toml.getVec<glm::vec3>("test2.test_param4");
	EXPECT_EQ(floatVec1, glm::vec3(2.75, 1.0, 3.4));
	const auto floatVec2 = toml.getVec<glm::vec3>("test2.test_param5");
	EXPECT_EQ(floatVec2, glm::vec3(2.0, 1.0, 3.0));

	EXPECT_EQ(toml.getString("test2.test_param6"), std::string("cool2cool"));
	EXPECT_FALSE(toml.getBool("test2.test_param7"));
	EXPECT_TRUE(toml.getBool("test2.test_param8"));
}
