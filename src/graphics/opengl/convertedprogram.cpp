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
#include <glm/gtc/type_ptr.hpp>

#include "convertedprogram.h"

namespace Graphics::OpenGL {

std::optional<GLint> ConvertedProgram::getAttributeLocation(const AttributeType &type) {
	const auto attributeIndexMapped = Program::getAttributeLocation(_attributeMappings[type]);
	if (attributeIndexMapped)
		return *attributeIndexMapped;

	return {};
}

std::optional<GLint> ConvertedProgram::getUniformLocation(const std::string &name) const {
	const auto symbolIter = _symbols.find(name);
	if (symbolIter != _symbols.end())
		return getUniformArraySymbolLocation(symbolIter->second);

	const auto samplerIter = _samplerMappings.find(name);
	if (samplerIter != _samplerMappings.end())
		return getUniformLocation(samplerIter->second);

	return Program::getUniformLocation(name);
}

void ConvertedProgram::addSamplerMappings(const std::map<std::string, std::string> &mappings) {
	_samplerMappings.insert(mappings.begin(), mappings.end());
}

void ConvertedProgram::setAttributeMappings(const std::map<AttributeType, std::string> &mappings) {
	_attributeMappings = mappings;
}

void ConvertedProgram::setSymbols(const std::vector<ShaderConverter::Symbol> &symbols) {
	for (const auto &symbol : symbols) {
		_symbols[symbol.name] = symbol;

		const auto location = getUniformLocation(symbol.name);

		if (!location)
			continue;

		_symbolLength[*location] = symbol.length;
	}
}

std::optional<GLint> ConvertedProgram::getUniformArraySymbolLocation(const ShaderConverter::Symbol &symbol, unsigned int offset) const {
	const std::string uniformArrayName =
			(symbol.shaderType == ShaderConverter::kVertex) ? "vs_uniforms_vec4" : "ps_uniforms_vec4";
	const std::string uniformArrayElementName = fmt::format("{}[{}]", uniformArrayName, symbol.index + offset);
	const auto uniformArrayLocation = getUniformLocation(uniformArrayElementName);
	if (!uniformArrayLocation)
		return {};

	return uniformArrayLocation;
}

void ConvertedProgram::setUniform1f(GLint id, const glm::vec1 &value) const {
	glm::vec4 alignedValue(value, 0.0f, 0.0f, 0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniform2f(GLint id, const glm::vec2 &value) const {
	glm::vec4 alignedValue(value, 0.0f, 0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniform3f(GLint id, const glm::vec3 &value) const {
	glm::vec4 alignedValue(value,  0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniformMatrix4f(GLint id, const glm::mat4 &value) const {
	if (id < 0)
		return;

	const glm::mat4 transposedValue = glm::transpose(value);
	const unsigned int length = _symbolLength.at(id);

	for (int i = 0; i < std::min(length, 4u); ++i) {
		Program::setUniform4f(id + i, transposedValue[i]);
	}
}

void ConvertedProgram::setUniformMatrix4x3fArray(GLint id, const std::vector<glm::mat4x3> &values) const {
	for (const auto &value: values) {
		glm::mat4 transposedValue = glm::transpose(value);

		setUniform4f(id, transposedValue[0]);
		setUniform4f(id + 1, transposedValue[1]);
		setUniform4f(id + 2, transposedValue[2]);

		id += 3;
	}
}

}
