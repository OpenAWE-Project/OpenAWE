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
#include <set>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/uuid.h"
#include "src/common/writefile.h"

#include "src/awe/resman.h"
#include "src/awe/objfile.h"

#include "src/graphics/shaderconverter.h"
#include "src/graphics/opengl/renderer.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/vbo.h"

namespace Graphics::OpenGL {

Renderer::Renderer(Graphics::Window &window) : _window(window) {
	_window.makeCurrent();

	// Initialize GLEW
	glewExperimental = true;
	GLenum result = glewInit();
	if (result != GLEW_OK)
		throw std::runtime_error(reinterpret_cast<const char *>(glewGetErrorString(result)));

	// Dump OpenGL Information
	//

	spdlog::info("OpenGL Vendor: {}", glGetString(GL_VENDOR));
	spdlog::info("OpenGL Renderer: {}", glGetString(GL_RENDERER));
	spdlog::info("OpenGL Version: {}", glGetString(GL_VERSION));
	spdlog::info("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
	assert(glGetError() == GL_NO_ERROR);

	GLint maxTextureUnits, maxTextureCoords, maxVertexAttribs, maxUniformLocations, maxPatchVertices;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
	spdlog::info("OpenGL Max Texture Units: {}", maxTextureUnits);
	spdlog::info("OpenGL Max Texture Coordinates: {}", maxTextureCoords);
	spdlog::info("OpenGL Max Vertex Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Uniform Attributes: {}", maxVertexAttribs);
	spdlog::info("OpenGL Max Patch Vertices: {}", maxPatchVertices);

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	spdlog::info("Num OpenGL Extensions: {}", numExtensions);

	std::string extensions;
	spdlog::debug("Found Extensions:");
	for (int i = 0; i < numExtensions; ++i) {
		spdlog::debug("- {}", glGetStringi(GL_EXTENSIONS, i));
	}

	if (!GLEW_EXT_texture_compression_s3tc) {
		throw std::runtime_error("No S3TC extension available");
	}

	if (maxPatchVertices < 4) {
		throw std::runtime_error("Not enough patch vertices available");
	}

	// Make some initial settings
	//

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	assert(glGetError() == GL_NO_ERROR);
	glPointSize(10);
	glLineWidth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_LEQUAL);
	//glShadeModel(GL_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(&Renderer::debugMessageCallback), nullptr);
	assert(glGetError() == GL_NO_ERROR);

	// Read and initialize shaders
	//
	const std::regex objFile("^[a-z0-9]+\\.obj$");
	for (const auto &item : std::filesystem::directory_iterator("../shaders")) {
		std::string filename = item.path().filename().string();
		if (!item.is_regular_file())
			continue;

		if (!std::regex_match(filename, objFile))
			continue;

		AWE::OBJFile obj(ResMan.getResource(item.path()));

		for (auto &program : obj.getPrograms()) {
			Graphics::ShaderConverter vertexConverter(*program.shaders.front().vertexShader);
			Graphics::ShaderConverter pixelConverter(*program.shaders.front().pixelShader);
			Shader vertexShader(GL_VERTEX_SHADER, vertexConverter.convertToGLSL());
			Shader fragmentShader(GL_FRAGMENT_SHADER, pixelConverter.convertToGLSL());

			if (program.name == "albedo_only") {
				vertexShader.compile();
				fragmentShader.compile();
				auto &p = _programs[obj.getName()] = std::make_unique<Program>();
				p->attach(vertexShader);
				p->attach(fragmentShader);
				p->link();
				p->setSymbols(vertexConverter.getSymbols());
				p->setAttributeMappings(vertexConverter.getAttributeMappings());
				p->setSamplerMappings(vertexConverter.getSamplerMappings());
			}
		}

	}
	assert(glGetError() == GL_NO_ERROR);

	const std::regex shaderFile("^[a-z]+\\_(vs|ps)\\.glsl$");
	//const std::regex shaderFile("^[a-z]+\\_(tes|tcs|vs|ps)\\.glsl$");
	std::vector<std::unique_ptr<Shader>> shaders;
	for (const auto &item : std::filesystem::directory_iterator("../shaders")) {
		std::string filename = item.path().filename().string();

		if (!std::regex_match(filename, shaderFile))
			continue;

		spdlog::info("Loading shader {}", filename);

		std::string name, type;
		std::stringstream ss(filename);
		std::getline(ss, name, '_');
		std::getline(ss, type, '.');

		std::ifstream in(item.path());
		std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		GLenum shaderType;
		if (type == "ps")
			shaderType = GL_FRAGMENT_SHADER;
		else if (type == "vs")
			shaderType = GL_VERTEX_SHADER;
		else if (type == "tcs")
			shaderType = GL_TESS_CONTROL_SHADER;
		else if (type == "tes")
			shaderType = GL_TESS_EVALUATION_SHADER;
		else
			throw std::runtime_error("Unknown or unsupported shader");

		shaders.emplace_back(new Shader(shaderType, source));

		if (_programs.find(name) == _programs.end())
			_programs[name] = std::make_unique<Program>();

		shaders.back()->compile();

		_programs[name]->attach(*shaders.back());
	}

	for (auto &item : _programs) {
		spdlog::info("Linking Program {}", item.first);
		item.second->link();
	}

	// Initialize stuff for video playback.
	//

	_programs["video"]->bind();

	// Create a static quad for playing videos
	_videoQuad = std::make_unique<VBO>(GL_ARRAY_BUFFER);

	constexpr float videoQuadVertices[] = {
			-1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 1.0f
	};

	_videoQuad->bufferData((byte *) videoQuadVertices, 24 * sizeof(float));

	_videoQuadAttributes = std::make_unique<VAO>();
	_videoQuadAttributes->bind();
	_videoQuad->bind();

	const auto &videoProgram = _programs["video"];

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		*videoProgram->getAttributeLocation(kPosition),
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		nullptr
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		*videoProgram->getAttributeLocation(kTexCoord0),
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		reinterpret_cast<void *>(2 * sizeof(float))
	);

