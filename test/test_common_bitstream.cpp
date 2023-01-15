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
#include <bitset>

#include "src/common/memreadstream.h"
#include "src/common/bitstream.h"
#include "src/common/endianness.h"

// 0000 0001 0010 0011
static const uint8_t kTestData[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

constexpr std::bitset<64> kResultData           = {0x0123456789ABCDEF};
constexpr std::bitset<64> kResultData16LE       = {0x23016745AB89EFCD};
constexpr std::bitset<64> kResultData32LE       = {0x67452301EFCDAB89};
constexpr std::bitset<64> kResultDataReversed   = {0x80C4A2E691D5B3F7};
constexpr std::bitset<64> kResultDataReversed16 = {0xC480E6A2D591F7B3};
constexpr std::bitset<64> kResultDataReversed32 = {0xE6A2C480F7B3D591};

TEST(BitStream, readBit_8MSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream8MSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultData, result);
}

TEST(BitStream, readBit_8LSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream8LSB bits(testDataStream);

	std::bitset<64> result;
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultDataReversed, result);
}

TEST(BitStream, readBits_8MSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream8MSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 0);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 4);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_8LSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream8LSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 1);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 6);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 2);
}

TEST(BitStream, readBit_16BEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16BEMSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultData, result);
}

TEST(BitStream, readBit_16BELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16BELSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultDataReversed16, result);
}

TEST(BitStream, readBit_16LEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16LEMSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultData16LE, result);
}

TEST(BitStream, readBit_16LELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16LELSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultDataReversed, result);
}

TEST(BitStream, readBits_16BEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16BEMSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 0);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 4);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_16BELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16BELSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 3);
	EXPECT_EQ(bits.read(3), 2);
	EXPECT_EQ(bits.read(3), 2);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_16LEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16LEMSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 2);
	EXPECT_EQ(bits.read(3), 1);
	EXPECT_EQ(bits.read(3), 4);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_16LELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream16LELSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 1);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 6);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 2);
}

TEST(BitStream, readBit_32BEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32BEMSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultData, result);
}

TEST(BitStream, readBit_32BELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32BELSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultDataReversed32, result);
}

TEST(BitStream, readBit_32LEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32LEMSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultData32LE, result);
}

TEST(BitStream, readBit_32LELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32LELSB bits(testDataStream);

	std::bitset<64> result(0);
	for (int i = 0; i < 64; ++i)
		result[63 - i] = bits.read();

	EXPECT_EQ(kResultDataReversed, result);
}

TEST(BitStream, readBits_32BEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32BEMSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 0);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 4);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_32BELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32BELSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 7);
	EXPECT_EQ(bits.read(3), 6);
	EXPECT_EQ(bits.read(3), 2);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 0);
}

TEST(BitStream, readBits_32LEMSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32LEMSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 6);
	EXPECT_EQ(bits.read(3), 3);
	EXPECT_EQ(bits.read(3), 5);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 1);
}

TEST(BitStream, readBits_32LELSB) {
	Common::MemoryReadStream testDataStream(kTestData, sizeof(kTestData));
	Common::BitStream32LELSB bits(testDataStream);

	EXPECT_EQ(bits.read(4), 1);
	EXPECT_EQ(bits.read(3), 0);
	EXPECT_EQ(bits.read(3), 6);
	bits.skip(2);
	EXPECT_EQ(bits.read(2), 2);
}

