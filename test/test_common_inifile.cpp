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

#include "src/common/inifile.h"
#include "src/common/memreadstream.h"

#include <gtest/gtest.h>

static const auto kINISample =
	"# Top Level comment\n"
	"\n"
	"[test1]\n"
	"test_param1=1\n"
	"test_param2=2   \n" // Test the removal of trailing whitespaces
	"  \n"
	"[test2]  # Another Section\n"
	"test_param1=-1\n"
	"test_param2=Test?$%# Another nonsense comment\n"
	"test_param3=2.75  # Test parameter comment\n" // Test another removal of trailing whitspaces
	"test_param4=2.75 1.0 3.4   #This is a vec3 parameter\n"
	"test_param5=2 1.0 3\n"
	"test_param6=cool2cool\n"
;

TEST(INIFile, parseInt) {
	Common::MemoryReadStream iniStream(kINISample, std::strlen(kINISample));
	Common::INIFile ini(iniStream);

	EXPECT_EQ(ini.get<int>("test1", "test_param1"), 1);
	EXPECT_EQ(ini.get<int>("test1", "test_param2"), 2);
	EXPECT_EQ(ini.get<int>("test2", "test_param1"), -1);
	EXPECT_ANY_THROW(ini.get<int>("test2", "test_param2"));
	EXPECT_EQ(ini.get<int>("test2", "test_param3"), 2);
	EXPECT_ANY_THROW(ini.get<int>("test2", "test_param6"));
}

TEST(INIFile, parseString) {
	Common::MemoryReadStream iniStream(kINISample, std::strlen(kINISample));
	Common::INIFile ini(iniStream);

	EXPECT_EQ(ini.get<std::string>("test1", "test_param1"), "1");
	EXPECT_EQ(ini.get<std::string>("test1", "test_param2"), "2");
	EXPECT_EQ(ini.get<std::string>("test2", "test_param1"), "-1");
	EXPECT_EQ(ini.get<std::string>("test2", "test_param2"), "Test?$%");
	EXPECT_EQ(ini.get<std::string>("test2", "test_param3"), "2.75");
	EXPECT_EQ(ini.get<std::string>("test2", "test_param4"), "2.75 1.0 3.4");
	EXPECT_EQ(ini.get<std::string>("test2", "test_param5"), "2 1.0 3");

	EXPECT_ANY_THROW(ini.get<std::string>("test2", "test_param7"));
}

TEST(INIFile, parseFloat) {
	Common::MemoryReadStream iniStream(kINISample, std::strlen(kINISample));
	Common::INIFile ini(iniStream);

	EXPECT_EQ(ini.get<float>("test1", "test_param1"), 1.0);
	EXPECT_EQ(ini.get<float>("test1", "test_param2"), 2.0);
	EXPECT_EQ(ini.get<float>("test2", "test_param1"), -1.0);
	EXPECT_ANY_THROW(ini.get<float>("test2", "test_param2"));
	EXPECT_EQ(ini.get<float>("test2", "test_param3"), 2.75);
	EXPECT_ANY_THROW(ini.get<float>("test2", "test_param6"));
}

TEST(INIFile, parseVec3) {
	Common::MemoryReadStream iniStream(kINISample, std::strlen(kINISample));
	Common::INIFile ini(iniStream);

	EXPECT_ANY_THROW(ini.get<glm::vec3>("test1", "test_param1"));
	EXPECT_ANY_THROW(ini.get<glm::vec3>("test1", "test_param2"));
	EXPECT_ANY_THROW(ini.get<glm::vec3>("test2", "test_param1"));
	EXPECT_ANY_THROW(ini.get<glm::vec3>("test2", "test_param2"));
	EXPECT_EQ(ini.get<glm::vec3>("test2", "test_param4"), glm::vec3(2.75, 1.0, 3.4));
	EXPECT_EQ(ini.get<glm::vec3>("test2", "test_param5"), glm::vec3(2.0, 1.0, 3.0));
	EXPECT_ANY_THROW(ini.get<glm::vec3>("test2", "test_param6"));
}
