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

class BufferTask : public Task {
public:
	BufferTask(GLuint &id, GLuint type) : _id(id), _type(type) {}

	void apply() override {
		glBindBuffer(_type, _id);
	}

protected:
	GLuint &_id;
	const GLuint _type;
};

class BufferCreationTask : public BufferTask {
public:
	BufferCreationTask(GLuint &id, GLuint type) : BufferTask(id, type) {}

	void apply() override {
		glGenBuffers(1, &_id);
	}
};

class BufferDeletionTask : public BufferTask {
public:
	BufferDeletionTask(GLuint &id, GLuint type) : BufferTask(id, type) {}

	void apply() override {
		glDeleteBuffers(1, &_id);
	}
};

class BufferDataTask : public BufferTask {
public:
	BufferDataTask(
			GLuint &id,
			GLuint type,
			GLuint usage,
			Common::ByteBuffer &&data
	)
	: BufferTask(id, type)
	, _usage(usage)
	, _data(std::move(data)), _s(_data.size()) {}

	void apply() override {
		BufferTask::apply();

		assert(_data.size() == _s);
		glBufferData(_type, _data.size(), _data.data(), _usage);
		assert(GL_NO_ERROR == glGetError());
	}

private:
	const GLuint _usage;
	const Common::ByteBuffer _data;
	const size_t _s;
};


VBO::VBO(TaskQueue &tasks, GLenum type, GLenum usage) : _tasks(tasks), _id(0), _type(type), _usage(usage) {
	_tasks.emplace_back(std::make_unique<BufferCreationTask>(_id, _type));
}

VBO::~VBO() {
	_tasks.emplace_back(std::make_unique<BufferDeletionTask>(_id, _type));
}

void VBO::bufferData(byte *data, size_t length) {
	bind();
	glBufferData(_type, length, data, _usage);
}

void VBO::bind() const {
	glBindBuffer(_type, _id);
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
	bind();
	_tasks.emplace_back(std::make_unique<BufferDataTask>(_id, _type, _usage, std::move(data)));
}

}