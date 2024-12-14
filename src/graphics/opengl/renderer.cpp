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
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <set>
#include <random>
#include <numbers>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "src/common/uuid.h"
#include "src/common/writefile.h"
#include "src/common/exception.h"
#include "src/common/strutil.h"
#include "src/common/readfile.h"
#include "src/common/sh.h"

#include "src/awe/resman.h"
#include "src/awe/objfile.h"
#include "src/awe/hg.h"

#include "src/platform/procaddress.h"

#include "src/graphics/shaderconverter.h"
#include "src/graphics/skeleton.h"
#include "src/graphics/images/surface.h"
#include "src/graphics/opengl/renderer.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/vbo.h"
#include "src/graphics/opengl/convertedprogram.h"
#include "src/graphics/opengl/proxytexture.h"
#include "src/common/dxbc.h"

namespace Graphics::OpenGL {

constexpr std::array kSkyDirections = {
		"Left", "Right", "Front", "Back", "Up", "Down"
};

static void *loadProcAddress(const char *name) {
	return Platform::getProcAddressGL(name);
}

Renderer::Renderer(Platform::Window &window, const std::string &shaderDirectory) :
		_window(window),
		_shaderDirectory(shaderDirectory) {
	_window.makeCurrent();

	gladLoadGL(reinterpret_cast<GLADloadfunc>(&loadProcAddress));

	// Initialize GLEW
	/*glewExperimental = false; // TODO: Make a config argument out of it
	GLenum result = glewInit();
	if (result != GLEW_OK)
		throw std::runtime_error(reinterpret_cast<const char *>(glewGetErrorString(result)));*/

	// Initialize debug output if possible
	if (GLAD_GL_ARB_debug_output) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallbackARB(reinterpret_cast<GLDEBUGPROC>(&Renderer::debugMessageCallback), nullptr);
	}

	// Dump OpenGL Information
	//

	const std::string vendor      = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
	const std::string renderer    = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
	const std::string version     = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	const std::string glslVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	spdlog::info("OpenGL Vendor: {}", vendor);
	spdlog::info("OpenGL Renderer: {}", renderer);
	spdlog::info("OpenGL Version: {}", version);
	spdlog::info("GLSL Version: {}", glslVersion);
	assert(glGetError() == GL_NO_ERROR);

