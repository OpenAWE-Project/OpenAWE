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

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/uuid.h"
#include "src/common/writefile.h"
#include "src/common/exception.h"
#include "src/common/strutil.h"

#include "src/awe/resman.h"
#include "src/awe/objfile.h"

#include "src/graphics/shaderconverter.h"
#include "src/graphics/skeleton.h"
#include "src/graphics/opengl/renderer.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/vbo.h"
#include "src/graphics/opengl/convertedprogram.h"
#include "src/graphics/opengl/proxytexture.h"

namespace Graphics::OpenGL {

Renderer::Renderer(Platform::Window &window, const std::string &shaderDirectory) : _window(window) {
	_window.makeCurrent();

	// Initialize GLEW
	glewExperimental = false; // TODO: Make a config argument out of it
	GLenum result = glewInit();
	if (result != GLEW_OK)
		throw std::runtime_error(reinterpret_cast<const char *>(glewGetErrorString(result)));

	// Dump OpenGL Information
	//

	const std::string vendor       = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const std::string renderer     = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const std::string version      = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const std::string glslVersion  = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	spdlog::info("OpenGL Vendor: {}", vendor);
	spdlog::info("OpenGL Renderer: {}", renderer);
	spdlog::info("OpenGL Version: {}", version);
	spdlog::info("GLSL Version: {}", glslVersion);
	assert(glGetError() == GL_NO_ERROR);

	GLint maxTextureUnits, maxTextureCoords, maxVertexAttribs, maxUniformLocations, maxVertexUniformComponents, maxFragmentUniformComponents;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
	spdlog::info("OpenGL Max Texture Units: {}", maxTextureUnits);
	spdlog::info("OpenGL Max Texture Coordinates: {}", maxTextureCoords);
	spdlog::info("OpenGL Max Vertex Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Uniform Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Vertex Uniform Components: {}", maxVertexUniformComponents);
	spdlog::info("OpenGL Max Fragment Uniform Components: {}", maxFragmentUniformComponents);

	if (GLEW_ARB_uniform_buffer_object) {
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

	if (GLEW_ARB_tessellation_shader) {
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

	if (!GLEW_EXT_texture_compression_s3tc) {
		throw std::runtime_error("No S3TC extension available");
	}

	_hasDebug = GLEW_KHR_debug;
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

	if (GLEW_ARB_debug_output) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallbackARB(reinterpret_cast<GLDEBUGPROC>(&Renderer::debugMessageCallback), nullptr);
	}

	// Read and initialize shaders
	//
	const std::regex objFile("^[a-z0-9]+\\.obj$");
	for (const auto &item : std::filesystem::directory_iterator(shaderDirectory)) {
		std::string filename = item.path().filename().string();
		if (!item.is_regular_file())
			continue;

		if (!std::regex_match(Common::toLower(filename), objFile))
			continue;

		AWE::OBJFile obj(ResMan.getResource(item.path().string()));

		for (auto &program : obj.getPrograms()) {
			const auto &vertexShaderStream = *program.shaders.front().vertexShader;
			const auto &pixelShaderStream = *program.shaders.front().pixelShader;

			if (vertexShaderStream.eos() || pixelShaderStream.eos())
				continue;

			Graphics::ShaderConverter vertexConverter(*program.shaders.front().vertexShader);
			Graphics::ShaderConverter pixelConverter(*program.shaders.front().pixelShader);
			ShaderPtr vertexShader   = Shader::fromGLSL(GL_VERTEX_SHADER, vertexConverter.convertToGLSL());
			ShaderPtr fragmentShader = Shader::fromGLSL(GL_FRAGMENT_SHADER, pixelConverter.convertToGLSL());

			if (program.name == "albedo_only") {
				auto p = std::make_unique<ConvertedProgram>();
				p->attach(*vertexShader);
				p->attach(*fragmentShader);
				p->link();
				p->setSymbols(vertexConverter.getSymbols());
				p->setAttributeMappings(vertexConverter.getAttributeMappings());
				p->addSamplerMappings(vertexConverter.getSamplerMappings());
				p->addSamplerMappings(pixelConverter.getSamplerMappings());

				_programs[RenderPassId(obj.getName(), program.shaders.front().flags)]->setProgram("material", std::move(p));
			}
		}

	}
	assert(glGetError() == GL_NO_ERROR);

	const std::regex shaderFile("^[a-z]+-[a-z\\_]+-0x[0-9a-c]+\\.(vert|frag|tesc|tese)\\.(glsl|spv)$");
	std::map<std::tuple<std::string, std::string, uint32_t>, ProgramPtr> programs;
	std::vector<ShaderPtr> shaders;
	for (const auto &item : std::filesystem::directory_iterator(shaderDirectory)) {
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
		spdlog::info("Linking Program {} in stage {} with properties {:x}", name, stage, properties);

		const auto id = RenderPassId(name, properties);

		program.second->validate();
		program.second->link();

		if (_programs.find(id) == _programs.end())
			_programs[id] = std::make_unique<ProgramCollection>();

		_programs[id]->setProgram(stage, program.second);
	}

	for (auto &item : _programs) {
		// If this render pass name already exists, skip it
		if (std::find_if(
			_renderPasses.begin(),
			_renderPasses.end(),
			[&](const auto &v){ return v.id == item.first; })
			!= _renderPasses.end()
		)
			continue;
		_renderPasses.emplace_back(RenderPass(item.first.programName, item.first.properties));
	}

	// Get width and height of the default framebuffer
	unsigned int width, height;
	window.getSize(width, height);

	// Initialize deferred shading
	//
	_depthTexture = std::make_unique<Texture>(width, height);
	_normalTexture = std::make_unique<Texture>(width, height);

	_deferredBuffer = std::make_unique<Framebuffer>("Deferred Buffer");
	_deferredBuffer->bind();

	_deferredBuffer->attachRenderBuffer(width, height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);

	_deferredBuffer->attachTexture(*_depthTexture, GL_COLOR_ATTACHMENT0);
	_deferredBuffer->attachTexture(*_normalTexture, GL_COLOR_ATTACHMENT1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Check for errors
    assert(glGetError() == GL_NO_ERROR);
}

Renderer::~Renderer() {
}

void Renderer::drawFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawWorld("material");
	drawGUI();

	_window.swap();
}

void Renderer::drawWorld(const std::string &stage) {
	glm::mat4 vp = _projection * _view;
	glm::mat4 viewToWorldMat = glm::inverse(_view);

	std::vector<glm::mat4x3> placeholderMatrices(64);
	std::fill(placeholderMatrices.begin(), placeholderMatrices.end(), glm::identity<glm::mat4x3>());

	const auto &defaultShader = getProgram("standardmaterial", stage, 0);
	static const glm::mat4 mirrorZ = glm::scale(glm::vec3(1, 1, -1));

	bool wireframe = false;
	Material::CullMode cullMode = Material::kNone;
	
	pushDebug(fmt::format("Draw stage {}", stage));

	for (const auto &pass: _renderPasses) {
		if (pass.renderTasks.empty())
			continue;

		pushDebug(fmt::format("Pass {} {:0<8x}", pass.id.programName, pass.id.properties));

		const ProgramPtr &currentShader = (!hasProgram(pass.id.programName, stage, pass.id.properties)) ? defaultShader : getProgram(pass.id.programName, stage, pass.id.properties);
		currentShader->bind();

		const std::optional<GLint> localToView = currentShader->getUniformLocation("g_mLocalToView");
		const std::optional<GLint> localToClip = currentShader->getUniformLocation("g_mLocalToClip");
		const std::optional<GLint> viewToClip = currentShader->getUniformLocation("g_mViewToClip");
		const std::optional<GLint> viewToWorld = currentShader->getUniformLocation("g_mViewToWorld");

		const std::optional<GLint> skinningMatrices = currentShader->getUniformLocation("GPU_skinning_matrices");

		for (const auto &task: pass.renderTasks) {
			glm::mat4 m = mirrorZ * task.model->getTransform();
			glm::mat4 mv = _view * m;
			glm::mat4 vm = task.model->getInverseTransform() * viewToWorldMat;
			glm::mat4 mvp = vp * m;

			const MeshPtr mesh = task.model->getMesh();
			if (mesh->hasBoundingSphere()) {
				auto boundSphere = mesh->getBoundingSphere();
				boundSphere.position = m * glm::vec4(boundSphere.position, 1.0f);
				if (!_frustrum.test(boundSphere))
					continue;
			}

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

			for (const auto &meshToRender: task.partMeshsToRender) {
				const auto &partmesh = partMeshs[meshToRender];

				if (!partmesh.material.hasStage(stage))
					continue;

				std::static_pointer_cast<Graphics::OpenGL::VAO>(partmesh.vertexAttributes.find(stage)->second)->bind();
				if (indices)
					indices->bind();

				GLuint textureSlot = 0;
				for (const auto &attribute : partmesh.material.getAttributes(stage)) {
					switch (attribute.type) {
						case Material::kVec1: {
							glm::vec1 value = std::get<glm::vec1>(attribute.data);
							currentShader->setUniform1f(attribute.index, value);
							break;
						}

						case Material::kVec2: {
							glm::vec2 value = std::get<glm::vec2>(attribute.data);
							currentShader->setUniform2f(attribute.index, value);
							break;
						}

						case Material::kVec3: {
							glm::vec3 value = std::get<glm::vec3>(attribute.data);
							currentShader->setUniform3f(attribute.index, value);
							break;
						}

						case Material::kVec4: {
							glm::vec4 value = std::get<glm::vec4>(attribute.data);
							currentShader->setUniform4f(attribute.index, value);
							break;
						}

						case Material::kTexture:
							glActiveTexture(getTextureSlot(textureSlot));
							std::dynamic_pointer_cast<Graphics::OpenGL::GLTexture>(std::get<TexturePtr>(attribute.data))->bind();
							currentShader->setUniformSampler(attribute.index, textureSlot);
							textureSlot += 1;
							break;
					}
					//assert(glGetError() == GL_NO_ERROR);
				}

				if (task.model->hasSkeleton() && skinningMatrices) {
					const auto matrices =	task.model->getSkeleton().getSkinningMatrices(partmesh.boneMap);
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

		popDebug(); // Pop Pass Message
	}

	popDebug(); // Pop Stage Message
}

void Renderer::drawGUI() {
	glm::mat4 vp = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1000.0f, 1000.0f);

	pushDebug("Draw GUI");

	auto program = getProgram("gui", "material", 0);
	program->bind();

	const GLint locationTexture = *program->getUniformLocation("g_sTexture");
	const GLint locationMVP     = *program->getUniformLocation("g_mMVP");

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	for (const auto &element : _guiElements) {
		glm::mat4 m = glm::translate(glm::vec3(
			element->getAbsolutePosition() +
			element->getRelativePosition() * glm::vec2(1920.0, 1080.0),
			0.0f
		));
		std::static_pointer_cast<Graphics::OpenGL::VAO>(element->getVertexAttributes())->bind();

		for (const auto &part : element->getParts()) {
			glm::mat4 m2 = m *
				glm::scale(glm::vec3(part.absoluteSize + part.relativeSize * glm::vec2(1920, 1080), 1.0f)) *
				glm::translate(glm::vec3(part.position, 1.0f))
			;
			std::static_pointer_cast<Graphics::OpenGL::VBO>(part.indices)->bind();

			glActiveTexture(getTextureSlot(0));
			std::dynamic_pointer_cast<Graphics::OpenGL::GLTexture>(part.texture)->bind();
			program->setUniformSampler(locationTexture, 0);
			program->setUniformMatrix4f(locationMVP, vp * m2);

			glDrawElementsBaseVertex(
					GL_TRIANGLES,
					part.indicesCount,
					GL_UNSIGNED_SHORT,
					reinterpret_cast<void*>(part.indicesOffset),
					part.verticesOffset
			);
		}
	}

	glEnable(GL_DEPTH_TEST);

	popDebug(); // Pop Draw GUI Message
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
	const auto &programCollection = _programs[RenderPassId(name, property)];
	return programCollection->getProgram(stage);
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
		case 0: return GL_TEXTURE0;
		case 1: return GL_TEXTURE1;
		case 2: return GL_TEXTURE2;
		case 3: return GL_TEXTURE3;
		case 4: return GL_TEXTURE4;
		case 5: return GL_TEXTURE5;
		case 6: return GL_TEXTURE6;
		case 7: return GL_TEXTURE7;
		case 8: return GL_TEXTURE8;
		case 9: return GL_TEXTURE9;
		case 10: return GL_TEXTURE10;
		case 11: return GL_TEXTURE11;
		case 12: return GL_TEXTURE12;
		case 13: return GL_TEXTURE13;
		case 14: return GL_TEXTURE14;
		case 17: return GL_TEXTURE17;
		case 18: return GL_TEXTURE18;
		case 19: return GL_TEXTURE19;
		case 20: return GL_TEXTURE20;
		case 21: return GL_TEXTURE21;
		case 22: return GL_TEXTURE22;
		case 23: return GL_TEXTURE23;
		case 24: return GL_TEXTURE24;
		case 25: return GL_TEXTURE25;
		case 26: return GL_TEXTURE26;
		case 27: return GL_TEXTURE27;
		case 28: return GL_TEXTURE28;
		case 29: return GL_TEXTURE29;
		case 30: return GL_TEXTURE30;
		case 31: return GL_TEXTURE31;
	}
}

void Renderer::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
									const GLchar *message, void *userParam) {
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			spdlog::error(message);
			return;
		default:
			switch (severity) {
				case GL_DEBUG_SEVERITY_HIGH:
					spdlog::info(message);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					spdlog::debug(message);
					break;
                default:
				case GL_DEBUG_SEVERITY_LOW:
					spdlog::trace(message);
					break;
			}
			return;
	}
}

ProxyTexturePtr Renderer::createProxyTexture() {
	return std::make_shared<Graphics::OpenGL::ProxyTexture>();
}

TexturePtr Renderer::createTexture(TextureType type) {
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

	return std::make_shared<Graphics::OpenGL::Texture>(texType);
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

	return std::make_shared<Graphics::OpenGL::VBO>(bufferType, usage);
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
	unsigned int offset
) {
	auto vao = std::make_unique<VAO>();
	vao->bind();
	std::static_pointer_cast<Graphics::OpenGL::VBO>(vertexData)->bind();

	std::string shaderName = shader;
	if (!hasProgram(shaderName, stage, properties))
		shaderName = "standardmaterial";

	ProgramPtr program = getProgram(shaderName, stage, properties);
	program->bind();

	// Calculate the size of the vertex element.
	GLuint stride = 0;
	for (const auto &attribute : vertexAttributes) {
		switch (attribute.dataType) {
			case kUByte:
				stride += 1;
				break;
			case kVec4BF:
			case kVec4BI:
			case kVec2S:
				stride += 4;
				break;
			case kVec4S:
			case kVec2F:
				stride += 8;
				break;
			case kVec3F:
				stride += 12;
				break;
			case kVec4F:
				stride += 16;
				break;
			default:
				throw CreateException("Invalid vertex attribute type");
		}
	}

	GLuint localOffset = 0;
	for (unsigned int i = 0; i < vertexAttributes.size(); ++i) {
		const auto vertexAttribute = vertexAttributes[i];
		bool integer = false;
		GLuint size = 0, totalSize = 0;
		GLenum type = 0;

		switch (vertexAttribute.dataType) {
			case kVec2F:
				type = GL_FLOAT;
				size = 2;
				totalSize = 8;
				break;
			case kVec3F:
				type = GL_FLOAT;
				size = 3;
				totalSize = 12;
				break;
			case kVec4F:
				type = GL_FLOAT;
				size = 4;
				totalSize = 16;
				break;
			case kVec2S:
				type = GL_SHORT;
				size = 2;
				totalSize = 4;
				break;
			case kVec4S:
				type = GL_SHORT;
				size = 4;
				totalSize = 8;
				break;
			case kVec4BI:
				integer = true;
			case kVec4BF:
				type = GL_UNSIGNED_BYTE;
				size = 4;
				totalSize = 4;
				break;
			case kUByte:
				type = GL_UNSIGNED_BYTE;
				size = 1;
				totalSize = 1;
				integer = true;
				break;
			default:
				throw CreateException("Invalid vertex attribute type");
		}

		const auto index = program->getAttributeLocation(vertexAttribute.component);
		if (index) {
			glEnableVertexAttribArray(*index);
			if (integer) {
				glVertexAttribIPointer(
						*index,
						size,
						type,
						stride,
						reinterpret_cast<const void *>(localOffset + offset)
				);
			} else {
				glVertexAttribPointer(
						*index,
						size,
						type,
						GL_FALSE,
						stride,
						reinterpret_cast<const void *>(localOffset + offset)
				);
			}
		}

		localOffset += totalSize;
	}

	program->validate();

	glBindVertexArray(0);

	return vao;
}

}
