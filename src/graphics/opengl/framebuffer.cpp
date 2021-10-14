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

#include "framebuffer.h"

namespace Graphics::OpenGL {

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &_id);
}

Framebuffer::~Framebuffer() {
	glDeleteRenderbuffers(_renderbuffers.size(), _renderbuffers.data());
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
	_attachments.emplace_back(attachmentType);
}

void Framebuffer::attachRenderBuffer(GLsizei width, GLsizei height, GLenum format, GLenum attachmentType) {
	GLuint rbo;
	glCreateRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, rbo);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glDrawBuffers(_attachments.size(), _attachments.data());
}

void Framebuffer::bindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

}
