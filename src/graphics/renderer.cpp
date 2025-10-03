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

#include "src/common/sh.h"

#include "src/graphics/renderer.h"

Graphics::Renderer::Renderer() {
	// Setup initial projection matrix
	_projection = glm::perspectiveFov(45.0f, 1920.0f, 1080.0f, 1.0f, 10000.0f);

	// Initialize frustrum with projection matrix
	_frustrum.setProjectionMatrix(_projection);

	_directionalSH.fill(glm::zero<glm::vec3>());
}

Graphics::Renderer::~Renderer() {
}

void Graphics::Renderer::addModel(Graphics::Model *model) {
	std::map<RenderPassId, RenderTask> renderTasks;

	// Collect all render tasks
	const auto &partMeshs = model->getMesh()->getMeshs();
	for (unsigned int i = 0; i < partMeshs.size(); ++i) {
		const auto &partMesh = partMeshs[i];

		const auto shaderName = partMesh.material.getShaderName();
		const auto renderPassId = RenderPassId(shaderName, partMesh.material.getProperties());
		const auto taskIter = renderTasks.find(renderPassId);
		if (taskIter == renderTasks.end()) {
			RenderTask renderTask{model};
			renderTasks.insert({renderPassId, renderTask});
		}
		renderTasks.at(renderPassId).partMeshsToRender.emplace_back(i);
	}

	// Put tasks into render passes
	for (auto &pass : _renderPasses) {
		auto passTask = renderTasks.find(pass.id);
		if (passTask == renderTasks.end())
			continue;

		pass.renderTasks.emplace_back(passTask->second);
		renderTasks.erase(passTask);
	}

	// Try to put render tasks which cannot be put into a pass, move it into the default pass
	if (renderTasks.empty())
		return;

	const auto &defaultPass = std::find_if(
			_renderPasses.begin(),
			_renderPasses.end(),
			[](const auto &pass) -> bool {
				return pass.id == RenderPassId("standardmaterial", 0);
			}
	);

	for (const auto &task: renderTasks) {
		defaultPass->renderTasks.emplace_back(task.second);
	}
}

void Graphics::Renderer::removeModel(Graphics::Model *model) {
	for (auto &pass: _renderPasses) {
		pass.renderTasks.erase(
			std::remove_if(
				pass.renderTasks.begin(),
				pass.renderTasks.end(),
				[&](const auto &renderTask) -> bool {
					return renderTask.model == model;
				}
			),
			pass.renderTasks.end()
		);
	}
}

void Graphics::Renderer::addImGuiElement(Graphics::ImGuiElement *imGuiElement) {
	_imguiElements.emplace_back(imGuiElement);
}

void Graphics::Renderer::removeImGuiElement(Graphics::ImGuiElement *imGuiElement) {
	const auto iter = std::remove(_imguiElements.begin(), _imguiElements.end(), imGuiElement);
	_imguiElements.erase(iter);
}

void Graphics::Renderer::addGUIElement(Graphics::GUIElement *gui) {
	_guiElements.emplace_back(gui);
}

void Graphics::Renderer::removeGUIElement(Graphics::GUIElement *gui) {
	const auto iter = std::remove(_guiElements.begin(), _guiElements.end(), gui);
	_guiElements.erase(iter);
}

void Graphics::Renderer::addLight(Graphics::Light *light) {
	_lights.emplace_back(light);
}

void Graphics::Renderer::removeLight(Graphics::Light *light) {
	const auto iter = std::remove(_lights.begin(), _lights.end(), light);
	_lights.erase(iter);
}

void Graphics::Renderer::setCamera(Graphics::Camera &camera) {
	_camera = camera;
}

void Graphics::Renderer::setAmbianceState(const AmbianceState &ambiance) {
	_ambiance = ambiance;

	const auto d1 = _ambiance.getAmbientSHDirection1();
	const auto d2 = _ambiance.getAmbientSHDirection2();
	const auto c1 = _ambiance.getAmbientSHIntensity1();
	const auto c2 = _ambiance.getAmbientSHIntensity2();

	constexpr std::array<std::tuple<int, int>, 9> shParams = {
		// l = 0
		std::tuple{0, 0},

		// l = 1
		{1, 1}, {1, 0}, {1, -1},

		// l = 2
		{2, 2}, {2, 1}, {2, 0}, {2, -1}, {2, -2}
	};

	for (int i = 0; i < _directionalSH.size(); ++i) {
		const auto [l, m] = shParams[i];
		const float bf1 = Common::shRealBasisFunc(l, m, d1);
		const float bf2 = Common::shRealBasisFunc(l, m, d2);

		_directionalSH[i] = bf1 * c1 + bf2 * c2;
	}
}

void Graphics::Renderer::setSkyLUT(TexturePtr lut) {
	_skyLUT = lut;
}

void Graphics::Renderer::setSky(Graphics::SkyPtr sky) {
	_sky = sky;
}

void Graphics::Renderer::update() {
	_view = _camera ? (*_camera).get().getLookAt() : glm::identity<glm::mat4>();
	_frustrum.setViewMatrix(_view);
}
