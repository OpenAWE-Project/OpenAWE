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

#ifndef OPENAWE_CONVERTEDPROGRAM_H
#define OPENAWE_CONVERTEDPROGRAM_H

#include "src/graphics/opengl/program.h"

namespace Graphics::OpenGL {

class ConvertedProgram : public Program {
public:
	void setAttributeMappings(const std::map<AttributeType, std::string> &mappings);
	void addSamplerMappings(const std::map<std::string, std::string> &mappings);

	void setSymbols(const std::vector<ShaderConverter::Symbol> &symbols);

	std::optional<GLint> getAttributeLocation(const AttributeType &type) override;

	std::optional<GLint> getUniformLocation(const std::string &name) const override;

	void setUniform1f(GLint id, const glm::vec1 &value) const override;

	void setUniform2f(GLint id, const glm::vec2 &value) const override;

	void setUniform3f(GLint id, const glm::vec3 &value) const override;

	void setUniformMatrix4f(GLint id, const glm::mat4 &value) const override;

	void setUniformMatrix4x3fArray(GLint id, const std::vector<glm::mat4x3> &values) const override;

private:
	std::optional<GLint> getUniformArraySymbolLocation(const ShaderConverter::Symbol &symbol, unsigned int offset = 0) const;

	std::map<std::string, std::string> _samplerMappings;
	std::map<AttributeType, std::string> _attributeMappings;
	std::map<std::string, ShaderConverter::Symbol> _symbols;
};

} // End of namespace Graphics::OpenGL

#endif //OPENAWE_CONVERTEDPROGRAM_H
