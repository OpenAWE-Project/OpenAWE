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
#include <assert.h>

#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include <vector>

#include "src/common/strutil.h"

#include "src/graphics/opengl/program.h"

namespace Graphics::OpenGL {

Program::Program(const std::string &label) : _id(glCreateProgram()) {
	if (GLEW_KHR_debug && !label.empty())
		glObjectLabel(GL_PROGRAM, _id, label.size(), label.c_str());
}

Program::~Program() {
	glDeleteProgram(_id);
}

void Program::attach(const Shader &shader) const {
	glAttachShader(_id, shader._id);
}

void Program::validate() const {
	glValidateProgram(_id);

	GLint result, infoLogLength;

	glGetProgramiv(_id, GL_VALIDATE_STATUS, &result);
	glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (result != GL_TRUE) {
		std::string log;
		log.resize(infoLogLength);

		glGetProgramInfoLog(_id, infoLogLength, nullptr, log.data());

		throw std::runtime_error(log);
	}
}

void Program::link() {
	glLinkProgram(_id);

	GLint result, infoLogLength;

	glGetProgramiv(_id, GL_LINK_STATUS, &result);
	glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (result != GL_TRUE) {
		std::string log;
		log.resize(infoLogLength);

		glGetProgramInfoLog(_id, infoLogLength, nullptr, log.data());

		throw std::runtime_error(log);
	}

	// Determine Attribute locations
	GLint numAttributes;
	GLint maxAttributeNameLength;
	glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
	glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTES, &numAttributes);
	GLchar *name = new GLchar[maxAttributeNameLength];
	for (int i = 0; i < numAttributes; ++i) {
		GLsizei actualLength;
		GLenum type;
		GLint size;
		glGetActiveAttrib(_id, i, maxAttributeNameLength, &actualLength, &size, &type, name);
		std::string attributeName(name, actualLength);
		_attributes[attributeName] = glGetAttribLocation(_id, name);
	}
	delete [] name;

	// Determine Uniform locations
	GLint numUniforms;
	GLint maxUniformNameLength;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
	glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &numUniforms);
	name = new GLchar[maxUniformNameLength];
	for (int i = 0; i < numUniforms; ++i) {
		GLsizei actualLength;
		GLenum type;
		GLint size;
		glGetActiveUniform(_id, i, maxUniformNameLength, &actualLength, &size, &type, name);
		const std::string uniformName(name, actualLength);
		_uniforms[uniformName] = glGetUniformLocation(_id, name);

		if (size > 1) {
			const std::string arrayName = Common::replace(uniformName, "[0]", "");
			_uniforms[arrayName] = _uniforms[uniformName];

			for (int j = 1; j < size; ++j) {
				const std::string itemName = Common::replace(uniformName, "[0]", fmt::format("[{}]", j));
				const auto itemLocation = glGetUniformLocation(_id, itemName.c_str());
				if (itemLocation >= 0)
					_uniforms[itemName] = itemLocation;
			}
		}
	}
	delete [] name;
}

void Program::bind() const {
	glUseProgram(_id);
}

std::optional<GLint> Program::getAttributeLocation(const AttributeType &type) {
	std::string attributeName;
	switch (type) {
		case kPosition:
			attributeName = "in_Position";
			break;
		case kNormal:
			attributeName = "in_Normal";
			break;
		case kColor0:
			attributeName = "in_Color0";
			break;
		case kColor1:
			attributeName = "in_Color1";
			break;
		case kColor2:
			attributeName = "in_Color2";
			break;
		case kColor3:
			attributeName = "in_Color3";
			break;
		case kBoneWeight:
			attributeName = "in_BoneWeight";
			break;
		case kBoneIndex:
			attributeName = "in_BoneID";
			break;
		case kTangent:
			attributeName = "in_Tangent";
			break;
		case kDisplacementFactor:
			attributeName = "in_DisplacementFactor";
			break;
		case kTangent0:
			attributeName = "in_Tangent0";
			break;
		case kTangent1:
			attributeName = "in_Tangent1";
			break;
		case kTangent2:
			attributeName = "in_Tangent2";
			break;
		case kTangent3:
			attributeName = "in_Tangent3";
			break;
		case kTexCoord0:
			attributeName = "in_UV0";
			break;
		case kTexCoord1:
			attributeName = "in_UV1";
			break;
		case kTexCoord2:
			attributeName = "in_UV2";
			break;
		case kTexCoord3:
			attributeName = "in_UV3";
			break;
		default:
			throw std::runtime_error("Unknown attribute");
	}

	const auto attributeIndex = getAttributeLocation(attributeName);
	if (attributeIndex)
		return *attributeIndex;

	return std::optional<GLint>();
}

void Program::setUniform1f(GLint id, float value) const {
	glUniform1f(id, value);
}

void Program::setUniform2f(GLint id, const glm::vec2 &value) const {
	glUniform2fv(id, 1, glm::value_ptr(value));
}

void Program::setUniform3f(GLint id, const glm::vec3 &value) const {
	glUniform3fv(id, 1, glm::value_ptr(value));
}

void Program::setUniform4f(GLint id, const glm::vec4 &value) const {
	glUniform4fv(id, 1, glm::value_ptr(value));
}

void Program::setUniform1fArray(GLint id, const std::vector<float> &values) const {
	glUniform1fv(id, values.size(), values.data());
}

void Program::setUniform2fArray(GLint id, const std::vector<glm::vec2> &values) const {
	glUniform2fv(id, values.size(), reinterpret_cast<const GLfloat *>(values.data()));
}

void Program::setUniform3fArray(GLint id, const std::vector<glm::vec3> &values) const {
	glUniform3fv(id, values.size(), reinterpret_cast<const GLfloat *>(values.data()));
}

void Program::setUniform4fArray(GLint id, const std::vector<glm::vec4> &values) const {
	glUniform4fv(id, values.size(), reinterpret_cast<const GLfloat *>(values.data()));
}

void Program::setUniformMatrix4f(GLint id, const glm::mat4 &value) const {
	glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::setUniformMatrix4x3fArray(GLint id, const std::vector<glm::mat4x3> &values) const {
	glUniformMatrix4x3fv(id, values.size(), GL_FALSE, reinterpret_cast<const GLfloat *>(values.data()));
}

void Program::setUniformSampler(GLint id, const GLuint value) const {
	glUniform1i(id, value);
}

std::optional<GLint> Program::getUniformLocation(const std::string &name) const {
	// First try to find the uniform in the cached uniforms
	const auto iter = _uniforms.find(name);
	if (iter != _uniforms.end())
		return iter->second;
	else
		return {};
}

std::optional<GLint> Program::getAttributeLocation(const std::string &name) const {
	auto iter = _attributes.find(name);
	if (iter == _attributes.end())
		return std::optional<GLint>();
	return iter->second;
}

}
