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

#include <regex>
#include <iostream>
#include <sstream>

#include "src/common/xml.h"

namespace Common {

XML::Node &XML::getRootNode() {
	return _rootNode;
}

void XML::read(ReadStream &xml) {
	_rootNode.name = "";

	std::stringstream ss;
	while (!xml.eos()) {
		ss << xml.readLine() << "\n";
	}

	std::string text = ss.str();

	LIBXML_TEST_VERSION

	text = std::regex_replace(text, std::regex("\\r|\\n|\\t"), "");

	xmlDocPtr doc;
	doc = xmlReadMemory(text.c_str(), text.length(), nullptr, nullptr, 0);

	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	readNode(rootNode, _rootNode);

	xmlFreeDoc(doc);
}

void XML::write(WriteStream &xml, bool header) {
	if (header)
		xml.writeString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	if (_rootNode.name.empty())
		return;

	writeNode(xml, _rootNode, 0);
}

void XML::writeNode(WriteStream &xml, const XML::Node &node, unsigned int indent) {
	for (int i = 0; i < indent; ++i) {
		xml.writeString("\t");
	}
	xml.writeString("<" + node.name);
	for (const auto &property : node.properties) {
		xml.writeString(" " + property.first + "=\"" + property.second + "\"");
	}

	if (node.content.empty() && node.children.empty()) {
		xml.writeString("/>\n");
		return;
	}
	xml.writeString(">");

	if (!node.content.empty()) {
		xml.writeString(node.content);
		xml.writeString("</" + node.name + ">\n");
		return;
	} else {
		xml.writeString("\n");
		for (const auto &child : node.children) {
			writeNode(xml, child, indent + 1);
		}
	}


	for (int i = 0; i < indent; ++i) {
		xml.writeString("\t");
	}
	xml.writeString("</" + node.name + ">\n");
}

void XML::readNode(xmlNodePtr node, Node &xmlNode) {
	if (node->name)
		xmlNode.name = std::string(reinterpret_cast<const char *>(node->name));
	if (node->content)
		xmlNode.content = reinterpret_cast<const char *>(node->content);

	for (xmlAttrPtr attrib = node->properties; attrib; attrib = attrib->next) {
		std::string name;
		std::string value;
		if (attrib->name)
			name = reinterpret_cast<const char *>(attrib->name);
		if (attrib->children)
			value = reinterpret_cast<const char *>(attrib->children->content);

		xmlNode.properties[name] = value;
	}

	if (!node->children)
		return;

	xmlNodePtr childNode;
	for (childNode = node->children; childNode; childNode = childNode->next) {
		Node newNode;

		if (childNode->type != XML_ELEMENT_NODE)
			continue;

		readNode(childNode, newNode);

		xmlNode.children.push_back(newNode);
	}
}

}
