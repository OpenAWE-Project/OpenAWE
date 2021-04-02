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

#ifndef SRC_COMMON_XML_H
#define SRC_COMMON_XML_H

#include <string>
#include <vector>
#include <map>
#include <regex>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "readstream.h"
#include "writestream.h"

namespace Common {

class XML {
public:
	class Node {
	public:
		Node() = default;

		std::string name;
		std::string content;

		std::map<std::string, std::string> properties;
		std::vector<Node> children;
	};

	void read(ReadStream &xml);
	void write(WriteStream &xml, bool header);

	Node &getRootNode();

private:
	static void writeNode(WriteStream &xml, const XML::Node &node, unsigned int indent);
	void writeIndent(WriteStream &xml, size_t indent);

	static void readNode(xmlNodePtr node, Node &xmlNode);

	Node _rootNode;
};

} // End of namespace Common

#endif // SRC_COMMON_XML_H
