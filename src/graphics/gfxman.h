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

#ifndef AWE_GFXMAN_H
#define AWE_GFXMAN_H

#include <memory>
#include <queue>
#include <future>

#include "src/common/singleton.h"

#include "src/platform/window.h"

#include "src/graphics/renderer.h"
#include "src/platform/window.h"
#include "src/graphics/vertexattribute.h"
#include "src/graphics/camera.h"
#include "src/graphics/buffer.h"

namespace Graphics {

class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	void initOpenGL(Platform::Window &window);

	void setCamera(Camera &camera);

	void setAmbianceState(const std::string &id);

	void addModel(Model *model);
	void removeModel(Model *model);

	void addGUIElement(GUIElement *gui);

	/*!
	 * Removes gui element by pointer from the scene
	 * \param gui The gui element to remove
	 */
	void removeGUIElement(GUIElement *gui);

	/*!
	 * Create a texture in the initialized rendering system using given image data from a decoder as base. The decoder
	 * also defines metadata, like how the dimensions of the texture are and the format of the texture.
	 * \param decoder The decoder defining the image data
	 * \return A render system specific texture object
	 */
	TexturePtr createTexture(const ImageDecoder &decoder);

	/*!
	 * Create an empty proxy texture for assigning other textures to it. This is useful for video playback to switch
	 * fast between single textures
	 * \return A newly created empty proxy texture
	 */
	ProxyTexturePtr createProxyTexture();

	/*!
	 * Create an empty 2d texture with the specified texture format and the specified size in width and height
	 * \param format The texture format, the texture should have
	 * \param width The width of the newly created texture
	 * \param height The height of the newly created texture
	 * \return A pointer to the newly created empty texture
	 */
	TexturePtr createEmptyTexture2D(TextureFormat format, unsigned int width, unsigned int height);

	/*!
	 * Create a buffer in the intialized render system using the given data block and the buffer type.
	 * \param data A pointer to the data which should be contained in the block
	 * \param length The length of the data of the data block
	 * \param type The type of the buffer
	 * \param modifiable If the buffer should be modifiable
	 * \return A render system specific buffer object
	 */
	BufferPtr createBuffer(const byte* data, size_t length, BufferType type, bool modifiable = false);

	/*!
	 * Create an empty buffer with the size of zero and no contents contained. It is assumed, that this buffer is used
	 * for frequently changing content, and therefore it is hinted, that it is modifiable by default
	 * \param type The type of the buffer
	 * \param modifiable If the buffer is frequently changed
	 * \return The newly created empty buffer
	 */
	BufferPtr createEmptyBuffer(BufferType type, bool modifiable = true);

	/*!
	 * Create an attribute object in the initialized render system using the given shader, vertex attributes and the
	 * vertex data buffer
	 * \param shader The shader for the attribute object
	 * \param stage The stage of the shader for the attribute object
	 * \param vertexAttributes The attributes to create the attribute object
	 * \param vertexData The vertex data to bind the attribute object to
	 * \param offset The global offset inside the vertex data from which to start
	 * \return A render system specific attribute object
	 */
	AttributeObjectPtr createAttributeObject(
		const std::string &shader,
		const std::string &stage,
		const std::vector<VertexAttribute> &vertexAttributes,
		BufferPtr vertexData,
		unsigned int offset = 0
	);

	/*!
	 * Get the index of a specific uniform from a specific shader
	 * \param shaderName The name of the shader from which to get the uniform variable
	 * \param stage The stage of the uniform index to get
	 * \param id The id of the uniform variable in the given shader
	 * \return The index of the uniform variable or -1 if the uniform variable is not found or not yet implemented
	 */
	int getUniformIndex(
		const std::string &shaderName,
		const std::string &stage,
		const std::string &id
	);

	void drawFrame();

private:
	std::unique_ptr<Renderer> _renderer;
};

} // End of namespace Graphics

#define GfxMan Graphics::GraphicsManager::instance()

#endif //AWE_GFXMAN_H
