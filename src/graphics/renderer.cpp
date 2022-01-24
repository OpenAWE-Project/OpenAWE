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

#include <algorithm>

#include "renderer.h"

Graphics::Renderer::Renderer() : _currentVideoFrame(Common::UUID::generateNil()) {

}

Graphics::Renderer::~Renderer() {
}

void Graphics::Renderer::addModel(Graphics::Model *model) {
	_models.emplace_back(model);
}

void Graphics::Renderer::removeModel(Graphics::Model *model) {
	const auto iter = std::find(_models.begin(), _models.end(), model);
	if (iter != _models.end())
		_models.erase(iter);
}

void Graphics::Renderer::addGUIElement(Graphics::GUIElement *gui) {
	_guiElements.emplace_back(gui);
}

void Graphics::Renderer::setCamera(Graphics::Camera &camera) {
	_camera = camera;
}

void Graphics::Renderer::setAmbianceState(const Graphics::AmbianceState ambiance) {
	_ambiance = ambiance;
}

void Graphics::Renderer::setCurrentVideoFrame(const Common::UUID &id) {
	_currentVideoFrame = id;
}
