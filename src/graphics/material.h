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
#include <glm/gtx/hash.hpp>
#include <boost/container_hash/hash.hpp>

#include "src/common/glmutil.h"

#include "src/graphics/buffer.h"
#include "src/graphics/texture.h"
#include "src/graphics/textureman.h"

namespace Graphics {

class Material {
public:
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
		int index{-1};

		Uniform(const std::string &id, UniformData value) : id(id), data(value) {}
		Uniform(const std::string &id, std::string value) : id(id), data(TextureMan.getTexture(value)) {}

		friend bool operator==(const Uniform &lhs, const Uniform &rhs) = default;
	};

	Material();
	Material(
			const std::string &shaderName,
			std::vector<std::string> stages,
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

	void setBuffer(BufferPtr buffer);
	BufferPtr getBuffer() const;

	friend bool operator==(const Material &lhs, const Material &rhs) = default;

private:
	friend struct std::hash<Material>;

	BufferPtr _buffer;
	CullMode _cullMode;
	BlendMode _blendMode;
	bool _refractive, _specular, _castShadow;
	uint32_t _properties;
	std::map<std::string, std::vector<Uniform>> _attributes;
	std::vector<std::string> _stages;
	std::string _shaderName;
};

}

template <>
struct std::hash<Graphics::Material::Uniform> {
	std::size_t operator()(const Graphics::Material::Uniform &uniform) const {
		std::size_t seed = 0;

		boost::hash_combine(seed, uniform.id);
		boost::hash_combine(seed, uniform.data);
		boost::hash_combine(seed, uniform.index);

		return seed;
	}
};

template <> struct boost::hash<Graphics::Material::Uniform> : std::hash<Graphics::Material::Uniform> {};

template <>
struct std::hash<Graphics::Material> {
	std::size_t operator()(Graphics::Material const &material) const {
		size_t seed = 0;

		boost::hash_combine(seed, material._buffer.get());
		boost::hash_combine(seed, material._cullMode);
		boost::hash_combine(seed, material._blendMode);
		boost::hash_combine(seed, material._refractive);
		boost::hash_combine(seed, material._specular);
		boost::hash_combine(seed, material._castShadow);
		boost::hash_combine(seed, material._properties);
		boost::hash_combine(seed, material._attributes);
		boost::hash_combine(seed, material._stages);
		boost::hash_combine(seed, material._shaderName);

		return seed;
	}
};

#endif //AWE_MATERIAL_H
