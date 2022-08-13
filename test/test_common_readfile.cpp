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

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "src/common/readfile.h"
#include "src/common/uuid.h"

#include "lipsum.h"

class ReadFile : public testing::Test {
protected:
	void SetUp() override {
		_filename = std::tmpnam(nullptr);
		std::ofstream out(_filename, std::ios::out | std::ios::binary);

		// Make 256 byte forwards and backwards
		for (int i = 0; i < 256; ++i) {
			data1[i] = i;
			data2[i] = 255 - i;
		}

		out.write(reinterpret_cast<char*>(data1), 256);
		out.write(reinterpret_cast<char*>(data2), 256);
		out.close();
	}

	void TearDown() override {
		std::filesystem::remove(_filename);
	}

	byte data1[256], data2[256];

	std::string _filename;
};

TEST_F(ReadFile, read) {
	Common::ReadFile readFile(_filename);

	EXPECT_EQ(readFile.pos(), 0);
	EXPECT_FALSE(readFile.eos());

	byte testData[256];
	EXPECT_EQ(readFile.read(testData, 256), 256);
	EXPECT_EQ(readFile.pos(), 256);
	EXPECT_FALSE(readFile.eos());
	for (int i = 0; i < 256; ++i) {
		EXPECT_EQ(data1[i], testData[i]);
	}

	EXPECT_EQ(readFile.read(testData, 256), 256);
	EXPECT_EQ(readFile.pos(), 512);
	EXPECT_TRUE(readFile.eos());
	for (int i = 0; i < 256; ++i) {
		EXPECT_EQ(data2[i], testData[i]);
	}

    readFile.seek(-10, Common::ReadStream::CURRENT);
    EXPECT_EQ(readFile.pos(), 502);
    EXPECT_FALSE(readFile.eos());
    EXPECT_EQ(readFile.read(testData, 10), 10);
    EXPECT_EQ(readFile.pos(), 512);
    EXPECT_TRUE(readFile.eos());

	EXPECT_EQ(readFile.read(testData, 256), 0);
    EXPECT_ANY_THROW(readFile.pos());
	EXPECT_TRUE(readFile.eos());
}

TEST_F(ReadFile, seek) {
    Common::ReadFile readFile(_filename);

    EXPECT_EQ(readFile.pos(), 0);
    EXPECT_FALSE(readFile.eos());

	readFile.seek(0, Common::ReadStream::BEGIN);
	EXPECT_EQ(readFile.pos(), 0);
	EXPECT_FALSE(readFile.eos());

    readFile.seek(10, Common::ReadStream::BEGIN);
    EXPECT_EQ(readFile.pos(), 10);
    EXPECT_FALSE(readFile.eos());

    readFile.seek(5, Common::ReadStream::CURRENT);
    EXPECT_EQ(readFile.pos(), 15);
    EXPECT_FALSE(readFile.eos());

	readFile.seek(-5, Common::ReadStream::CURRENT);
	EXPECT_EQ(readFile.pos(), 10);
	EXPECT_FALSE(readFile.eos());

	readFile.seek(0, Common::ReadStream::CURRENT);
	EXPECT_EQ(readFile.pos(), 10);
	EXPECT_FALSE(readFile.eos());

    readFile.seek(0, Common::ReadStream::END);
    EXPECT_EQ(readFile.pos(), 512);
    EXPECT_TRUE(readFile.eos());

    readFile.seek(-5, Common::ReadStream::END);
    EXPECT_EQ(readFile.pos(), 507);
    EXPECT_FALSE(readFile.eos());

	readFile.seek(0, Common::ReadStream::END);
	EXPECT_EQ(readFile.pos(), 512);
	EXPECT_TRUE(readFile.eos());

    readFile.seek(5, Common::ReadStream::END);
    EXPECT_EQ(readFile.pos(), 517);
    EXPECT_TRUE(readFile.eos());
}

TEST_F(ReadFile, size) {
	Common::ReadFile readFile(_filename);

	EXPECT_EQ(readFile.size(), 512);
}

TEST_F(ReadFile, invalidFile) {
    EXPECT_ANY_THROW(Common::ReadFile readFile(_filename + "_invalid"));
}