	if (GLAD_GL_NVX_gpu_memory_info) {
		GLint dedicatedVideoMemory, totalAvailableMemory, currentAvailableMemory;
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicatedVideoMemory);
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalAvailableMemory);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentAvailableMemory);

		spdlog::info("Dedicated video memory: {}kb", dedicatedVideoMemory);
		spdlog::info("Total available memory: {}kb", totalAvailableMemory);
		spdlog::info("Current available video memory: {}kb", currentAvailableMemory);
	} else if (GLAD_GL_ATI_meminfo) {
		GLint vboFreeMemory[4], textureFreeMemory[4], renderbufferFreeMemory[4];
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, vboFreeMemory);
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, textureFreeMemory);
		glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, renderbufferFreeMemory);

		spdlog::info("VBO free memory: {}", vboFreeMemory[0]);
		spdlog::info("VBO free auxiliary memory: {}", vboFreeMemory[2]);
		spdlog::info("Texture free memory: {}", textureFreeMemory[0]);
		spdlog::info("Texture free auxiliary memory: {}", textureFreeMemory[2]);
		spdlog::info("Renderbuffer free memory: {}", renderbufferFreeMemory[0]);
		spdlog::info("Renderbuffer free auxiliary memory: {}", renderbufferFreeMemory[2]);
	}

	GLint maxTextureUnits, maxVertexAttribs, maxUniformLocations, maxVertexUniformComponents, maxFragmentUniformComponents;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
	spdlog::info("OpenGL Max Texture Units: {}", maxTextureUnits);
	spdlog::info("OpenGL Max Vertex Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Uniform Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Vertex Uniform Components: {}", maxVertexUniformComponents);
	spdlog::info("OpenGL Max Fragment Uniform Components: {}", maxFragmentUniformComponents);

	if (GLAD_GL_ARB_uniform_buffer_object) {
		GLint maxUniformBufferBindings, maxUniformBlockSize, maxVertexUniformBlocks, maxFragmentUniformBlocks;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertexUniformBlocks);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &maxFragmentUniformBlocks);
		spdlog::info("OpenGL Max Uniform Buffer Bindings: {}", maxUniformBufferBindings);
		spdlog::info("OpenGL Max Uniform Block Size: {}", maxUniformBlockSize);
		spdlog::info("OpenGL Max Vertex Uniform Blocks: {}", maxVertexUniformBlocks);
		spdlog::info("OpenGL Max Fragment Uniform Blocks: {}", maxFragmentUniformBlocks);
	}

	if (GLAD_GL_ARB_tessellation_shader) {
		GLint maxPatchVertices;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
		spdlog::info("OpenGL Max Patch Vertices: {}", maxPatchVertices);
		if (maxPatchVertices < 4) {
			throw CreateException("Not enough patch vertices available. Expected 4, got {}", maxPatchVertices);
		}
	}

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	spdlog::info("Num OpenGL Extensions: {}", numExtensions);

	std::string extensions;
	spdlog::debug("Found Extensions:");
	for (int i = 0; i < numExtensions; ++i) {
		const std::string extension = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
		spdlog::debug("- {}", extension);
	}

	if (!GLAD_GL_EXT_texture_compression_s3tc) {
		throw std::runtime_error("No S3TC extension available");
	}

	_hasDebug = GLAD_GL_KHR_debug;
	if (_hasDebug)
		spdlog::info("GL_KHR_debug extension found, opengl labelling and messaging will be available");

	// Make some initial settings
	//

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	assert(glGetError() == GL_NO_ERROR);
	glPointSize(10);
	glLineWidth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glFrontFace(GL_CW);
	//glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_LEQUAL);
	//glShadeModel(GL_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Read and initialize shaders
	//
	rebuildShaders();

	// Get width and height of the default framebuffer
	unsigned int width, height;
	window.getSize(width, height);

	// Initialize sky cube framebuffer
	//
	_skyCubeBuffer = std::make_unique<Framebuffer>("Sky Buffer");
	_skyCubeBuffer->setClearColor({0.0, 0.0, 0.0, 0.0});
	_skyCubeTexture = std::make_shared<Texture>(
			_loadingTasks,
			16,
			6 * 16,
			kRGBA16F,
			"sky_buffer"
	);
	_skyCubeBuffer->attachTexture(*_skyCubeTexture, GL_COLOR_ATTACHMENT0);

	// Initialize deferred shading
	//
	_depthTexture = std::make_unique<Texture>(_loadingTasks, width, height, kRGBA16F, "depth_buffer");
	_normalTexture = std::make_unique<Texture>(_loadingTasks, width, height, kRGBA16F, "normal_buffer");
	_depthstencilBuffer = std::make_unique<Renderbuffer>(width, height, GL_DEPTH24_STENCIL8,
														 "depthstencil_renderbuffer");

	_deferredBuffer = std::make_unique<Framebuffer>("Deferred Buffer");
	_deferredBuffer->setClearColor({0.0f, 0.0f, 0.0f, 0.0f});
	_deferredBuffer->bind();

	_deferredBuffer->attachRenderBuffer(*_depthstencilBuffer, GL_DEPTH_STENCIL_ATTACHMENT);

	_deferredBuffer->attachTexture(*_depthTexture, GL_COLOR_ATTACHMENT0);
	_deferredBuffer->attachTexture(*_normalTexture, GL_COLOR_ATTACHMENT1);

	_lightBufferTexture = std::make_unique<Texture>(_loadingTasks, width, height, kRGBA16F, "light_buffer");

	_lightBuffer = std::make_unique<Framebuffer>("Light Buffer");
	_lightBuffer->setClearColor({0.0f, 0.0f, 0.0f, 0.0f});
	_lightBuffer->bind();

	_lightBuffer->attachTexture(*_lightBufferTexture, GL_COLOR_ATTACHMENT0);
	_lightBuffer->attachRenderBuffer(*_depthstencilBuffer, GL_DEPTH_STENCIL_ATTACHMENT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Initialize noise map
	//
	std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<unsigned int> dist(0, 0xFF);

	auto noiseSurface = Surface(32, 32, 64, kRGBA8);
	for (int i = 0; i < 32 * 32 * 64; ++i) {
		const auto date = dist(mt);
		std::memset(reinterpret_cast<byte*>(noiseSurface.getData()) + i * 4, date,	4);
	}

	_noiseMap = std::make_unique<Texture>(_loadingTasks, GL_TEXTURE_3D, "noise_map");
	_noiseMap->load(std::move(noiseSurface));

	// Initialize basis func map
	//
	std::array<float, 144 * 96> basisData;
	std::ranges::fill(basisData, 0.0);

	constexpr std::array<std::tuple<int, int>, 9> shParams = {
			// l = 0
			std::tuple<int, int>{0, 0},

			// l = 1
			{1, -1}, {1, 0}, {1, 1},

			// l = 2
			{2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}
	};

	static constexpr float kSphereSideArea = (4 * std::numbers::pi) / 6;

	for (int iy = 0; iy < 6; ++iy) { // Cube Plane
		for (int ix = 0; ix < 9; ++ix) { // Degree
			const auto xoffset = ix * 16;
			const auto yoffset = iy * 16;

			const auto [l, m] = shParams[ix];

			float sum = 0;
			for (int by = 0; by < 16; ++by) {
				for (int bx = 0; bx < 16; ++bx) {
					const auto targetPixelVector = glm::vec3{
							static_cast<float>(bx - 7.5) / 8.0,
							1,
							static_cast<float>(by - 7.5) / 8.0,
					};
					const auto baseVector = glm::vec3{0, 1, 0};
					const auto shValue = Common::shBasisFunc(l, m, targetPixelVector);
					const auto weight = (glm::dot(targetPixelVector, baseVector) / (glm::length(targetPixelVector) * glm::length(baseVector))) / std::pow(glm::length(targetPixelVector), 2);
					sum += std::abs(shValue.real() * weight);
					basisData[(yoffset + by) * 144 + xoffset + bx] = shValue.real() * weight;
				}
			}

			for (int by = 0; by < 16; ++by) {
				for (int bx = 0; bx < 16; ++bx) {
					basisData[(yoffset + by) * 144 + xoffset + bx] *= kSphereSideArea / sum;
				}
			}
		}

		break;
	}

	auto basisFuncsSurface = Surface(144, 96, kR32F);
	std::memcpy(basisFuncsSurface.getData(), basisData.data(), basisData.size() * sizeof(float));
	_basisFunc = std::make_shared<Texture>(_loadingTasks, GL_TEXTURE_2D, "basis_funcs");
	_basisFunc->load(std::move(basisFuncsSurface));

	// Initialize ImGui
	ImGui_ImplOpenGL3_Init();

	// Check for errors
	assert(glGetError() == GL_NO_ERROR);
}

