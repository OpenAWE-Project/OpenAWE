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

#ifndef AWE_SHADER_H
#define AWE_SHADER_H

#include <string>
#include <vector>

#include "src/common/types.h"

#include "src/graphics/opengl/opengl.h"

namespace Graphics::OpenGL {

class Shader;

typedef std::shared_ptr<Shader> ShaderPtr;

class Shader : Common::Noncopyable {
public:
	/*!
	 * Create a new shader from a spirv binary
	 * \param type The type of the shader created. Should be given as opengl constant
	 * \param bytecode A byte vector containing the valid bytecode of the shader
	 * \param label An optional label with which this shader should be labelled. Useful for graphics debuggers
	 * \return A shared pointer to the new shader object
	 */
	static ShaderPtr fromSPIRV(
		GLuint type,
		const std::vector<byte> &bytecode,
		const std::string &label = ""
	);

	/*!
	 * Create a new shader from a glsl text source
	 * \param type The type of the shader created. Should be given as opengl constant
	 * \param source The glsl source of the shader to be created
	 * \param label An optional label with which this shader should be labelled. Useful for graphics debuggers
	 * \return A shared pointer to the new shader object
	 */
	static ShaderPtr fromGLSL(
		GLuint type,
		const std::string &source,
		const std::string &label = ""
	);

	~Shader();

private:
	Shader(GLuint type, const std::string &label);

	friend class Program;

	const GLuint _id;
};

} // End of namespace Graphics::OpenGL

#endif //AWE_SHADER_H
