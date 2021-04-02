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

#ifndef AWE_PROGRAM_H
#define AWE_PROGRAM_H

#include <map>

#include "src/common/writestream.h"

#include "src/graphics/shaderconverter.h"
#include "src/graphics/types.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/shader.h"

namespace Graphics::OpenGL {

class Program : Common::Noncopyable {
public:
	Program();
	~Program();

	void attach(const Shader &shader) const;
	void validate() const;
	void link();

	void bind() const;

	void setSamplerMappings(const std::map<std::string, std::string> &mappings);
	void setAttributeMappings(const std::map<AttributeType, std::string> &mappings);
	std::optional<GLint> getAttributeLocation(const AttributeType &type);

	// ,--- Uniform access
	void setSymbols(const std::vector<ShaderConverter::Symbol> &symbols);

	void setUniform1f(const std::string &name, const glm::vec1 &value) const;
	void setUniform2f(const std::string &name, const glm::vec2 &value) const;
	void setUniform3f(const std::string &name, const glm::vec3 &value) const;
	void setUniform4f(const std::string &name, const glm::vec4 &value) const;
	void setUniformMatrix4f(const std::string &name, const glm::mat4 &value) const;
	void setUniformSampler(const std::string &name, const GLuint value) const;
	// '---

private:
	std::optional<GLint> getUniformArraySymbolLocation(const ShaderConverter::Symbol &symbol, unsigned int offset = 0) const;

	std::optional<GLint> getUniformLocation(const std::string &name) const;
	std::optional<GLint> getAttributeLocation(const std::string &name) const;

	std::map<std::string, GLuint> _attributes;
	std::map<std::string, GLuint> _uniforms;

	std::map<std::string, std::string> _samplerMappings;
	std::map<AttributeType, std::string> _attributeMappings;
	std::map<std::string, ShaderConverter::Symbol> _symbols;

	GLuint _id;
};

}

#endif //AWE_PROGRAM_H
