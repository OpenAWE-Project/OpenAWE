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

#include <glm/gtc/type_ptr.hpp>

#include "src/common/exception.h"

#include "src/graphics/opengl/framebuffer.h"

namespace Graphics::OpenGL {

Framebuffer::Framebuffer(const std::string &label) {
	glGenFramebuffers(1, &_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw CreateException("Failed to initialize framebuffer");

	bind();
	if (GLAD_GL_KHR_debug && !label.empty())
		glObjectLabel(GL_FRAMEBUFFER, _id, label.size(), label.c_str());
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &_id);
}

void Framebuffer::attachTexture(const Texture &texture, GLenum attachmentType) {
	glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			attachmentType,
			texture._type,
			texture._id,
			0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw CreateException("Failed to attach framebuffer to texture");

	_attachments.emplace_back(attachmentType);
}

void Framebuffer::attachRenderBuffer(const Renderbuffer &renderbuffer, GLenum attachmentType) {
	glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			attachmentType,
			GL_RENDERBUFFER,
			renderbuffer._id
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw CreateException("Failed to attach renderbuffer tot exture");
}

void Framebuffer::clear() {
	for (size_t i = 0; i < _attachments.size(); ++i) {
		glClearBufferfv(GL_COLOR, i, glm::value_ptr(_clearColor));
	}
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glDrawBuffers(_attachments.size(), _attachments.data());
}

void Framebuffer::bindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void Framebuffer::setClearColor(const glm::vec4 &clearColor) {
	_clearColor = clearColor;
}

Renderbuffer::Renderbuffer(GLsizei width, GLsizei height, GLenum format, const std::string &label) {
	glGenRenderbuffers(1, &_id);
	glBindRenderbuffer(GL_RENDERBUFFER, _id);

	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);

	if (GLAD_GL_KHR_debug && !label.empty())
		glObjectLabel(GL_RENDERBUFFER, _id, label.size(), label.c_str());
}

Renderbuffer::~Renderbuffer() {
	glDeleteRenderbuffers(1, &_id);
}

}
