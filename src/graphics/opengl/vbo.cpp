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

#include <cstring>

#include <iostream>
#include <assert.h>

#include <spdlog/spdlog.h>

#include "src/graphics/opengl/vbo.h"
#include "src/graphics/opengl/task.h"

namespace Graphics::OpenGL {

VBO::VBO(TaskQueue &tasks, GLenum type, GLenum usage) : _tasks(tasks), _id(std::make_shared<GLuint>(0)), _type(type), _usage(usage) {
	_tasks.emplace_back([=, id = _id] {
		glGenBuffers(1, id.get());
	});
}

VBO::~VBO() {
	_tasks.emplace_back([=, id = _id] {
		glDeleteBuffers(1, id.get());
	});
}

void VBO::bind() const {
	glBindBuffer(_type, *_id);
}

void *VBO::map() const {
	bind();
	return glMapBufferRange(_type, 0, getBufferSize(), GL_READ_WRITE);
}

void VBO::unmap() const {
	bind();
	glUnmapBuffer(_type);
}

unsigned int VBO::getBufferSize() const {
	bind();
	GLint bufferSize;
	glGetBufferParameteriv(_type, GL_BUFFER_SIZE, &bufferSize);
	return bufferSize;
}

void VBO::read(byte *data, size_t length) {
	bind();
	void *bufferData = glMapBufferRange(_type, 0, getBufferSize(), GL_READ_ONLY);
	std::memcpy(data, bufferData, std::min<size_t>(length, getBufferSize()));
	glUnmapBuffer(_type);
}

void VBO::write(Common::ByteBuffer &&data) {
	_tasks.emplace_back([=, type = _type, usage = _usage, id = _id]{
		glBindBuffer(type, *id);
		glBufferData(type, data.size(), data.data(), _usage);
	});
}

}