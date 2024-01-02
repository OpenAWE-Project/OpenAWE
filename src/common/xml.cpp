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

#include "src/common/strutil.h"
#include "src/common/exception.h"
#include "src/common/xml.h"

namespace Common {

XML::Node &XML::Node::addNewNode(const std::string &name) {
	Node newNode;
	newNode.name = name;
	return children.emplace_back(newNode);
}

bool XML::Node::hasProperty(const std::string &attribute) const {
	return properties.find(attribute) != properties.end();
}

const std::string &XML::Node::getString(const std::string &attribute) const {
	const auto iter = properties.find(attribute);
	if (iter == properties.end())
		throw CreateException("Property {} not found in tag", attribute);

	return iter->second;
}

int XML::Node::getInt(const std::string &attribute) const {
	const auto iter = properties.find(attribute);
	if (iter == properties.end())
		throw CreateException("Property {} not found in tag", attribute);

	return std::stoi(iter->second);
}

float XML::Node::getFloat(const std::string &attribute) const {
	const auto iter = properties.find(attribute);
	if (iter == properties.end())
		throw CreateException("Property {} not found in tag", attribute);

	return std::stof(iter->second);
}

glm::vec3 XML::Node::getVec3(const std::string &attribute) const {
	const auto iter = properties.find(attribute);
	if (iter == properties.end())
		throw CreateException("Property {} not found in tag", attribute);

	const auto valueStrings = Common::split(iter->second, std::regex(" "));

	if (valueStrings.size() != 3)
		throw CreateException("Incorrect number of values detected, expected 3, got {}", valueStrings.size());

	return {
		std::stof(valueStrings[0]),
		std::stof(valueStrings[1]),
		std::stof(valueStrings[2])
	};
}

XML::Node &XML::getRootNode() {
	return _rootNode;
}

void XML::read(ReadStream &xml) {
	// Clear the root node
	_rootNode = Node{};

	// Read the xml document line by line
	std::stringstream ss;
	while (!xml.eos()) {
		ss << xml.readLine() << "\n";
	}

	// Get the whole text
	std::string text = ss.str();

	// Parse the document and get the root element
	tinyxml2::XMLDocument doc;
	doc.Parse(text.c_str());
	if (doc.Error())
		throw Common::Exception("Error while parsing xml document {} {}", doc.ErrorLineNum(), doc.ErrorStr());
	const auto rootNode = doc.RootElement();

	// Read recursively from the root node
	readNode(rootNode, _rootNode);
}

void XML::write(WriteStream &xml, bool header) {
	if (_rootNode.name.empty())
		return;

	tinyxml2::XMLDocument doc;

	doc.SetBOM(header);
	doc.InsertFirstChild(doc.NewElement(""));
	writeNode(doc.RootElement(), _rootNode);

	// Print the data to a string
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);

	// And write it to the write stream
	const std::string result(printer.CStr());
	xml.write(result.c_str(), result.size());
}

void XML::writeNode(tinyxml2::XMLElement *element, const XML::Node &node) {
	// Set the name of the root node
	element->SetName(node.name.c_str());

	// Write all attributes
	for (const auto &property : node.properties) {
		element->SetAttribute(property.first.c_str(), property.second.c_str());
	}

	// If the node is a text node, then write a text element
	if (!node.content.empty()) {
		tinyxml2::XMLText *text = element->GetDocument()->NewText(node.content.c_str());
		element->InsertEndChild(text);
	// Id the node has child nodes write the child nodes
	} else {
		for (const auto &child : node.children) {
			tinyxml2::XMLNode *newChild;
			newChild = element->GetDocument()->NewElement(child.name.c_str());
			writeNode(newChild->ToElement(), child);
			element->InsertEndChild(newChild);
		}
	}
}

void XML::readNode(tinyxml2::XMLElement *node, Node &xmlNode) {
	// Set the name of the node
	xmlNode.name = node->Name();

	// Set the attributes of the node
	for (auto attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		xmlNode.properties[attribute->Name()] = attribute->Value();
	}

	// Set potential childs of the node
	for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
		Node childNode;

		if (child->ToComment() != nullptr)
			continue;

		const auto xmlText = child->ToText();
		const auto xmlElement = child->ToElement();

		if (xmlText) {
			// If the node doesn't have a
			xmlNode.content = xmlText->Value();
			break;
		} else if (xmlElement) {
			readNode(xmlElement, childNode);
		}

		xmlNode.children.emplace_back(childNode);
	}
}

}
