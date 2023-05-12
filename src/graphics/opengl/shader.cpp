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

#include <stdexcept>

#include <fmt/format.h>

#include "src/common/exception.h"

#include "src/graphics/opengl/shader.h"

namespace Graphics::OpenGL {

ShaderPtr Shader::fromSPIRV(GLuint type, const std::vector<byte> &bytecode, const std::string &label) {
	ShaderPtr shader(new Shader(type, label));

	if (!GLEW_ARB_gl_spirv || !GLEW_ARB_spirv_extensions)
		throw CreateException("Cannot create spirv shader without spirv extension");

	glShaderBinary(
			1,
			&shader->_id,
			GL_SPIR_V_BINARY_ARB,
			bytecode.data(),
			bytecode.size()
	);

	glSpecializeShader(shader->_id, "main", 0, nullptr, nullptr);

	GLint status;
	glGetShaderiv(shader->_id, GL_COMPILE_STATUS, &status);
	if (!status)
		throw CreateException("Failed to compile spirv shader with {}", status);

	return shader;
}

ShaderPtr Shader::fromGLSL(GLuint type, const std::string &source, const std::string &label) {
	ShaderPtr shader(new Shader(type, label));

	const char *shaderSource = source.c_str();
	glShaderSource(
			shader->_id,
			1,
			&shaderSource,
			nullptr
	);

	glCompileShader(shader->_id);

	GLint result, infoLogLength;

	glGetShaderiv(shader->_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader->_id, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (result != GL_TRUE) {
		std::string log;
		log.resize(infoLogLength);

		glGetShaderInfoLog(shader->_id, infoLogLength, nullptr, log.data());

		throw std::runtime_error(fmt::format("Error while compiling shader:\n{}", log));
	}

	return shader;
}

Shader::Shader(GLuint type, const std::string &label) : _id(glCreateShader(type)) {
	if (GLEW_KHR_debug && !label.empty())
		glObjectLabel(GL_SHADER, _id, label.size(), label.c_str());
}

Shader::~Shader() {
	glDeleteShader(_id);
}

}
