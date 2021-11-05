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

#ifndef OPENAWE_OBJECTXMLWRITESTREAM_H
#define OPENAWE_OBJECTXMLWRITESTREAM_H

#include <stack>

#include "src/common/xml.h"

#include "src/awe/objectstream.h"

namespace AWE {

class ObjectXMLWriteStream : public ObjectWriteStream {
public:
	ObjectXMLWriteStream(Common::XML::Node &rootNode);

	void writeObject(Object object, ObjectType type, unsigned int version) override;

protected:
	void skip(size_t s) override;

	void variable(const std::string &name, bool &value) override;
	void variable(const std::string &name, int32_t &value) override;
	void variable(const std::string &name, uint32_t &value, bool bigEndian) override;
	void variable(const std::string &name, float &value) override;
	void variable(const std::string &name, std::string &value, bool dp) override;
	void variable(const std::string &name, glm::vec3 &value) override;
	void variable(const std::string &name, glm::mat3 &value) override;
	void variable(const std::string &name, GID &value) override;
	void variable(const std::string &name, ObjectID &value) override;
	void variable(const std::string &name, std::vector<bool> &value, size_t fixedSize) override;
	void variable(const std::string &name, std::vector<int32_t> &value) override;
	void variable(const std::string &name, std::vector<uint32_t> &value, bool dp) override;
	void variable(const std::string &name, std::vector<int32_t> &value, size_t fixedSize) override;
	void variable(const std::string &name, std::vector<rid_t> &value) override;
	void variable(const std::string &name, std::vector<glm::vec2> &value) override;
	void variable(const std::string &name, std::vector<float> &value) override;
	void variable(const std::string &name, std::vector<ObjectID> &value) override;
	void variable(const std::string &name, std::vector<std::string> &value) override;
	void variable(const std::string &name, std::vector<std::string> &value, size_t fixedSize) override;

	void object(const std::string &name, Object &value, ObjectType type) override;
	void objects(const std::string &name, std::vector<Object> &value, ObjectType type) override;

private:
	Common::XML::Node &_rootNode;
	std::stack<std::reference_wrapper<Common::XML::Node>> _objectNode;
};

} // End of namespace AWE

#endif //OPENAWE_OBJECTXMLWRITESTREAM_H
