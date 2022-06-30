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

#include <fmt/format.h>
#include <glm/gtx/string_cast.hpp>
#include <utility>

#include "src/common/exception.h"

#include "src/awe/objectxmlwritestream.h"

namespace AWE {

ObjectXMLWriteStream::ObjectXMLWriteStream(Common::XML::Node &rootNode) : _rootNode(rootNode), _index(0) {
	_objectNode.emplace(_rootNode);
}

void ObjectXMLWriteStream::setBytecodeCollection(std::shared_ptr<AWE::Script::Collection> collection) {
	_collection = std::move(collection);
}

void ObjectXMLWriteStream::writeObject(AWE::Object object, ObjectType type, unsigned int version) {
	Common::XML::Node &currentNode = _objectNode.top();
	auto &newObjectNode = currentNode.addNewNode("object");
	newObjectNode.properties["version"] = std::to_string(version);
	if (_objectNode.size() == 1)
		newObjectNode.properties["index"] = std::to_string(_index);
	_objectNode.push(newObjectNode);

	ObjectStream::object(object, type, version);

	_objectNode.pop();
	_index++;
}

void ObjectXMLWriteStream::skip(size_t s) {
	// We don't need to skip anything when writing, so this method remains blank
}

void ObjectXMLWriteStream::variable(const std::string &name, bool &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = std::to_string(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, int32_t &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = std::to_string(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, uint32_t &value, bool bigEndian) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = std::to_string(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, float &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = std::to_string(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, std::string &value, bool dp) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = value;
}

void ObjectXMLWriteStream::variable(const std::string &name, glm::vec3 &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = glm::to_string<glm::vec3>(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, glm::mat3 &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	newVariableNode.content = glm::to_string<glm::mat3>(value);
}

void ObjectXMLWriteStream::variable(const std::string &name, GID &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	auto &typeNode = newVariableNode.addNewNode("type");
	typeNode.content = std::to_string(value.type);
	auto &hashNode = newVariableNode.addNewNode("hash");
	hashNode.content = fmt::format("0x{:x}", value.id);
}

void ObjectXMLWriteStream::variable(const std::string &name, ObjectID &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;
	auto &typeNode = newVariableNode.addNewNode("type");
	typeNode.content = std::to_string(value.getType());
	auto &hashNode = newVariableNode.addNewNode("id");
	hashNode.content = fmt::format("0x{:x}", value.getID());
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<bool> &value, size_t fixedSize) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	assert(value.size() == fixedSize);
	for (int i = 0; i < fixedSize; ++i) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = std::to_string(value[i]);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<int32_t> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item: value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = std::to_string(item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<uint32_t> &value, bool dp) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item : value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = std::to_string(item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<int32_t> &value, size_t fixedSize) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	if (value.size() < fixedSize)
		throw Common::Exception(
				"Invalid size of vector for fixed size. Needs at least {} elements but only has {}",
				fixedSize,
				value.size()
		);

	for (const auto &item: value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = std::to_string(item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<rid_t> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item: value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = fmt::format("{:x}", item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<glm::vec2> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item: value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = glm::to_string(item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<float> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item : value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = std::to_string(item);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<ObjectID> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item : value) {
		auto &itemNode = newVariableNode.addNewNode("item");

		auto &typeNode = itemNode.addNewNode("type");
		typeNode.content = std::to_string(item.getType());

		auto &idNode = itemNode.addNewNode("id");
		idNode.content = std::to_string(item.getID());
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<GID> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &gid: value) {
		auto &itemNode = newVariableNode.addNewNode("item");

		auto &typeNode = itemNode.addNewNode("type");
		typeNode.content = std::to_string(gid.type);

		auto &idNode = itemNode.addNewNode("id");
		idNode.content = fmt::format("{:x}", gid.id);
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<std::string> &value) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	for (const auto &item : value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = item;
	}
}

void ObjectXMLWriteStream::variable(const std::string &name, std::vector<std::string> &value, size_t fixedSize) {
	Common::XML::Node &currentObjectNode = _objectNode.top();
	auto &newVariableNode = currentObjectNode.addNewNode("variable");
	newVariableNode.properties["name"] = name;

	if (value.size() < fixedSize)
		throw Common::Exception(
				"Invalid size of vector for fixed size. Needs at least {} elements but only has {}",
				fixedSize,
				value.size()
		);

	for (const auto &item: value) {
		auto &itemNode = newVariableNode.addNewNode("item");
		itemNode.content = item;
	}
}

void ObjectXMLWriteStream::object(const std::string &name, Object &value, ObjectType type) {
	Common::XML::Node &currentNode = _objectNode.top();
	auto &newObjectNode = currentNode.addNewNode("object");
	newObjectNode.properties["name"] = name;
	_objectNode.push(newObjectNode);

	ObjectStream::object(value, type, 0);

	// If we have script variables and have the necessary collection information, try to disassemble the byte code
	if (type == kScriptVariables && _collection) {
		newObjectNode.children.clear();
		Script::Disassembler disassembler = _collection->createDisassembler(
				std::any_cast<Templates::ScriptVariables>(value)
		);
		newObjectNode.content = disassembler.generate();
	}

	_objectNode.pop();
}

void ObjectXMLWriteStream::objects(const std::string &name, std::vector<Object> &value, ObjectType type) {
	Common::XML::Node &currentNode = _objectNode.top();
	auto &newObjectsNode = currentNode.addNewNode("objects");
	newObjectsNode.properties["name"] = name;
	_objectNode.push(newObjectsNode);

	for (auto &item: value) {
		auto &newObjectNode = newObjectsNode.addNewNode("object");
		ObjectStream::object(item, type, 0);
	}

	_objectNode.pop();
}

} // End of namespace AWE
