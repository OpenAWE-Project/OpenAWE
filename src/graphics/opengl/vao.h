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

#ifndef AWE_VAO_H
#define AWE_VAO_H

#include "src/common/types.h"

#include "src/graphics/attributeobject.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/task.h"

namespace Graphics::OpenGL {

class VAO : public AttributeObject {
public:
	VAO(TaskQueue &queue, const std::string &label = "");
	~VAO();

	void bind();

	void applyAttributes(ProgramPtr program, const std::vector<VertexAttribute> &vertexAttributes, BufferPtr vertexData,
						 unsigned int offset);

private:
	TaskQueue &_queue;
	std::shared_ptr<GLuint> _id;
};

} // End of namespace Graphics::OpenGL

#endif //AWE_VAO_H
