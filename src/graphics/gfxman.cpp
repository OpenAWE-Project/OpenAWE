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

#include "src/common/exception.h"

#include "src/awe/atmfile.h"
#include "src/awe/resman.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/opengl/renderer.h"
#include "src/graphics/images/tex.h"

namespace Graphics {

void GraphicsManager::initOpenGL(Platform::Window &window, const std::string &shaderDirectory) {
	if (_renderer)
		throw std::runtime_error("Renderer already initialized");

	_renderer = std::make_unique<Graphics::OpenGL::Renderer>(window, shaderDirectory);
}

void GraphicsManager::releaseRenderer() {
	if (_renderer) {
		_renderer.reset();
	}
}

void GraphicsManager::addModel(Model *model) {
	if (!_renderer)
		return;

	_renderer->addModel(model);
}

void GraphicsManager::removeModel(Model *model) {
	if (!_renderer)
		return;

	_renderer->removeModel(model);
}

void GraphicsManager::addImGuiElement(ImGuiElement *imgui) {
	if (!_renderer)
		return;

	_renderer->addImGuiElement(imgui);
}

void GraphicsManager::removeImGuiElement(ImGuiElement *imgui) {
	if (!_renderer)
		return;

	_renderer->removeImGuiElement(imgui);
}

void GraphicsManager::addGUIElement(GUIElement *gui) {
	if (!_renderer)
		return;

	_renderer->addGUIElement(gui);
}

void GraphicsManager::removeGUIElement(GUIElement *gui) {
	if (!_renderer)
		return;

	_renderer->removeGUIElement(gui);
}

bool GraphicsManager::isLoading() const {
	if (!_renderer)
		return false;

	return _renderer->isLoading();
}

void GraphicsManager::setSky(SkyPtr sky) {
	if (!_renderer)
		return;

	_renderer->setSky(sky);
}

void GraphicsManager::addLight(Light *light) {
	if (!_renderer)
		return;

	_renderer->addLight(light);
}

void GraphicsManager::removeLight(Light *light) {
	if (!_renderer)
		return;

	_renderer->addLight(light);
}

TexturePtr GraphicsManager::createTexture(ImageDecoder &&decoder, const std::string &label) {
	if (!_renderer)
		return {};

	TexturePtr texture = _renderer->createTexture(decoder.getType(), label);
	texture->load(std::move(decoder));
	return texture;
}

ProxyTexturePtr GraphicsManager::createProxyTexture() {
	if (!_renderer)
		return {};

	return _renderer->createProxyTexture();
}

TexturePtr GraphicsManager::createEmptyTexture2D(TextureFormat format, unsigned int width, unsigned int height,
                                                 const std::string &label) {
	if (!_renderer)
		return {};

	TexturePtr texture = _renderer->createTexture(kTexture2D, label);
	texture->allocate(format, width, height);
	return texture;
}

BufferPtr GraphicsManager::createBuffer(Common::ByteBuffer &&data, BufferType type, bool modifiable) {
	if (!_renderer)
		return {};

	BufferPtr buffer = _renderer->createBuffer(type, modifiable);
	buffer->write(std::move(data));
	return buffer;
}

BufferPtr GraphicsManager::createEmptyBuffer(BufferType type, bool modifiable) {
	if (!_renderer)
		return {};

	BufferPtr buffer = _renderer->createBuffer(type, modifiable);
	return buffer;
}

AttributeObjectPtr
GraphicsManager::createAttributeObject(
	const std::string &shader,
	const std::string &stage,
	uint32_t properties,
	const std::vector<VertexAttribute> &vertexAttributes,
	BufferPtr vertexData,
	unsigned int offset,
	const std::string &label
) {
	if (!_renderer)
		return {};

	return _renderer->createAttributeObject(
		shader,
		stage,
		properties,
		vertexAttributes,
		std::move(vertexData),
		offset,
		label
	);
}

int GraphicsManager::getUniformIndex(
	const std::string &shaderName,
	const std::string &stage,
	uint32_t properties,
	const std::string &id) {
	if (!_renderer)
		return -1;

	return _renderer->getUniformIndex(shaderName, stage, properties, id);
}

void GraphicsManager::update() {
	if (!_renderer)
		return;

	_renderer->update();
}

void GraphicsManager::drawFrame() {
	if (!_renderer)
		return;

	_renderer->update();
	_renderer->drawFrame();
}

void GraphicsManager::setAmbianceState(const std::string &id) {
	if (!_renderer)
		return;

	std::unique_ptr<Common::ReadStream> ambianceFile(
			ResMan.getResource(fmt::format("ambiance_presets/{}.xml", id))
	);

	_renderer->setAmbianceState(AmbianceState(*ambianceFile));
}

void GraphicsManager::setAtmosphere(const std::string &id) {
	if (!_renderer)
		return;

	const auto atmPath = fmt::format("atmosphere/{}.atm", id);
	std::unique_ptr<Common::ReadStream> atmStream(ResMan.getResource(atmPath));
	if (!atmStream)
		throw CreateException("Cannot find {}", atmPath);

	AWE::ATMFile atm(*atmStream);
	auto skyLUT = createTexture(TEX(atm.getAtmosphericLUT()), fmt::format("{}_skylut", id));
	skyLUT->setWrapMode(WrapMode::kClamp, WrapMode::kClamp);
	_renderer->setSkyLUT(skyLUT);
}

void GraphicsManager::setCamera(Camera &camera) {
	if (!_renderer)
		return;

	_renderer->setCamera(camera);
}

}
