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

#ifndef OPENAWE_ATTRIBUTEOBJECT_H
#define OPENAWE_ATTRIBUTEOBJECT_H

#include <memory>

#include "src/graphics/vertexattribute.h"
#include "src/graphics/buffer.h"

namespace Graphics {

class AttributeObject;

typedef std::shared_ptr<AttributeObject> AttributeObjectPtr;

class AttributeObject : Common::Noncopyable {
public:
	virtual void addAttributes(
		const std::vector<VertexAttribute> &vertexAttributes,
		BufferPtr vertexData,
		unsigned int offset,
		bool perInstance
	) = 0;
};

} // End of namespace Graphics

#endif //OPENAWE_ATTRIBUTEOBJECT_H
