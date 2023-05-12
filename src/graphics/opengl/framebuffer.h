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

#ifndef AWE_FRAMEBUFFER_H
#define AWE_FRAMEBUFFER_H

#include "src/graphics/opengl/texture.h"
#include "src/graphics/opengl/opengl.h"

namespace Graphics::OpenGL {

class Framebuffer {
public:
	Framebuffer(const std::string &label = "");
	~Framebuffer();

	void attachTexture(const Texture &texture, GLenum attachmentType);
	void attachRenderBuffer(GLsizei width, GLsizei height, GLenum format, GLenum attachmentType);

	void bind();
	void bindRead();

private:
	std::vector<GLuint> _renderbuffers;
	std::vector<GLenum> _attachments;
	GLuint _id;
};

} // End of namespace Graphics::OpenGL

#endif //AWE_FRAMEBUFFER_H
