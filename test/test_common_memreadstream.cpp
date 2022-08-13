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

#include "lipsum.h"

TEST(MemoryReadStream, seek) {
	static const byte data1[42] = {0};
	static const byte data2[46] = {0};

	Common::MemoryReadStream memoryReadStream1(data1, 42);
	Common::MemoryReadStream memoryReadStream2(data2, 46);

	memoryReadStream1.seek(21, Common::ReadStream::CURRENT);
	EXPECT_EQ(memoryReadStream1.pos(), 21);
	EXPECT_FALSE(memoryReadStream1.eos());

	memoryReadStream1.seek(12, Common::ReadStream::CURRENT);
	EXPECT_EQ(memoryReadStream1.pos(), 33);
	EXPECT_FALSE(memoryReadStream1.eos());

	memoryReadStream1.seek(21, Common::ReadStream::BEGIN);
	EXPECT_EQ(memoryReadStream1.pos(), 21);
	EXPECT_FALSE(memoryReadStream1.eos());

	memoryReadStream1.seek(21, Common::ReadStream::CURRENT);
	EXPECT_EQ(memoryReadStream1.pos(), 42);
	EXPECT_TRUE(memoryReadStream1.eos());

	memoryReadStream1.seek(0, Common::ReadStream::END);
	EXPECT_EQ(memoryReadStream1.pos(), 42);
	EXPECT_TRUE(memoryReadStream1.eos());

	memoryReadStream2.skip(21);
	EXPECT_EQ(memoryReadStream2.pos(), 21);
	EXPECT_FALSE(memoryReadStream2.eos());

	memoryReadStream2.skip(25);
	EXPECT_EQ(memoryReadStream2.pos(), 46);
	EXPECT_TRUE(memoryReadStream2.eos());

	memoryReadStream2.skip(-21);
	EXPECT_EQ(memoryReadStream2.pos(), 25);
	EXPECT_FALSE(memoryReadStream2.eos());

	EXPECT_ANY_THROW(memoryReadStream1.seek(43, Common::ReadStream::BEGIN));
	EXPECT_ANY_THROW(memoryReadStream2.seek(1, Common::ReadStream::END));
}

TEST(MemoryReadStream, read) {
	byte data[512];

	// Make 256 byte forwards and backwards
	for (int i = 0; i < 256; ++i) {
		data[i] = i;
	}
	for (int i = 0; i < 256; ++i) {
		data[i + 256] = 255 - i;
	}

	Common::MemoryReadStream memoryReadStream(data, 512, false);
	byte data1[256];
	byte data2[256];
	EXPECT_EQ(memoryReadStream.pos(), 0);
	EXPECT_EQ(memoryReadStream.read(data1, 256), 256);
	EXPECT_EQ(memoryReadStream.pos(), 256);
	EXPECT_EQ(memoryReadStream.read(data2, 256), 256);
	EXPECT_EQ(memoryReadStream.pos(), 512);

	for (int i = 0; i < 256; ++i) {
		EXPECT_EQ(data1[i], i);
		EXPECT_EQ(data2[i], 255 - i);
	}
}

TEST(MemoryReadStream, readByte) {
	static const byte data[4] = {0x00, 0x02, 0x04, 0x08};

	Common::MemoryReadStream stream(data, 4);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readByte(), 0);
	EXPECT_EQ(stream.pos(), 1);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readByte(), 2);
	EXPECT_EQ(stream.pos(), 2);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readByte(), 4);
	EXPECT_EQ(stream.pos(), 3);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readByte(), 8);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint64LE) {
	static const byte data[32] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
			0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x12, 0x00, 0x34, 0x00, 0x00, 0x56, 0x00, 0x78,
			0x78, 0x00, 0x56, 0x00, 0x00, 0x34, 0x00, 0x12
	};

	Common::MemoryReadStream stream(data, 32);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64LE(), 0x0100000000000000);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64LE(), 0x0000000000000001);
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64LE(), 0x7800560000340012);
	EXPECT_EQ(stream.pos(), 24);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64LE(), 0x1200340000560078);
	EXPECT_EQ(stream.pos(), 32);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint64BE) {
	static const byte data[32] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
			0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x12, 0x00, 0x34, 0x00, 0x00, 0x56, 0x00, 0x78,
			0x78, 0x00, 0x56, 0x00, 0x00, 0x34, 0x00, 0x12
	};

	Common::MemoryReadStream stream(data, 32);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64BE(), 0x0000000000000001);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64BE(), 0x0100000000000000);
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64BE(), 0x1200340000560078);
	EXPECT_EQ(stream.pos(), 24);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint64BE(), 0x7800560000340012);
	EXPECT_EQ(stream.pos(), 32);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint32LE) {
	static const byte data[16] = {
			0x00, 0x00, 0x00, 0x01,
			0x01, 0x00, 0x00, 0x00,
			0x12, 0x34, 0x56, 0x78,
			0x78, 0x56, 0x34, 0x12
	};

	Common::MemoryReadStream stream(data, 16);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32LE(), 0x01000000);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32LE(), 0x00000001);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32LE(), 0x78563412);
	EXPECT_EQ(stream.pos(), 12);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32LE(), 0x12345678);
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint32BE) {
	static const byte data[16] = {
			0x00, 0x00, 0x00, 0x01,
			0x01, 0x00, 0x00, 0x00,
			0x12, 0x34, 0x56, 0x78,
			0x78, 0x56, 0x34, 0x12
	};

	Common::MemoryReadStream stream(data, 16);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32BE(), 0x00000001);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32BE(), 0x01000000);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32BE(), 0x12345678);
	EXPECT_EQ(stream.pos(), 12);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint32BE(), 0x78563412);
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint16LE) {
	static const byte data[8] = {
			0x00, 0x01,
			0x01, 0x00,
			0x12, 0x78,
			0x78, 0x12
	};

	Common::MemoryReadStream stream(data, 8);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16LE(), 0x0100);
	EXPECT_EQ(stream.pos(), 2);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16LE(), 0x0001);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16LE(), 0x7812);
	EXPECT_EQ(stream.pos(), 6);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16LE(), 0x1278);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readUint16BE) {
	static const byte data[8] = {
			0x00, 0x01,
			0x01, 0x00,
			0x12, 0x78,
			0x78, 0x12
	};

	Common::MemoryReadStream stream(data, 8);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16BE(), 0x0001);
	EXPECT_EQ(stream.pos(), 2);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16BE(), 0x0100);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16BE(), 0x1278);
	EXPECT_EQ(stream.pos(), 6);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readUint16BE(), 0x7812);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readIEEEFloatLE) {
	static const byte data[8] = {
			0x00, 0x00, 0x80, 0x3f,
			0x00, 0x00, 0x80, 0xbf
	};

	Common::MemoryReadStream stream(data, 8);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readIEEEFloatLE(), 1.0f);
	EXPECT_EQ(stream.pos(), 4);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readIEEEFloatLE(), -1.0f);
	EXPECT_EQ(stream.pos(), 8);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readFixedSizeString) {
	Common::MemoryReadStream stream(reinterpret_cast<const byte*>(::kLipsum), strlen(::kLipsum));

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readFixedSizeString(6).c_str(), "Lorem ");
	EXPECT_EQ(stream.pos(), 6);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readFixedSizeString(6).c_str(), "ipsum ");
	EXPECT_EQ(stream.pos(), 12);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readFixedSizeString(6).c_str(), "dolor ");
	EXPECT_EQ(stream.pos(), 18);
	EXPECT_FALSE(stream.eos());
}

