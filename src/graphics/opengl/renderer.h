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

#ifndef AWE_OPENGL_RENDERER_H
#define AWE_OPENGL_RENDERER_H

#include <list>
#include <map>
#include <memory>

#include "src/platform/window.h"

#include "src/graphics/renderer.h"
#include "src/graphics/opengl/program.h"
#include "src/graphics/opengl/vbo.h"
#include "src/graphics/opengl/vao.h"
#include "src/graphics/opengl/texture.h"
#include "src/graphics/opengl/framebuffer.h"
#include "src/graphics/opengl/programcollection.h"
#include "task.h"

namespace Graphics::OpenGL {

class Renderer : public Graphics::Renderer {
public:
	explicit Renderer(Platform::Window &window, const std::string &shaderDirectory);
	~Renderer();

	void update() override;

	void drawFrame() override;

	bool isLoading() const override;

	TexturePtr createTexture(
            TextureType type,
            const std::string &label
    ) override;

	ProxyTexturePtr createProxyTexture() override;

	BufferPtr createBuffer(BufferType type, bool modifiable) override;

	AttributeObjectPtr
	createAttributeObject(const std::string &shader, const std::string &stage, uint32_t properties,
						  const std::vector<VertexAttribute> &vertexAttributes, BufferPtr vertexData,
						  unsigned int offset, const std::string &label) override;

	int getUniformIndex(const std::string &shaderName, const std::string &stage, uint32_t properties,
						const std::string &id) override;

private:
	void drawWorld(const std::string &stage);
	void drawLights();
	void drawGUI();
	void drawSky();
	void drawImGui();

	void pushDebug(const std::string &message);
	void popDebug();

	ProgramPtr getProgram(const std::string &name, const std::string &stage, const uint32_t property);
	bool hasProgram(const std::string &name, const std::string &stage, uint32_t properties);
	GLenum getTextureSlot(unsigned int slot);

	static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

	/*!
	 * Apply a vector of uniforms to a program
	 * \param program The program to apply the uniforms to
	 * \param uniforms The uniforms to apply to the program
	 * \param textureSlot The current texture slot, gets incremented when new textures are applied
	 */
	void applyUniforms(
		ProgramPtr &program,
		std::vector<Material::Uniform> uniforms,
		GLuint &textureSlot
	);

	/*!
	 * Clear the programs and reread all available shaders
	 */
	void rebuildShaders();

	Platform::Window &_window;

	bool _hasDebug;
	const std::string _shaderDirectory;

	std::unique_ptr<Renderbuffer> _depthstencilBuffer;

	std::unique_ptr<Texture> _lightBufferTexture;
	std::unique_ptr<Framebuffer> _lightBuffer;

	std::unique_ptr<Texture> _depthTexture;
	std::unique_ptr<Texture> _normalTexture;
	std::unique_ptr<Texture> _noiseMap;
	std::unique_ptr<Framebuffer> _deferredBuffer;

	std::map<RenderPassId, std::unique_ptr<ProgramCollection>> _programs;

	TaskQueue _loadingTasks;
};

}

#endif //AWE_OPENGL_RENDERER_H
