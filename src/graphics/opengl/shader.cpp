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

#if WITH_SPIRV_CROSS
#	include <spirv_cross/spirv_glsl.hpp>
#	include <spdlog/spdlog.h>
#endif // WITH_SPIRV_CROSS

#include "src/common/strutil.h"
#include "src/common/exception.h"

#include "src/graphics/opengl/shader.h"

namespace Graphics::OpenGL {

ShaderPtr Shader::fromSPIRV(GLuint type, const std::vector<byte> &bytecode, const std::string &label) {
	ShaderPtr shader(new Shader(type, label));

	if (!GLEW_ARB_gl_spirv || !GLEW_ARB_spirv_extensions) {
#if WITH_SPIRV_CROSS
		spirv_cross::CompilerGLSL glsl(
			reinterpret_cast<const uint32_t *>(bytecode.data()),
			bytecode.size() / 4
		);

		const std::string glslVersion(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
		const std::string versionOnly = Common::replace(
				Common::extract(
					glslVersion,
					std::regex("[1-4]\\.[0-6]\\.?0")
				),
				".", ""
		);

		spirv_cross::CompilerGLSL::Options options;
		options.es = false;
		options.version = std::stoul(versionOnly);

		glsl.set_common_options(options);
		const auto shaderCode = glsl.compile();

		return fromGLSL(type, shaderCode, label);
#else // WITH_SPIRV_CROSS
		throw CreateException("Cannot create spirv shader without spirv extension");
#endif // WITH_SPIRV_CROSS
	}

	glShaderBinary(
		1,
		&shader->_id,
		GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
		bytecode.data(),
		bytecode.size()
	);

	glSpecializeShaderARB(
		shader->_id,
		"main",
		0,
		nullptr,
		nullptr
	);

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
