//
// Created by Patrick Dworski on 09.05.26.
//

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