Renderer::~Renderer() {
	ImGui_ImplOpenGL3_Shutdown();
}

void Renderer::drawFrame() {
	_window.makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_deferredBuffer->bind();
	_deferredBuffer->clear();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawWorld("depth");

	_lightBuffer->bind();
	_lightBuffer->clear();
	drawLights();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	drawSky();
	drawWorld("material");
	drawGUI();

	drawImGui();

	_window.swap();
}

void Renderer::update() {
	Graphics::Renderer::update();

	_window.makeCurrent();

	// Loading
	while (!_loadingTasks.empty()) {
		auto &task = _loadingTasks.front();
		task->apply();
		_loadingTasks.pop_front();
	}
}

void Renderer::drawWorld(const std::string &stage) {
	glm::mat4 vp = _projection * _view;
	glm::mat4 viewToWorldMat = glm::inverse(_view);

	std::vector<glm::mat4x3> placeholderMatrices(64);
	std::fill(placeholderMatrices.begin(), placeholderMatrices.end(), glm::identity<glm::mat4x3>());

	const auto &defaultShader = getProgram("standardmaterial", stage, 0);
	static const glm::mat4 mirrorZ = glm::scale(glm::vec3(1, 1, -1));

	const auto screenResolution = glm::vec2(1920, 1080);

	bool wireframe = false;
	Material::CullMode cullMode = Material::kNone;

	pushDebug(fmt::format("Draw stage {}", stage));

	for (const auto &pass: _renderPasses) {
		if (pass.renderTasks.empty())
			continue;

		if (pass.id.programName == "terrain")
			int i = 0;

		pushDebug(fmt::format("Pass {} {:0<8x}", pass.id.programName, pass.id.properties));

		ProgramPtr currentShader = (!hasProgram(pass.id.programName, stage, pass.id.properties)) ? defaultShader
																								 : getProgram(
						pass.id.programName, stage, pass.id.properties);
		currentShader->bind();

		const std::optional<GLint> localToView = currentShader->getUniformLocation("g_mLocalToView");
		const std::optional<GLint> localToClip = currentShader->getUniformLocation("g_mLocalToClip");
		const std::optional<GLint> viewToClip = currentShader->getUniformLocation("g_mViewToClip");
		const std::optional<GLint> viewToWorld = currentShader->getUniformLocation("g_mViewToWorld");
		const std::optional<GLint> worldToView = currentShader->getUniformLocation("g_mWorldToView");
		const std::optional<GLint> screenRes = currentShader->getUniformLocation("g_vScreenRes");
		const std::optional<GLint> lightBuffer = currentShader->getUniformLocation("g_sLightBuffer");

		const std::optional<GLint> skinningMatrices = currentShader->getUniformLocation("GPU_skinning_matrices");

		if (screenRes)
			currentShader->setUniform2f(*screenRes, screenResolution);

		GLuint textureSlotShader = 0;
		if (lightBuffer) {
			glActiveTexture(getTextureSlot(textureSlotShader));
			_lightBufferTexture->bind();
			currentShader->setUniformSampler(*lightBuffer, textureSlotShader++);
		}

		for (const auto &task: pass.renderTasks) {
			if (!task.model->isVisible())
				continue;

			glm::mat4 m = mirrorZ * task.model->getTransform();
			glm::mat4 mv = _view * m;
			glm::mat4 vm = task.model->getInverseTransform() * viewToWorldMat;
			glm::mat4 mvp = vp * m;

			const MeshPtr mesh = task.model->getMesh();
			if (task.model->hasBoundSphere()) {
				auto boundingSphere = task.model->getBoundSphere();
				boundingSphere.position = m * glm::vec4(boundingSphere.position, 1.0f);
				if (!_frustrum.test(boundingSphere))
					continue;
			}

			if (task.partMeshsToRender.empty())
				continue;

			pushDebug(task.model->getLabel());

			const auto &partMeshs = mesh->getMeshs();
			const auto indices = std::static_pointer_cast<Graphics::OpenGL::VBO>(mesh->getIndices());

			if (localToView)
				currentShader->setUniformMatrix4f(*localToView, mv);
			if (localToClip)
				currentShader->setUniformMatrix4f(*localToClip, mvp);
			if (viewToClip)
				currentShader->setUniformMatrix4f(*viewToClip, _projection);
			if (viewToWorld)
				currentShader->setUniformMatrix4f(*viewToWorld, vm);
			if (worldToView)
				currentShader->setUniformMatrix4f(*worldToView, _view);

			for (const auto &meshToRender: task.partMeshsToRender) {
				const auto &partmesh = partMeshs[meshToRender];

				if (!partmesh.material.hasStage(stage))
					continue;

				std::static_pointer_cast<Graphics::OpenGL::VAO>(partmesh.vertexAttributes.find(stage)->second)->bind();
				if (indices)
					indices->bind();

				GLuint textureSlot = textureSlotShader;
				applyUniforms(currentShader, partmesh.material.getUniforms(stage), textureSlot);

				const auto uniforms = task.model->getUniforms(
						stage,
						pass.id.programName,
						pass.id.properties
				);
				applyUniforms(currentShader, uniforms, textureSlot);

				if (task.model->hasSkeleton() && skinningMatrices) {
					const auto matrices = task.model->getSkeleton().getSkinningMatrices(partmesh.boneMap);
					currentShader->setUniformMatrix4x3fArray(*skinningMatrices, matrices);
				} else if (skinningMatrices) {
					currentShader->setUniformMatrix4x3fArray(*skinningMatrices, placeholderMatrices);
				}

				GLenum type;
				switch (partmesh.renderType) {
					default:
					case Mesh::kTriangles:
						type = GL_TRIANGLES;
						break;
					case Mesh::kTriangleFan:
						type = GL_TRIANGLE_FAN;
						break;
					case Mesh::kLines:
						type = GL_LINES;
						break;
					case Mesh::kPoints:
						type = GL_POINTS;
						break;
					case Mesh::kPatches:
						type = GL_PATCHES;
						break;
				}

				if (cullMode != partmesh.material.getCullMode()) {
					const Material::CullMode newCullMode = partmesh.material.getCullMode();
					if (cullMode != Material::kNone && newCullMode == Material::kNone)
						glDisable(GL_CULL_FACE);
					else if (cullMode == Material::kNone && newCullMode != Material::kNone)
						glEnable(GL_CULL_FACE);

					switch (newCullMode) {
						case Material::kFront:
							glCullFace(GL_FRONT);
							break;
						case Material::kBack:
							glCullFace(GL_BACK);
							break;
						case Material::kFrontBack:
							glCullFace(GL_FRONT_AND_BACK);
							break;
						default:
							break;
					}

					cullMode = newCullMode;
				}

				if (partmesh.wireframe && !wireframe) {
					glDisable(GL_DEPTH_TEST);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					wireframe = true;
				} else if (!partmesh.wireframe && wireframe) {
					glEnable(GL_DEPTH_TEST);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					wireframe = false;
				}

				if (task.model->getNumInstances() != 1) {
					if (!indices) {
						glDrawArraysInstanced(
								type,
								0,
								partmesh.length,
								task.model->getNumInstances()
						);
					} else {
						glDrawElementsInstanced(
								type,
								partmesh.length,
								GL_UNSIGNED_SHORT,
								reinterpret_cast<void *>(partmesh.offset),
								task.model->getNumInstances()
						);
					}
				} else {
					if (!indices) {
						glDrawArrays(
								type,
								0,
								partmesh.length
						);
					} else {
						glDrawElements(
								type,
								partmesh.length,
								GL_UNSIGNED_SHORT,
								reinterpret_cast<void *>(partmesh.offset)
						);
					}
				}
			}

			popDebug(); // Pop Model Message
		}

		popDebug(); // Pop Pass Message
	}

	popDebug(); // Pop Stage Message
}