TEST(MemoryReadStream, readFixedSizeStringUTF16) {
	const std::u16string data = u"Lorem ipsum dolor ";
	Common::MemoryReadStream stream(reinterpret_cast<const byte*>(data.data()), data.size() * 2);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readFixedSizeStringUTF16(6), std::u16string(u"Lorem "));
	EXPECT_EQ(stream.pos(), 12);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readFixedSizeStringUTF16(6), std::u16string(u"ipsum "));
	EXPECT_EQ(stream.pos(), 24);
	EXPECT_FALSE(stream.eos());

	EXPECT_EQ(stream.readFixedSizeStringUTF16(6), std::u16string(u"dolor "));
	EXPECT_EQ(stream.pos(), 36);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readFixedSizeStringNullTerminated) {
	const char *kTestString = "Hello World :)\0Hello World #2";
	Common::MemoryReadStream stream(kTestString, 29);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readFixedSizeString(16, true).c_str(), "Hello World :)");
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_FALSE(stream.eos());
}

TEST(MemoryReadStream, readNullTerminatedString) {
	const char *kTestString = "Hello World :)\0Hello World #2\0";
	Common::MemoryReadStream stream(kTestString, 30);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readNullTerminatedString().c_str(), "Hello World :)");
	EXPECT_EQ(stream.pos(), 15);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readNullTerminatedString().c_str(), "Hello World #2");
	EXPECT_EQ(stream.pos(), 30);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readNullTerminatedStringBlocked) {
	const char *kTestString = "Hello World :)\0\0Hello\0\0\0World #2\0\0\0\0";
	Common::MemoryReadStream stream(kTestString, 36);

	EXPECT_EQ(stream.pos(), 0);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readNullTerminatedString(4).c_str(), "Hello World :)");
	EXPECT_EQ(stream.pos(), 16);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readNullTerminatedString(4).c_str(), "Hello");
	EXPECT_EQ(stream.pos(), 24);
	EXPECT_FALSE(stream.eos());

	EXPECT_STREQ(stream.readNullTerminatedString(4).c_str(), "World #2");
	EXPECT_EQ(stream.pos(), 36);
	EXPECT_TRUE(stream.eos());
}

TEST(MemoryReadStream, readStream) {
	const byte kData[] = {
			0x01, 0x02, 0x03, 0x04,
			0x11, 0x12, 0x13, 0x14,
			0x21, 0x22, 0x23, 0x24,
			0x31, 0x32, 0x33, 0x34,
	};

	Common::MemoryReadStream stream(kData, 16);
	std::unique_ptr<Common::ReadStream> stream1(stream.readStream(8));
	stream.seek(0, Common::ReadStream::BEGIN);
	std::unique_ptr<Common::ReadStream> stream2(stream.readStream());

	EXPECT_EQ(stream1->readUint32BE(), 0x01020304);
	EXPECT_EQ(stream1->readUint32BE(), 0x11121314);

	EXPECT_EQ(stream2->readUint32BE(), 0x01020304);
	EXPECT_EQ(stream2->readUint32BE(), 0x11121314);
	EXPECT_EQ(stream2->readUint32BE(), 0x21222324);
	EXPECT_EQ(stream2->readUint32BE(), 0x31323334);
}

TEST(MemoryReadStream, size) {
	static const byte data1[42] = {0};
	static const byte data2[46] = {0};

	Common::MemoryReadStream memoryReadStream1(data1, 42);
	Common::MemoryReadStream memoryReadStream2(data2, 46);

	EXPECT_EQ(memoryReadStream1.size(), 42);
	EXPECT_EQ(memoryReadStream2.size(), 46);
}
