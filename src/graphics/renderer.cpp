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

#include "src/common/toml.h"

#include "src/awe/hg.h"
#include "src/awe/resman.h"

#include "src/graphics/renderer.h"
#include "src/graphics/std140layout.h"

Graphics::Renderer::Renderer() {
	// Setup initial projection matrix
	_projection = glm::perspectiveFov(45.0f, 1920.0f, 1080.0f, 1.0f, 10000.0f);

	// Initialize frustrum with projection matrix
	_frustrum.setProjectionMatrix(_projection);
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

void Graphics::Renderer::setAmbianceState(const Graphics::AmbianceState ambiance) {
	_ambiance = ambiance;
}

void Graphics::Renderer::setRayleigh(const glm::vec3 &rayleigh) {
	if (_rayleigh == rayleigh)
		return;

	_globalChanged = true;
	_rayleigh = rayleigh;
}

void Graphics::Renderer::setMie(const glm::vec3 &mie) {
	if (_mie == mie)
		return;

	_globalChanged = true;
	_mie = mie;
}

void Graphics::Renderer::setHGPhase(float hgPhase) {
	if (_hgPhase == hgPhase)
		return;

	_globalChanged = true;
	_hgPhase = hgPhase;
}

void Graphics::Renderer::setSkyLUT(TexturePtr lut) {
	_skyLUT = lut;
}

void Graphics::Renderer::setSky(Graphics::SkyPtr sky) {
	_sky = sky;
}

void Graphics::Renderer::update() {
	_view = _camera ? (*_camera).get().getLookAt() : glm::identity<glm::mat4>();
	const auto viewSky = _camera ? (*_camera).get().getLookAtSky() : glm::identity<glm::mat4>();
	_frustrum.setViewMatrix(_view);

	if (!_globalBuffer)
		_globalBuffer = createBuffer(kUniformBuffer, false, "Global Buffer");
	if (!_globalSkyBuffer)
		_globalSkyBuffer = createBuffer(kUniformBuffer, false, "Global Sky Buffer");

	if (!_cloudParamsBuffer) {
		_cloudParamsBuffer = createBuffer(kUniformBuffer, false, "Cloud Params Buffer");
	}

	_cloudParamsBuffer->write(std140::pack(cloudParams));

	_sunDir = _sky->getSunDir();

	const auto hgConstants = AWE::generateHenyeyGreensteinConstants(_hgPhase);

	global.viewToClip = _projection;
	global.viewToWorld = glm::inverse(_view);
	global.worldToView = _view;
	global.worldToClip = _projection * _view;
	global.clipToView = glm::inverse(_projection);
	global.clipToWorld = global.viewToWorld * global.clipToView;
	global.screenRes = {1920, 1080};
	global.atmosphers.sunDir = _sunDir;
	global.atmosphers.rayleigh = _rayleigh;
	global.atmosphers.mie = _mie;
	global.atmosphers.hgConstants[0] = hgConstants.at(0);
	global.atmosphers.hgConstants[1] = hgConstants.at(1);
	global.atmosphers.hgConstants[2] = hgConstants.at(2);

	globalSky = global;
	globalSky.worldToView = viewSky;
	globalSky.viewToWorld = glm::inverse(viewSky);
	globalSky.worldToClip = _projection * viewSky;
	globalSky.clipToView = globalSky.viewToWorld * globalSky.clipToView;

	cloudParams.cloudThreshold = _ambiance.getCloudThreshold();

	_globalBuffer->write(std140::pack(global));
	_globalSkyBuffer->write(std140::pack(globalSky));
}