void Renderer::drawLights() {
	pushDebug("Draw Lights");

	auto stencilProgram = getProgram("deferredlight", "render_stencil", 0);
	auto pointlightProgram = getProgram("deferredlight", "pointlight", 0);

	const glm::mat4 vp = _projection * _view;
	const glm::mat4 clipToView = glm::inverse(_projection);
	const glm::mat4 mirrorZ = glm::scale(glm::vec3(1, 1, -1));
	const GLint localToClipIndex = *pointlightProgram->getUniformLocation("g_mLocalToClip");
	const GLint localToClipStencilIndex = *stencilProgram->getUniformLocation("g_mLocalToClip");
	const GLint clipToViewIndex = *pointlightProgram->getUniformLocation("g_mClipToView");

	const GLint lightColorIndex = *pointlightProgram->getUniformLocation("g_vLightColor");
	const GLint lightFalloffIndex = *pointlightProgram->getUniformLocation("g_vLightFalloff");
	const GLint lightPositionIndex = *pointlightProgram->getUniformLocation("g_vLightPosition");
	const GLint screenResIndex = *pointlightProgram->getUniformLocation("g_vScreenRes");
	const GLint linearDepthIndex = *pointlightProgram->getUniformLocation("g_sLinearDepthBuffer");
	const GLint normalBufferIndex = *pointlightProgram->getUniformLocation("g_sNormalBuffer");

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	for (const auto &light: _lights) {
		if (!light->isEnabled())
			continue;

		if (!_frustrum.test(Common::BoundSphere{mirrorZ * light->getTransform()[3], light->getRange() * 1.075f}))
			continue;

		pushDebug(light->getLabel());

		glClear(GL_STENCIL_BUFFER_BIT);

		const auto vao = std::static_pointer_cast<Graphics::OpenGL::VAO>(light->getAttributeObject());
		const auto indices = std::static_pointer_cast<Graphics::OpenGL::VBO>(light->getIndices());


		// Render a stencil test
		//
		stencilProgram->bind();

		vao->bind();
		indices->bind();

		glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 1, 0xFF);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_REPLACE, GL_KEEP);

		stencilProgram->setUniformMatrix4f(localToClipStencilIndex, vp * mirrorZ * light->getTransform());

		glDrawElements(
				GL_TRIANGLES,
				light->getNumIndices(),
				GL_UNSIGNED_SHORT,
				reinterpret_cast<void *>(0)
		);


		// Render the actual pointlight
		//
		pointlightProgram->bind();

		vao->bind();
		indices->bind();

		glStencilFuncSeparate(GL_BACK, GL_EQUAL, 1, 0xFF);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_REPLACE, GL_KEEP);

		if (linearDepthIndex) {
			glActiveTexture(GL_TEXTURE0);
			_depthTexture->bind();
			pointlightProgram->setUniformSampler(linearDepthIndex, 0);
		}

		glActiveTexture(GL_TEXTURE1);
		_normalTexture->bind();
		pointlightProgram->setUniformSampler(normalBufferIndex, 1);

		const auto cameraLightPositionView = _view * mirrorZ * light->getTransform()[3];

		pointlightProgram->setUniform2f(screenResIndex, glm::vec2(1920, 1080));
		pointlightProgram->setUniformMatrix4f(localToClipIndex, vp * mirrorZ * light->getTransform());
		pointlightProgram->setUniformMatrix4f(clipToViewIndex, clipToView);
		pointlightProgram->setUniform3f(lightPositionIndex, cameraLightPositionView);
		pointlightProgram->setUniform3f(lightColorIndex, light->getColor() * light->getIntensity());
		pointlightProgram->setUniform4f(lightFalloffIndex, light->getFalloff());

		glDrawElements(
				GL_TRIANGLES,
				light->getNumIndices(),
				GL_UNSIGNED_SHORT,
				reinterpret_cast<void *>(0)
		);

		popDebug();
	}

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	popDebug(); // Pop Draw Lights message
}

