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
	enum UniformType {
		kFloat,
		kVec2,
		kVec3,
		kVec4,
		kFloatArray,
		kVec2Array,
		kVec3Array,
		kVec4Array,
		kTexture
	};

	enum CullMode {
		kNone, kFront, kBack, kFrontBack
	};

	typedef std::variant<
			float,
			glm::vec2,
			glm::vec3,
			glm::vec4,
			std::vector<float>,
			std::vector<glm::vec2>,
			std::vector<glm::vec3>,
			std::vector<glm::vec4>,
			TexturePtr
	> UniformData;

	struct Uniform {
		std::string id;
		UniformData data;
		UniformType type;
		int index;

		Uniform(const std::string &id, float value) : id(id), data(value), type(kFloat), index(-1) {}
		Uniform(const std::string &id, glm::vec2 value) : id(id), data(value), type(kVec2), index(-1) {}
		Uniform(const std::string &id, glm::vec3 value) : id(id), data(value), type(kVec3), index(-1) {}
		Uniform(const std::string &id, glm::vec4 value) : id(id), data(value), type(kVec4), index(-1) {}
		Uniform(const std::string &id, std::vector<float> value) : id(id), data(value), type(kFloatArray), index(-1) {}
		Uniform(const std::string &id, std::vector<glm::vec2> value) : id(id), data(value), type(kVec2Array), index(-1) {}
		Uniform(const std::string &id, std::vector<glm::vec3> value) : id(id), data(value), type(kVec3Array), index(-1) {}
		Uniform(const std::string &id, std::vector<glm::vec4> value) : id(id), data(value), type(kVec4Array), index(-1) {}
		Uniform(const std::string &id, TexturePtr value) : id(id), data(value), type(kTexture), index(-1) {}
	};

	Material();
	Material(
		const std::string &shaderName,
		std::initializer_list<std::string> stages,
		std::vector<Uniform> attributes,
		uint32_t properties = 0
	);

	const std::vector<std::string> &getStages() const;

	const std::vector<Uniform> &getUniforms(const std::string &stage) const;
	const std::string &getShaderName() const;

	void setCullMode(CullMode cullMode);
	CullMode getCullMode() const;

	BlendMode getBlendMode() const;

	void setBlendMode(BlendMode blendMode);

	bool hasStage(const std::string &stage) const;
	uint32_t getProperties() const;

private:
	CullMode _cullMode;
	BlendMode _blendMode;
	bool _refractive, _specular, _castShadow;
	uint32_t _properties;
	std::map<std::string, std::vector<Uniform>> _attributes;
	std::vector<std::string> _stages;
	std::string _shaderName;
};

}

#endif //AWE_MATERIAL_H
