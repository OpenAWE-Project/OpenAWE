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
#include <optional>

#include "src/common/writestream.h"

#include "src/graphics/shaderconverter.h"
#include "src/graphics/types.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/shader.h"

namespace Graphics::OpenGL {

class Program;

typedef std::shared_ptr<Program> ProgramPtr;

class Program : Common::Noncopyable {
public:
	Program(const std::string &label = "");
	~Program();

	void attach(const Shader &shader) const;
	void validate() const;
	void link();

	void bind() const;

	virtual std::optional<GLint> getAttributeLocation(const AttributeType &type);

	// ,--- Uniform access
	virtual std::optional<GLint> getUniformLocation(const std::string &name) const;

	virtual void setUniform1f(GLint id, const glm::vec1 &value) const;
	virtual void setUniform2f(GLint id, const glm::vec2 &value) const;
	virtual void setUniform3f(GLint id, const glm::vec3 &value) const;
	void setUniform4f(GLint id, const glm::vec4 &value) const;
	virtual void setUniformMatrix4f(GLint id, const glm::mat4 &value) const;
	virtual void setUniformMatrix4x3fArray(GLint id, const std::vector<glm::mat4x3> &values) const;
	void setUniformSampler(GLint id, const GLuint value) const;
	// '---

protected:
	std::optional<GLint> getAttributeLocation(const std::string &name) const;

private:
	std::map<std::string, GLuint> _attributes;
	std::map<std::string, GLuint> _uniforms;

	const GLuint _id;
};

}

#endif //AWE_PROGRAM_H