void Renderer::drawGUI() {
	glm::mat4 vp = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1000.0f, 1000.0f);

	pushDebug("Draw GUI");

	auto program = getProgram("gui", "material", 0);
	program->bind();

	const GLint locationTexture = *program->getUniformLocation("g_sTexture");
	const GLint locationMVP = *program->getUniformLocation("g_mMVP");

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	for (const auto &element: _guiElements) {
		glm::mat4 m = glm::translate(glm::vec3(
				element->getAbsolutePosition() +
				element->getRelativePosition() * glm::vec2(1920.0, 1080.0),
				0.0f
		));
		std::static_pointer_cast<Graphics::OpenGL::VAO>(element->getVertexAttributes())->bind();

		for (const auto &part: element->getParts()) {
			glm::mat4 m2 = m *
						   glm::scale(glm::vec3(part.absoluteSize + part.relativeSize * glm::vec2(1920, 1080), 1.0f)) *
						   glm::translate(glm::vec3(part.position, 1.0f));
			std::static_pointer_cast<Graphics::OpenGL::VBO>(part.indices)->bind();

			glActiveTexture(getTextureSlot(0));
			std::dynamic_pointer_cast<Graphics::OpenGL::GLTexture>(part.texture)->bind();
			program->setUniformSampler(locationTexture, 0);
			program->setUniformMatrix4f(locationMVP, vp * m2);

			glDrawElementsBaseVertex(
					GL_TRIANGLES,
					part.indicesCount,
					GL_UNSIGNED_SHORT,
					reinterpret_cast<void *>(part.indicesOffset),
					part.verticesOffset
			);
		}
	}

	glEnable(GL_DEPTH_TEST);

	popDebug(); // Pop Draw GUI Message
}

