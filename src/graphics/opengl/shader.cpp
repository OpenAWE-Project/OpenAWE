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

#include "shader.h"

namespace Graphics::OpenGL {

Shader::Shader(GLuint type, const std::string& source) {
	_id = glCreateShader(type);

	char const *shaderSource = source.c_str();

	glShaderSource(
			_id,
			1,
			&shaderSource,
			nullptr
	);
}

Shader::~Shader() {
	glDeleteShader(_id);
}

void Shader::compile() const {
	glCompileShader(_id);

	GLint result, infoLogLength;

	glGetShaderiv(_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (result != GL_TRUE) {
		std::string log;
		log.resize(infoLogLength);

		glGetShaderInfoLog(_id, infoLogLength, nullptr, log.data());

		throw std::runtime_error(fmt::format("Error while compiling shader:\n{}", log));
	}
}

}
