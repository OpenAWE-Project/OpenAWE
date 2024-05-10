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

#ifndef AWE_VBO_H
#define AWE_VBO_H

#include <src/common/types.h>

#include "src/graphics/buffer.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/task.h"

namespace Graphics::OpenGL {

/*!
 * \brief Vertex Buffer Object
 *
 * A Vertex Buffer Object for storing vertex and
 * index data for later access for rendering.
 */
class VBO : public Buffer {
public:
	VBO(TaskQueue &tasks, GLenum type, GLenum usage);
	~VBO();

	void bufferData(byte *data, size_t length);

	void bind() const;

	void *map() const;
	void unmap() const;

	unsigned int getBufferSize() const;

	void read(byte *data, size_t length) override;
	void write(Common::ByteBuffer &&data) override;

private:
	TaskQueue &_tasks;
	GLuint _id;
	GLenum _type;
	GLenum _usage;
};

} // End of namespace Graphics::OpenGL

#endif //AWE_VBO_H