void Renderer::drawSky() {
	if (!_sky)
		return;

	pushDebug("Draw Sky");

	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);

	const glm::mat4 p = glm::perspectiveFov(45.0f, 1920.0f, 1080.0f, 1.0f, 10000.0f);
	const glm::mat4 v = glm::lookAt(glm::zero<glm::vec3>(), _camera->get().getDirection(), _camera->get().getUp());
	const glm::mat4 pv = glm::inverse(p * v);

	auto skyProgram = getProgram("sky", "night_sky", 0);
	skyProgram->bind();

	const auto viewToClip = skyProgram->getUniformLocation("g_mViewToClip");
	const auto worldToView = skyProgram->getUniformLocation("g_mWorldToView");
	const auto viewToWorld = skyProgram->getUniformLocation("g_mViewToWorld");
	const auto clipToWorld = skyProgram->getUniformLocation("g_mClipToWorld");
	const auto screenRes = skyProgram->getUniformLocation("g_vScreenRes");

	const auto noiseMap = skyProgram->getUniformLocation("g_sNoiseMap");
	const auto stereoBuffer = skyProgram->getUniformLocation("g_sStereoBuffer");

	const auto skyMesh = _sky->getSkyMesh();
	assert(skyMesh);
	const auto indices = std::static_pointer_cast<Graphics::OpenGL::VBO>(skyMesh->getIndices());

	for (const auto &mesh: skyMesh->getMeshs()) {
		assert(glGetError() == GL_NO_ERROR);
		std::static_pointer_cast<Graphics::OpenGL::VAO>(
				mesh.vertexAttributes.find("night_sky")->second
		)->bind();

		indices->bind();

		if (viewToClip)
			skyProgram->setUniformMatrix4f(*viewToClip, p);
		if (worldToView)
			skyProgram->setUniformMatrix4f(*worldToView, v);
		if (clipToWorld)
			skyProgram->setUniformMatrix4f(*clipToWorld, glm::inverse(p * v));

		if (screenRes)
			skyProgram->setUniform2f(*screenRes, glm::vec2(1920.0, 1080.0));

		GLuint textureSlot = 0;
		/*if (noiseMap) {
			glActiveTexture(getTextureSlot(textureSlot));
			_noiseMap->bind();
			skyProgram->setUniformSampler(*noiseMap, textureSlot++);
		}

		if (stereoBuffer) {
			glActiveTexture(getTextureSlot(textureSlot));
			_stereoBuffer->bind();
			skyProgram->setUniformSampler(*stereoBuffer, textureSlot++);
		}*/

		applyUniforms(skyProgram, mesh.material.getUniforms("night_sky"), textureSlot);
		applyUniforms(skyProgram, _sky->getUniforms(), textureSlot);

		glDrawElements(
				GL_TRIANGLES,
				mesh.length,
				GL_UNSIGNED_SHORT,
				reinterpret_cast<const void *>(mesh.offset)
		);
	}

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);

	popDebug();
}

void Renderer::drawImGui() {
	pushDebug("Draw ImGui");

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	for (const auto &imguiElement: _imguiElements) {
		imguiElement->draw();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	popDebug();
}

void Renderer::pushDebug(const std::string &message) {
	if (!_hasDebug)
		return;

	glPushDebugGroup(
			GL_DEBUG_SOURCE_APPLICATION,
			0,
			message.size(),
			message.c_str()
	);
}

void Renderer::popDebug() {
	if (!_hasDebug)
		return;

	glPopDebugGroup();
}

ProgramPtr Renderer::getProgram(const std::string &name, const std::string &stage, const uint32_t property) {
	const auto programIter = _programs.find(RenderPassId(name, property));
	if (programIter == _programs.cend())
		throw CreateException("Couldn't find program with name '{}' and property 0x{:08x}", stage, property);
	return programIter->second->getProgram(stage);
}

bool Renderer::hasProgram(const std::string &name, const std::string &stage, uint32_t properties) {
	const auto id = RenderPassId(name, properties);

	if (_programs.find(id) == _programs.end())
		return false;

	const auto &programCollection = _programs[id];
	if (!programCollection->hasStage(stage))
		return false;

	return true;
}

GLenum Renderer::getTextureSlot(unsigned int slot) {
	switch (slot % 32) {
		default:
		case 0:
			return GL_TEXTURE0;
		case 1:
			return GL_TEXTURE1;
		case 2:
			return GL_TEXTURE2;
		case 3:
			return GL_TEXTURE3;
		case 4:
			return GL_TEXTURE4;
		case 5:
			return GL_TEXTURE5;
		case 6:
			return GL_TEXTURE6;
		case 7:
			return GL_TEXTURE7;
		case 8:
			return GL_TEXTURE8;
		case 9:
			return GL_TEXTURE9;
		case 10:
			return GL_TEXTURE10;
		case 11:
			return GL_TEXTURE11;
		case 12:
			return GL_TEXTURE12;
		case 13:
			return GL_TEXTURE13;
		case 14:
			return GL_TEXTURE14;
		case 17:
			return GL_TEXTURE17;
		case 18:
			return GL_TEXTURE18;
		case 19:
			return GL_TEXTURE19;
		case 20:
			return GL_TEXTURE20;
		case 21:
			return GL_TEXTURE21;
		case 22:
			return GL_TEXTURE22;
		case 23:
			return GL_TEXTURE23;
		case 24:
			return GL_TEXTURE24;
		case 25:
			return GL_TEXTURE25;
		case 26:
			return GL_TEXTURE26;
		case 27:
			return GL_TEXTURE27;
		case 28:
			return GL_TEXTURE28;
		case 29:
			return GL_TEXTURE29;
		case 30:
			return GL_TEXTURE30;
		case 31:
			return GL_TEXTURE31;
	}
}

void Renderer::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
									const GLchar *message, void *userParam) {
	const std::string msg(message, length);

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			spdlog::error(msg);
			return;

		case GL_DEBUG_TYPE_PUSH_GROUP:
		case GL_DEBUG_TYPE_POP_GROUP:
			return;

		default:
			switch (severity) {
				case GL_DEBUG_SEVERITY_HIGH:
					spdlog::info(msg);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					spdlog::debug(msg);
					break;
				default:
				case GL_DEBUG_SEVERITY_LOW:
					spdlog::trace(msg);
					break;
			}
			return;
	}
}

