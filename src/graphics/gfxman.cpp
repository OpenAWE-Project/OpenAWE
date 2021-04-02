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

#include <fmt/format.h>

#include "src/awe/resman.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/opengl/renderer.h"

namespace Graphics {

void GraphicsManager::initOpenGL(Window &window) {
	if (_renderer)
		throw std::runtime_error("Renderer already initialized");

	_renderer = std::make_unique<Graphics::OpenGL::Renderer>(window);
}

void GraphicsManager::addModel(Model *model) {
	_renderer->addModel(model);
}

void GraphicsManager::removeModel(Model *model) {

}

void GraphicsManager::addGUIElement(GUIElement *gui) {
	_renderer->addGUIElement(gui);
}

Common::UUID GraphicsManager::registerVertices(byte *data, size_t length, Common::UUID vertexLayout) {
	return _renderer->registerVertices(data, length);
}

Common::UUID GraphicsManager::registerIndices(byte *data, size_t length) {
	return _renderer->registerIndices(data, length);
}

Common::UUID GraphicsManager::registerTexture(const ImageDecoder &decoder) {
	return _renderer->registerTexture(decoder);
}

std::future<Common::UUID> GraphicsManager::registerTextureAsync(const ImageDecoder &decoder) {
	const std::lock_guard l(_textureAccess);

	_texturesToProcess.emplace_back(decoder);

	return _texturesToProcess.back().promise.get_future();
}

void GraphicsManager::deregisterTexture(Common::UUID &id) {
	_renderer->deregisterTexture(id);
}

void GraphicsManager::deregisterTextureAsync(Common::UUID &id) {
	const std::lock_guard l(_textureAccess);
	_texturesToDelete.emplace_back(id);
}

Common::UUID GraphicsManager::registerVertexAttributes(const std::string &shader,
															 const std::vector<VertexAttribute> &vertexAttributes,
															 Common::UUID vertexData) {
	return _renderer->registerVertexAttributes(shader, vertexAttributes, vertexData);
}

void GraphicsManager::setCurrentVideoFrame(Common::UUID &id) {
	_renderer->setCurrentVideoFrame(id);
}

void GraphicsManager::drawFrame() {
	_textureAccess.lock();

	for (auto &textureToDelete : _texturesToDelete) {
		deregisterTexture(textureToDelete);
	}
	_texturesToDelete.clear();

	for (auto &texture : _texturesToProcess) {
		Common::UUID id = registerTexture(texture.decoder);
		texture.promise.set_value(id);
	}
	_texturesToProcess.clear();

	_textureAccess.unlock();

	_renderer->drawFrame();
}

void GraphicsManager::setAmbianceState(const std::string &id) {
	std::unique_ptr<Common::ReadStream> ambianceFile(
			ResMan.getResource(fmt::format("ambiance_presets/{}.xml", id))
	);

	_renderer->setAmbianceState(AmbianceState(*ambianceFile));
}

Camera GraphicsManager::getCamera() const {
	return _camera;
}

void GraphicsManager::setCamera(const Camera &camera) {
	_camera = camera;
	_renderer->setCamera(_camera);
}

}
