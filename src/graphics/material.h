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

#ifndef AWE_MATERIAL_H
#define AWE_MATERIAL_H

#include <string>
#include <map>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include "src/common/uuid.h"

#include "src/graphics/texture.h"

namespace Graphics {

class Material {
public:
	enum AttributeType {
		kVec1, kVec2, kVec3, kVec4, kTexture
	};

	enum CullMode {
		kNone, kFront, kBack, kFrontBack
	};

	enum BlendMode {
		kOpaque
	};

	typedef std::variant<
			glm::vec1,
			glm::vec2,
			glm::vec3,
			glm::vec4,
			TexturePtr
	> AttributeData;

	struct Attribute {
		std::string id;
		AttributeType type;
		AttributeData data;
		int index;

		Attribute(const std::string &id, glm::vec1 value) : id(id), data(value), type(kVec1), index(-1) {}
		Attribute(const std::string &id, glm::vec2 value) : id(id), data(value), type(kVec2), index(-1) {}
		Attribute(const std::string &id, glm::vec3 value) : id(id), data(value), type(kVec3), index(-1) {}
		Attribute(const std::string &id, glm::vec4 value) : id(id), data(value), type(kVec4), index(-1) {}
		Attribute(const std::string &id, TexturePtr value) : id(id), data(value), type(kTexture), index(-1) {}
	};

	Material() = default;
	Material(const std::string &shaderName, std::vector<Attribute> attributes);

	const std::vector<Attribute> &getAttributes() const;
	const std::string &getShaderName() const;

	void setCullMode(CullMode cullMode);
	CullMode getCullMode() const;

private:
	CullMode _cullMode;
	bool _refractive, _specular, _castShadow;
	std::vector<Attribute> _attributes;
	std::string _shaderName;
};

}

#endif //AWE_MATERIAL_H