ProxyTexturePtr Renderer::createProxyTexture() {
	return std::make_shared<Graphics::OpenGL::ProxyTexture>();
}

TexturePtr Renderer::createTexture(TextureType type, const std::string &label) {
	GLenum texType;
	switch (type) {
		case kTexture2D:
			texType = GL_TEXTURE_2D;
			break;
		case kTexture3D:
			texType = GL_TEXTURE_3D;
			break;
		case kTextureCube:
			texType = GL_TEXTURE_CUBE_MAP;
			break;
		default:
			throw Common::Exception("Invalid texture type");
	}

	return std::make_shared<Graphics::OpenGL::Texture>(_loadingTasks, texType, label);
}

BufferPtr Renderer::createBuffer(BufferType type, bool modifiable) {
	GLenum bufferType;
	GLenum usage;

	switch (type) {
		case kIndexBuffer:
			bufferType = GL_ELEMENT_ARRAY_BUFFER;
			break;
		case kVertexBuffer:
			bufferType = GL_ARRAY_BUFFER;
			break;
		case kUniformBuffer:
			bufferType = GL_UNIFORM_BUFFER;
			break;
		default:
			throw Common::Exception("Invalid buffer type");
	}

	if (modifiable) {
		usage = GL_DYNAMIC_DRAW;
	} else {
		usage = GL_STATIC_DRAW;
	}

	return std::make_shared<Graphics::OpenGL::VBO>(_loadingTasks, bufferType, usage);
}

int Renderer::getUniformIndex(const std::string &shaderName, const std::string &stage, uint32_t properties,
							  const std::string &id) {
	if (!hasProgram(shaderName, stage, properties))
		return -1;

	const auto uniformLocation = getProgram(shaderName, stage, properties)->getUniformLocation(id);
	if (!uniformLocation)
		return -1;

	return *uniformLocation;
}

AttributeObjectPtr
Renderer::createAttributeObject(
		const std::string &shader,
		const std::string &stage,
		uint32_t properties,
		const std::vector<VertexAttribute> &vertexAttributes,
		BufferPtr vertexData,
		unsigned int offset,
		const std::string &label
) {
	auto vao = std::make_unique<VAO>(_loadingTasks, label);

	std::string shaderName = shader;
	if (!hasProgram(shaderName, stage, properties))
		shaderName = "standardmaterial";

	ProgramPtr program = getProgram(shaderName, stage, properties);

	vao->applyAttributes(program, vertexAttributes, vertexData, offset);

	return vao;
}

void Renderer::applyUniforms(ProgramPtr &program, std::vector<Material::Uniform> uniforms, GLuint &textureSlot) {
	for (const auto &attribute: uniforms) {
		switch (attribute.type) {
			case Material::kFloat: {
				float value = std::get<float>(attribute.data);
				program->setUniform1f(attribute.index, value);
				break;
			}

			case Material::kVec2: {
				glm::vec2 value = std::get<glm::vec2>(attribute.data);
				program->setUniform2f(attribute.index, value);
				break;
			}

			case Material::kVec3: {
				glm::vec3 value = std::get<glm::vec3>(attribute.data);
				program->setUniform3f(attribute.index, value);
				break;
			}

			case Material::kVec4: {
				glm::vec4 value = std::get<glm::vec4>(attribute.data);
				program->setUniform4f(attribute.index, value);
				break;
			}

			case Material::kFloatArray: {
				std::vector<float> values = std::get<std::vector<float>>(attribute.data);
				program->setUniform1fArray(attribute.index, values);
				break;
			}

			case Material::kVec2Array: {
				std::vector<glm::vec2> values = std::get<std::vector<glm::vec2>>(attribute.data);
				program->setUniform2fArray(attribute.index, values);
				break;
			}

			case Material::kVec3Array: {
				std::vector<glm::vec3> values = std::get<std::vector<glm::vec3>>(attribute.data);
				program->setUniform3fArray(attribute.index, values);
				break;
			}

			case Material::kVec4Array: {
				std::vector<glm::vec4> values = std::get<std::vector<glm::vec4>>(attribute.data);
				program->setUniform4fArray(attribute.index, values);
				break;
			}

			case Material::kTexture:
				glActiveTexture(getTextureSlot(textureSlot));
				std::dynamic_pointer_cast<Graphics::OpenGL::GLTexture>(std::get<TexturePtr>(attribute.data))->bind();
				program->setUniformSampler(attribute.index, textureSlot);
				textureSlot += 1;
				break;
		}
	}
}

