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

#include <memory>
#include <utility>

#include <fmt/format.h>

#include "src/awe/resman.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/opengl/renderer.h"

namespace Graphics {

void GraphicsManager::initOpenGL(Platform::Window &window) {
	if (_renderer)
		throw std::runtime_error("Renderer already initialized");

	_renderer = std::make_unique<Graphics::OpenGL::Renderer>(window);
}

void GraphicsManager::addModel(Model *model) {
	_renderer->addModel(model);
}

void GraphicsManager::removeModel(Model *model) {
	_renderer->removeModel(model);
}

void GraphicsManager::addGUIElement(GUIElement *gui) {
	_renderer->addGUIElement(gui);
}

void GraphicsManager::removeGUIElement(GUIElement *gui) {
	_renderer->removeGUIElement(gui);
}

void GraphicsManager::addLight(Light *light) {
	_renderer->addLight(light);
}

TexturePtr GraphicsManager::createTexture(const ImageDecoder &decoder) {
	TexturePtr texture = _renderer->createTexture(decoder.getType());
	texture->load(decoder);
	return texture;
}

ProxyTexturePtr GraphicsManager::createProxyTexture() {
	return _renderer->createProxyTexture();
}

TexturePtr GraphicsManager::createEmptyTexture2D(TextureFormat format, unsigned int width, unsigned int height) {
	TexturePtr texture = _renderer->createTexture(kTexture2D);
	texture->allocate(format, width, height);
	return texture;
}

BufferPtr GraphicsManager::createBuffer(const byte *data, size_t length, BufferType type, bool modifiable) {
	BufferPtr buffer = _renderer->createBuffer(type, modifiable);
	buffer->write(data, length);
	return buffer;
}

BufferPtr GraphicsManager::createEmptyBuffer(BufferType type, bool modifiable) {
	BufferPtr buffer = _renderer->createBuffer(type, modifiable);
	return buffer;
}

AttributeObjectPtr
GraphicsManager::createAttributeObject(const std::string &shader, const std::string &stage,
									   const std::vector<VertexAttribute> &vertexAttributes, BufferPtr vertexData,
									   unsigned int offset) {
	return _renderer->createAttributeObject(
		shader,
		stage,
		vertexAttributes,
		std::move(vertexData),
		offset
	);
}

int GraphicsManager::getUniformIndex(const std::string &shaderName, const std::string &stage, const std::string &id) {
	return _renderer->getUniformIndex(shaderName, stage, id);
}

void GraphicsManager::drawFrame() {
	_renderer->update();
	_renderer->drawFrame();
}

void GraphicsManager::setAmbianceState(const std::string &id) {
	std::unique_ptr<Common::ReadStream> ambianceFile(
			ResMan.getResource(fmt::format("ambiance_presets/{}.xml", id))
	);

	_renderer->setAmbianceState(AmbianceState(*ambianceFile));
}

void GraphicsManager::setCamera(Camera &camera) {
	_renderer->setCamera(camera);
}

}