	glBindVertexArray(0);

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);

	glUseProgram(0);
	assert(glGetError() == GL_NO_ERROR);

	// Initialize Pools
	//

	_texturePool.resize(64);
	glGenTextures(64, _texturePool.data());

	// Check for errors
    assert(glGetError() == GL_NO_ERROR);
}

Renderer::~Renderer() {
	glDeleteTextures(64, _texturePool.data());
}

void Renderer::drawFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawWorld();
	drawVideo();
	drawGUI();

	_window.swap();
}

void Renderer::drawWorld() {
	glm::mat4 p = glm::perspectiveFov(45.0f, 1920.0f, 1080.0f, 1.0f, 10000.0f);
	glm::mat4 v = _camera.getLookAt();
	glm::mat4 vp = p * v;

	for (const auto &model : _models) {
		const glm::mat4 rotation = model->getRotation();
		const glm::vec3 position = model->getPosition();
		const glm::vec3 scale = model->getScale();
		glm::mat4 m = glm::translate(position) * rotation * glm::scale(scale);
		glm::mat4 mvp = vp * m;

		const MeshPtr mesh = model->getMesh();
		const auto partMeshs = mesh->getMeshs();

		for (const auto &partmesh : partMeshs) {
			auto programIter = _programs.find(partmesh.material.getShaderName());
			std::string shaderName = partmesh.material.getShaderName();
			if (programIter == _programs.end())
				shaderName = "standardmaterial";

			std::unique_ptr<Program> &program = _programs[shaderName];
			program->bind();

			_vaos[partmesh.vertexAttributesId]->bind();

			bool noIndices = mesh->getIndices().isNil();
			if (!noIndices)
				_indices[mesh->getIndices()]->bind();

			GLuint textureSlot = 0;
			for (const auto &attribute : partmesh.material.getAttributes()) {
				switch (attribute.type) {
					case Material::kVec1: {
						glm::vec1 value = std::get<glm::vec1>(attribute.data);
						program->setUniform1f(attribute.id, value);
						break;
					}

					case Material::kVec2: {
						glm::vec2 value = std::get<glm::vec2>(attribute.data);
						program->setUniform2f(attribute.id, value);
						break;
					}

					case Material::kVec3: {
						glm::vec3 value = std::get<glm::vec3>(attribute.data);
						program->setUniform3f(attribute.id, value);
						break;
					}

					case Material::kVec4: {
						glm::vec4 value = std::get<glm::vec4>(attribute.data);
						program->setUniform4f(attribute.id, value);
						break;
					}

					case Material::kTexture:
						glActiveTexture(getTextureSlot(textureSlot));
						_textures[std::get<Common::UUID>(attribute.data)]->bind();
						program->setUniformSampler(attribute.id, textureSlot);
						textureSlot += 1;
						break;
				}
				assert(glGetError() == GL_NO_ERROR);
			}

			/*if (shaderName == "standardmaterial") {
				glUniform3fv(program->getUniformLocation("g_sAmbientLight.color"), 1, glm::value_ptr(_ambiance.getAmbientLightColor()));
				glUniform1f(program->getUniformLocation("g_sAmbientLight.intensity"), _ambiance.getAmbientLightIntensity());
			}*/

			program->setUniformMatrix4f("g_mLocalToView", mvp);
			assert(glGetError() == GL_NO_ERROR);

			GLenum type;
			switch (partmesh.renderType) {
				default:
				case Mesh::kTriangles:
					type = GL_TRIANGLES;
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

			if (partmesh.material.getCullMode() != Material::kNone) {
				if (glIsEnabled(GL_CULL_FACE) == GL_FALSE)
					glEnable(GL_CULL_FACE);

				switch (partmesh.material.getCullMode()) {
					case Material::kBack:
						glCullFace(GL_BACK);
						break;
					case Material::kFront:
						glCullFace(GL_FRONT_AND_BACK);
						break;
				}
			} else {
				if (glIsEnabled(GL_CULL_FACE) == GL_TRUE)
					glDisable(GL_CULL_FACE);
			}

			if (partmesh.wireframe) {
				glDisable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			if (noIndices) {
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

			assert(glGetError() == GL_NO_ERROR);
		}
	}
}

void Renderer::drawVideo() {
	// If no video frame is loaded, skip video drawing
	if (_currentVideoFrame.isNil())
		return;

	auto &program = _programs["video"];
	program->bind();

	_videoQuadAttributes->bind();

	glActiveTexture(getTextureSlot(0));
	_textures[_currentVideoFrame]->bind();
	program->setUniformSampler("g_sVideoTexture", 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}

void Renderer::drawGUI() {
	glm::mat4 vp = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1000.0f, 1000.0f);

	std::unique_ptr<Program> &program = _programs["gui"];
	program->bind();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	for (const auto &element : _guiElements) {
		glm::mat4 m = glm::translate(glm::vec3(element->getPosition(), 0.0f));
		_vaos[element->getVertexAttributes()]->bind();

		for (const auto &part : element->getParts()) {
			glm::mat4 m2 = m *
					glm::scale(glm::vec3(part.scale, 1.0f)) *
					glm::translate(glm::vec3(part.position, 1.0f))
			;
			_indices[part.indices]->bind();

			glActiveTexture(getTextureSlot(0));
			_textures[part.texture]->bind();
			program->setUniformSampler("g_sTexture", 0);
			program->setUniformMatrix4f("g_mMVP", vp * m2);

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
}

Common::UUID Renderer::registerVertices(byte *data, size_t length) {
	Common::UUID id = Common::UUID::generateRandom();

	_vertices[id] = std::make_unique<VBO>(GL_ARRAY_BUFFER);
	_vertices[id]->bufferData(data, length);

	Common::WriteFile writeFile("vertex.dat");
	unsigned int bufferSize = _vertices[id]->getBufferSize();
	writeFile.write(_vertices[id]->map(), bufferSize);
	writeFile.close();
	_vertices[id]->unmap();

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return id;
}

Common::UUID Renderer::registerIndices(byte *data, size_t length) {
	Common::UUID id = Common::UUID::generateRandom();

	_indices[id] = std::make_unique<VBO>(GL_ELEMENT_ARRAY_BUFFER);
	_indices[id]->bufferData(data, length);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return id;
}

Common::UUID
Renderer::registerVertexAttributes(const std::string &shader, const std::vector<VertexAttribute> &vertexAttributes,
								   Common::UUID vertexData) {
	Common::UUID id = Common::UUID::generateRandom();

	const auto& vao = _vaos[id] = std::make_unique<VAO>();
	vao->bind();
	const auto& vbo = _vertices[vertexData];
	vbo->bind();

	auto programIter = _programs.find(shader);
	std::string shaderName = shader;
	if (programIter == _programs.end())
		shaderName = "standardmaterial";

	std::unique_ptr<Program> &program = _programs[shaderName];
	program->bind();
	//const std::unique_ptr<Program> &program = _programs[shader];
	//program->bind();

	// Calculate the size of the vertex element.
	GLuint stride = 0;
	for (const auto &attribute : vertexAttributes) {
		switch (attribute.dataType) {
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
		}
	}

	GLuint offset = 0;
	for (int i = 0; i < vertexAttributes.size(); ++i) {
		const auto vertexAttribute = vertexAttributes[i];
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
				type = GL_UNSIGNED_SHORT;
				size = 2;
				totalSize = 4;
				break;
			case kVec4S:
				type = GL_UNSIGNED_SHORT;
				size = 4;
				totalSize = 8;
				break;
			case kVec4BI:
			case kVec4BF:
				type = GL_UNSIGNED_BYTE;
				size = 4;
				totalSize = 4;
				break;
		}

		const auto index = program->getAttributeLocation(vertexAttribute.component);
		if (index) {
			glEnableVertexAttribArray(*index);
			glVertexAttribPointer(
					*index,
					size,
					type,
					GL_FALSE,
					stride,
					reinterpret_cast<const void *>(offset)
			);
		}

		offset += totalSize;
	}

	program->validate();

	glBindVertexArray(0);

	return id;
}

Common::UUID Renderer::registerTexture(const ImageDecoder &decoder) {
	Common::UUID id = Common::UUID::generateRandom();

	if (_texturePool.empty()) {
		_texturePool.resize(32);
		glGenTextures(32, _texturePool.data());
	}

	GLuint textureId = _texturePool.back();
	//assert(glIsTexture(textureId) == GL_TRUE);
	_textures[id] = std::make_unique<Texture>(decoder, textureId);

	_texturePoolAssignments[id] = textureId;
	_texturePool.pop_back();

	return id;
}

void Renderer::deregisterTexture(const Common::UUID &id) {
	if (id.isNil())
		return;

	_textures.erase(id);

	GLuint textureId = _texturePoolAssignments[id];
	_texturePoolAssignments.erase(id);
	_texturePool.push_back(textureId);
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
		case 15: return GL_TEXTURE15;
		case 16: return GL_TEXTURE16;
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

}