void Renderer::rebuildShaders() {
	// Clear all previously created programs
	_programs.clear();

	const std::regex objFile("^[a-z0-9]+\\.obj$");
	for (const auto &item: std::filesystem::directory_iterator(_shaderDirectory)) {
		const std::string filename = item.path().filename().string();
		if (!item.is_regular_file())
			continue;

		if (!std::regex_match(Common::toLower(filename), objFile))
			continue;

		auto *objStream = new Common::ReadFile(item.path().string());
		AWE::OBJFile obj(objStream);

		spdlog::info("Loading shader archive {}", filename);

		for (auto &program: obj.getPrograms()) {
			for (const auto &shader: program.shaders) {
				spdlog::info("Loading and converting shader progran {} with id 0x{:0>8x}", program.name, shader.flags);
				const auto &vertexShaderStream = *shader.vertexShader;
				const auto &pixelShaderStream = *shader.pixelShader;

				if (vertexShaderStream.eos() || pixelShaderStream.eos())
					continue;

				const auto identifier = fmt::format(
						"{}-{}-0x{:0>8x}",
						obj.getName(),
						program.name,
						shader.flags
				);

				Common::DXBC vertexConverter(*shader.vertexShader);
				Common::DXBC fragmentConverter(*shader.pixelShader);
				ShaderPtr vertexShader = Shader::fromGLSL(
						GL_VERTEX_SHADER,
						vertexConverter.generateGlsl(),
						fmt::format("{}-vert", identifier)
				);
				ShaderPtr fragmentShader = Shader::fromGLSL(
						GL_FRAGMENT_SHADER,
						fragmentConverter.generateGlsl(),
						fmt::format("{}-frag", identifier)
				);

				auto p = std::make_unique<Program>(identifier);
				p->attach(*vertexShader);
				p->attach(*fragmentShader);
				p->link();

				const auto passId = RenderPassId(obj.getName(), shader.flags);
				if (_programs.find(passId) == _programs.end())
					_programs[passId] = std::make_unique<ProgramCollection>();

				_programs[passId]->setProgram(
						program.name,
						std::move(p)
				);
			}
		}
	}
	assert(glGetError() == GL_NO_ERROR);

	const std::regex shaderFile("^[a-z]+-[a-z0-9\\_]+-0x[0-9a-c]+\\.(vert|frag|tesc|tese|comp)\\.(glsl|spv)$");
	std::map<std::tuple<std::string, std::string, uint32_t>, ProgramPtr> programs;
	std::vector<ShaderPtr> shaders;
	for (const auto &item: std::filesystem::directory_iterator(_shaderDirectory)) {
		std::string filename = item.path().filename().string();

		if (!std::regex_match(filename, shaderFile))
			continue;

		spdlog::info("Loading shader {}", filename);

		std::string name, stage, type, propertiesString;
		std::stringstream ss(filename);
		std::getline(ss, name, '-');
		std::getline(ss, stage, '-');
		std::getline(ss, propertiesString, '.');
		std::getline(ss, type, '.');

		const uint32_t properties = std::stoul(propertiesString, nullptr, 16);

		std::ifstream in(item.path());
		std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		GLenum shaderType;
		if (type == "frag")
			shaderType = GL_FRAGMENT_SHADER;
		else if (type == "vert")
			shaderType = GL_VERTEX_SHADER;
		else if (type == "tesc")
			shaderType = GL_TESS_CONTROL_SHADER;
		else if (type == "tese")
			shaderType = GL_TESS_EVALUATION_SHADER;
		else
			throw std::runtime_error("Unknown or unsupported shader");

		auto &shader = shaders.emplace_back(Shader::fromGLSL(
				shaderType,
				source,
				fmt::format("{}-{}-0x{:0>8x}-{}", name, stage, properties, type)
		));

		const auto identifier = std::make_tuple(name, stage, properties);
		if (programs.find(identifier) == programs.end())
			programs[identifier] = std::make_shared<Program>(
					fmt::format("{}-{}-0x{:0>8x}", name, stage, properties)
			);

		programs[identifier]->attach(*shader);
	}

	for (const auto &program: programs) {
		const auto &[name, stage, properties] = program.first;
		spdlog::info("Linking Program {} in stage {} with properties {:0>8x}", name, stage, properties);

		const auto id = RenderPassId(name, properties);

		program.second->link();

		if (_programs.find(id) == _programs.end())
			_programs[id] = std::make_unique<ProgramCollection>();

		_programs[id]->setProgram(stage, program.second);
	}

	for (auto &item: _programs) {
		// If this render pass name already exists, skip it
		if (std::find_if(
				_renderPasses.begin(),
				_renderPasses.end(),
				[&](const auto &v) { return v.id == item.first; })
			!= _renderPasses.end()
				)
			continue;
		_renderPasses.emplace_back(RenderPass(item.first.programName, item.first.properties));
	}
}

bool Renderer::isLoading() const {
	return !_loadingTasks.empty();
}

}
