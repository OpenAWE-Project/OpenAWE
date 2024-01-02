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

#include <tinyxml2.h>

#include "src/common/readstream.h"
#include "src/common/writestream.h"

namespace Common {

/*!
 * \brief Class to simplify handling of xml documents
 *
 * This class is a helper class to handle xml documents by using DOM structure with generic nodes. It doesn't support
 * more advanced xml features like comments (will be simply skipped) or dtd's.
 */
class XML {
public:
	class Node {
	public:
		Node() = default;

		std::string name;
		std::string content;

		std::map<std::string, std::string> properties;
		std::vector<Node> children;

		/*!
		 * Add a new child node to this node
		 * \param name The name of the new node
		 * \return A reference to the newly created node
		 */
		Node &addNewNode(const std::string &name);

		/*!
		 * Check if a given property is available in the node
		 * @param attribute The attribute name to check for its presence
		 * @return If the property exists
		 */
		bool hasProperty(const std::string &attribute) const;

		/*!
		 * Get an attribute of the node as string
		 * @param attribute the attribute to search for
		 * @return the searched attribute or empty string if it doesn't exist
		 */
		const std::string &getString(const std::string &attribute) const;

		/*!
		 * Get an attribute of the node as integer
		 * @param attribute the attribute to search for
		 * @return The searched attribute as int
		 */
		int getInt(const std::string &attribute) const;

		/*!
		 * Get an attribute of the node as float
		 * @param attribute the attribute to search for
		 * @return The searched attribute as float
		 */
		float getFloat(const std::string &attribute) const;

		/*!
		 * Get an attribute of the node as vec3
		 * \param attribute The attribute to search for
		 * \return The searched attribute as vec3
		 */
		glm::vec3 getVec3(const std::string &attribute) const;
	};

	/*!
	 * Read xml data from a ReadStream object to the hierarchy. any other available hierarchy will be dropped
	 * @param xml The xml readstream to parse
	 */
	void read(ReadStream &xml);

	/*!
	 * Write xml data from the current DOM hierarchy into a write stream.
	 * @param xml The xml write stream
	 * @param header If a BOM header should be prepended
	 */
	void write(WriteStream &xml, bool header);

	/*!
	 * Get the current root node of the DOM hierarchy
	 * @return The current root node
	 */
	Node &getRootNode();

private:
	static void writeNode(tinyxml2::XMLElement *element, const XML::Node &node);
	static void readNode(tinyxml2::XMLElement *element, Node &xmlNode);

	Node _rootNode;
};

} // End of namespace Common

#endif // SRC_COMMON_XML_H
