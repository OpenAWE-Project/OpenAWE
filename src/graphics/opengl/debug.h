//
// Created by Patrick Dworski on 09.05.26.
//

#ifndef OPENAWE_DEBUG_H
#define OPENAWE_DEBUG_H

#include <string>

#include "src/graphics/opengl/opengl.h"

namespace Graphics::OpenGL {
enum class ObjectType {
	kVertexArray,
	kBufferObject,
	kTexture,
	kShader,
	kProgram,
	kFramebuffer,
	kRenderbuffer
};

/**
 * Attach a string label to an arbitrary opengl object by providing its id, type and actual label. This will only work
 * if the platform supports labeling OpenGL objects, if not it will return regardless.
 *
 * @param id The id of the opengl object to label
 * @param type The type of the opengle object to label
 * @param label The actual label to attach to the objec
 */
void labelObject(GLuint id, ObjectType type, const std::string& label);

/**
 * Push a new label for which the following opengl functions will be grouped together until it is popped
 *
 * @param label The label to mark the group
 */
void pushDebugMarker(const std::string& label);

/**
 * Pop the current debug marker
 */
void popDebugMarker();
}

#endif //OPENAWE_DEBUG_H
