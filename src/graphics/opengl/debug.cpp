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

#include "src/graphics/opengl/debug.h"

#include "src/graphics/opengl/opengl.h"

namespace Graphics::OpenGL {

#ifdef GL_KHR_debug
GLenum getKHRDebugType(ObjectType type) {
	switch (type) {
		using enum ObjectType;
		case kVertexArray: return GL_VERTEX_ARRAY;
		case kBufferObject: return GL_BUFFER;
		case kTexture: return GL_TEXTURE;
		case kShader: return GL_SHADER;
		case kProgram: return GL_PROGRAM;
		case kFramebuffer: return GL_FRAMEBUFFER;
		case kRenderbuffer: return GL_RENDERBUFFER;
	}
}
#endif

#ifdef GL_EXT_debug_label
GLenum getEXTDebugLabelType(ObjectType type) {
	switch (type) {
		using enum ObjectType;
		case kVertexArray: return GL_VERTEX_ARRAY_OBJECT_EXT;
		case kBufferObject: return GL_BUFFER_OBJECT_EXT;
		case kTexture: return GL_TEXTURE;
		case kShader: return GL_SHADER_OBJECT_EXT;
		case kProgram: return GL_PROGRAM_OBJECT_EXT;
		case kFramebuffer: return GL_FRAMEBUFFER;
		case kRenderbuffer: return GL_RENDERBUFFER;
	}
}
#endif

void labelObject(GLuint id, ObjectType type, const std::string& label) {
	if (label.empty())
		return;

#ifdef GL_KHR_debug
	if (GLAD_GL_KHR_debug)
		glObjectLabel(getKHRDebugType(type), id, label.size(), label.c_str());
#endif

#ifdef GL_EXT_debug_label
	glLabelObjectEXT(getEXTDebugLabelType(type), id, label.size(), label.c_str());
#endif
}

void pushDebugMarker(const std::string& label) {
#ifdef GL_KHR_debug
	if (GLAD_GL_KHR_debug)
		glPushDebugGroup(
			GL_DEBUG_SOURCE_APPLICATION,
			0,
			label.size(),
			label.c_str()
		);
#endif

#ifdef GL_EXT_debug_marker
	glPushGroupMarkerEXT(label.size(), label.c_str());
#endif
}
void popDebugMarker() {
#ifdef GL_KHR_debug
	if (GLAD_GL_KHR_debug)
		glPopDebugGroup();
#endif

#ifdef GL_EXT_debug_marker
	glPopGroupMarkerEXT();
#endif
}

}
