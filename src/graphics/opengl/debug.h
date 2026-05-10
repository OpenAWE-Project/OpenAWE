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
