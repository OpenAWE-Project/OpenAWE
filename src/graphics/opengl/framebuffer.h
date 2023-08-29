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

class Renderbuffer;

/*!
 * \brief An implementation for handling opengl framebuffers
 *
 * This class encapsulates a framebuffer object from opengl allowing it to be bound and allows to attach textures and
 * renderbuffers to it. It also allows the framebuffer to be labeled to allow graphics debuggers to better identify this
 * framebuffer
 */
class Framebuffer {
public:
	/*!
	 * Initialize the framebuffer with an optional label
	 * \param label The label for the framebuffer
	 */
	Framebuffer(const std::string &label = "");
	~Framebuffer();

	/*!
	 * Attach a texture to this framebuffer
	 * \param texture The texture to attach
	 * \param attachmentType The attachment type using a GL_*_ATTACHMENT_* variable
	 */
	void attachTexture(const Texture &texture, GLenum attachmentType);

	/*!
	 * Attach a renderbuffer to this framebuffer
	 * \param renderbuffer The renderbuffer to attach
	 * \param attachmentType The attachment type using a GL_*_ATTACHMENT_* variable
	 */
	void attachRenderBuffer(const Renderbuffer &renderbuffer, GLenum attachmentType);

	/*!
	 * Set the color, the framebuffer should be cleared with
	 * \param clearColor The clear color for the buffer to be cleared
	 */
	void setClearColor(const glm::vec4 &clearColor);

	/*!
	 * Clear the framebuffers color attachments with a specified clear color
	 */
	void clear();

	/*!
	 * Bind the framebuffer as the current
	 */
	void bind();

	/*!
	 * Bind the framebuffer in read only mode as the current
	 */
	void bindRead();

private:
	std::vector<GLenum> _attachments;
	glm::vec4 _clearColor;
	GLuint _id;
};

/*!
 * \brief An implementation for handling opengl renderbuffers
 *
 * This class encapsulates a renderbuffer object from opengl. It allows the renderbuffer to be created with multiple
 * parameters and an optional label
 */
class Renderbuffer {
public:
	/*!
	 * Create the renderbuffer with size and its format and an optional label
	 * @param width The width of the renderbuffer to create
	 * @param height The height of the renderbuffer to create
	 * @param format The format of the renderbuffer to create
	 * @param label An optional label to attach for graphics debuggers
	 */
	Renderbuffer(GLsizei width, GLsizei height, GLenum format, const std::string &label = "");
	~Renderbuffer();

private:
	friend class Framebuffer;

	GLuint _id;
};

} // End of namespace Graphics::OpenGL

#endif //AWE_FRAMEBUFFER_H
