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
#include "src/common/xml.h"

static const char* kXmlData = "<nodeType1 attributeInt=\"13\" attributeInt2=\"14\" attributeString=\"TestString\">\n"
							  "  <!-- Comment -->"
							  "  <subNode1 attributeFloat=\"0.0\" attributeFloat2=\"0.5\" attributeFloat3=\"1.0f\" attributeFloat4=\"1.f\"/>\n"
							  "  <subNode2>\n"
							  "    <subNode3 attributeInt=\"-13\"/>\n"
							  "    <subNode4 attributeInt=\"+13\"/>\n"
							  "    <subNode5 />\n"
							  "  </subNode2>\n"
							  "</nodeType1>";

static const char* kInvalidXmlData1 = "<nodeType1 attributeInt2=14>";
static const char* kInvalidXmlData2 = "";
static const char* kInvalidXmlData3 = "<!-- Test";


TEST(XML, readOnly) {
	Common::MemoryReadStream xmlStream(kXmlData, std::strlen(kXmlData));

	Common::XML xml;
	ASSERT_NO_THROW(xml.read(xmlStream));

	Common::XML::Node &rootNode = xml.getRootNode();
	EXPECT_EQ(rootNode.children.size(), 2);

	EXPECT_EQ(rootNode.getInt("attributeInt"), 13);
	EXPECT_EQ(rootNode.getInt("attributeInt2"), 14);
	EXPECT_STREQ(rootNode.getString("attributeString").c_str(), "TestString");

	Common::XML::Node &subNode1 = rootNode.children[0];
	Common::XML::Node &subNode2 = rootNode.children[1];

	EXPECT_EQ(subNode1.children.size(), 0);
	EXPECT_EQ(subNode2.children.size(), 3);

	EXPECT_STREQ(subNode1.name.c_str(), "subNode1");
	EXPECT_STREQ(subNode2.name.c_str(), "subNode2");

	EXPECT_EQ(subNode1.getFloat("attributeFloat"), 0.0f);
	EXPECT_EQ(subNode1.getFloat("attributeFloat2"), 0.5f);
	EXPECT_EQ(subNode1.getFloat("attributeFloat3"), 1.0f);
	EXPECT_EQ(subNode1.getFloat("attributeFloat4"), 1.0f);

	Common::XML::Node &subNode3 = subNode2.children[0];
	Common::XML::Node &subNode4 = subNode2.children[1];
	Common::XML::Node &subNode5 = subNode2.children[2];

	EXPECT_STREQ(subNode3.name.c_str(), "subNode3");
	EXPECT_STREQ(subNode4.name.c_str(), "subNode4");
	EXPECT_STREQ(subNode5.name.c_str(), "subNode5");

	EXPECT_EQ(subNode3.children.size(), 0);
	EXPECT_EQ(subNode4.children.size(), 0);
	EXPECT_EQ(subNode5.children.size(), 0);

	EXPECT_EQ(subNode3.getInt("attributeInt"), -13);
	EXPECT_EQ(subNode4.getInt("attributeInt"), 13);
}

TEST(XML, invalidData) {
	Common::MemoryReadStream xmlStream1(kInvalidXmlData1, std::strlen(kInvalidXmlData1));
	Common::MemoryReadStream xmlStream2(kInvalidXmlData2, std::strlen(kInvalidXmlData2));
	Common::MemoryReadStream xmlStream3(kInvalidXmlData3, std::strlen(kInvalidXmlData3));

	Common::XML xml;

	EXPECT_ANY_THROW(xml.read(xmlStream1));
	EXPECT_ANY_THROW(xml.read(xmlStream2));
	EXPECT_ANY_THROW(xml.read(xmlStream3));
}
