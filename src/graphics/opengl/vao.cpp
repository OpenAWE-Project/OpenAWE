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

#include "src/common/exception.h"

#include "src/graphics/opengl/program.h"
#include "src/graphics/opengl/vao.h"
#include "src/graphics/opengl/task.h"
#include "vbo.h"

namespace Graphics::OpenGL {

struct VAOAttribute {
	bool integer = false;
	GLuint size = 0, localOffset = 0;
	GLenum type = 0;
	AttributeType attributeType;
};

VAO::VAO(TaskQueue &queue, ProgramPtr program, const std::string &label) :
	_queue(queue),
	_program(program),
	_id(std::make_shared<GLuint>(0)) {
	_queue.emplace_back([=, id = _id] {
		glGenVertexArrays(1, id.get());

		if (GLAD_GL_KHR_debug && !label.empty())
			glObjectLabel(GL_VERTEX_ARRAY, *id, label.size(), label.c_str());
	});
}

VAO::~VAO() {
	_queue.emplace_back([=, id = _id] {
		glDeleteVertexArrays(1, id.get());
	});
}

void VAO::bind() {
	glBindVertexArray(*_id);
}

void
VAO::addAttributes(const std::vector<VertexAttribute> &vertexAttributes, BufferPtr vertexData, unsigned int offset,
				   bool perInstance) {
	// Calculate the size of the vertex element.
	GLuint stride = 0;
	for (const auto &attribute : vertexAttributes) {
		switch (attribute.dataType) {
			case kUByte:
				stride += 1;
				break;
			case kVec4BF:
			case kVec4BI:
			case kVec2S:
				stride += 4;
				break;
			case kVec4S:
			case kVec2F:
				stride += 8;
				break;
			case kVec3F:
				stride += 12;
				break;
			case kVec4F:
				stride += 16;
				break;
			default:
				throw CreateException("Invalid vertex attribute type");
		}
	}

	GLuint localOffset = 0;
	std::vector<VAOAttribute> attributes;
	for (auto vertexAttribute : vertexAttributes) {
		bool integer = false;
		GLuint size = 0, totalSize = 0;
		GLenum type = 0;

		switch (vertexAttribute.dataType) {
			case kVec2F:
				type = GL_FLOAT;
				size = 2;
				totalSize = 8;
				break;
			case kVec3F:
				type = GL_FLOAT;
				size = 3;
				totalSize = 12;
				break;
			case kVec4F:
				type = GL_FLOAT;
				size = 4;
				totalSize = 16;
				break;
			case kVec2S:
				type = GL_SHORT;
				size = 2;
				totalSize = 4;
				break;
			case kVec4S:
				type = GL_SHORT;
				size = 4;
				totalSize = 8;
				break;
			case kVec4BI:
				integer = true;
				[[fallthrough]];
			case kVec4BF:
				type = GL_UNSIGNED_BYTE;
				size = 4;
				totalSize = 4;
				break;
			case kUByte:
				type = GL_UNSIGNED_BYTE;
				size = 1;
				totalSize = 1;
				integer = true;
				break;
			default:
				throw CreateException("Invalid vertex attribute type");
		}

		attributes.emplace_back(VAOAttribute{integer, size, localOffset, type, vertexAttribute.component});

		localOffset += totalSize;
	}

	_queue.emplace_back([=, id = _id]() {
		glBindVertexArray(*id);
		std::static_pointer_cast<Graphics::OpenGL::VBO>(vertexData)->bind();

		for (const auto &attribute: attributes) {
			const auto index = _program->getAttributeLocation(attribute.attributeType);
			if (index) {
				glEnableVertexAttribArray(*index);

				if (perInstance)
					glVertexAttribDivisor(*index, 1);

				if (attribute.integer) {
					glVertexAttribIPointer(
							*index,
							attribute.size,
							attribute.type,
							stride,
							reinterpret_cast<const void *>(attribute.localOffset + offset)
					);
				} else {
					glVertexAttribPointer(
							*index,
							attribute.size,
							attribute.type,
							GL_FALSE,
							stride,
							reinterpret_cast<const void *>(attribute.localOffset + offset)
					);
				}
			}
		}
	});
}

}
