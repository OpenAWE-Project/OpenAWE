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

#ifndef AWE_RENDERER_H
#define AWE_RENDERER_H

#include <vector>

#include "src/common/frustrum.h"

#include "src/graphics/model.h"
#include "src/graphics/camera.h"
#include "src/graphics/vertexattribute.h"
#include "src/graphics/guielement.h"
#include "src/graphics/ambiancestate.h"
#include "src/graphics/texture.h"
#include "src/graphics/buffer.h"
#include "src/graphics/light.h"
#include "src/graphics/attributeobject.h"
#include "src/graphics/images/decoder.h"
#include "src/graphics/sky.h"
#include "src/graphics/imguielement.h"


namespace Graphics {

class Renderer {
public:
	Renderer();
	virtual ~Renderer();

	void addModel(Model *model);
	void removeModel(Model *model);
	void addImGuiElement(ImGuiElement *imGuiElement);
	void removeImGuiElement(ImGuiElement *imGuiElement);
	void addGUIElement(GUIElement *gui);
	void removeGUIElement(GUIElement *gui);
	void addLight(Light *light);
	void removeLight(Light *light);

	void setCamera(Camera &camera);

	void setAmbianceState(const AmbianceState ambiance);

	/*!
	 * Set the sky lut from an atmosphere file for the renderer
	 *
	 * \param lut The lut texture to set for the renderer
	 */
	void setSkyLUT(TexturePtr lut);

	virtual bool isLoading() const = 0;

	/*!
	 * Set a sky as sky for the rendering
	 *
	 * \param sky The sky to set for rndering
	 */
	void setSky(SkyPtr sky);

    /*!
     * Create a texture by the specified type with the given label
     * @param type The type of the texture
     * @param label The label of the texture
     * @return The neqly created shared texture pointer
     */
	virtual TexturePtr createTexture(
        TextureType type,
        const std::string &label
    ) = 0;
	virtual ProxyTexturePtr createProxyTexture() = 0;
	virtual BufferPtr createBuffer(BufferType type, bool modifiable) = 0;
	virtual AttributeObjectPtr createAttributeObject(
		const std::string &shader,
		const std::string &stage,
		uint32_t properties,
		const std::vector<VertexAttribute> &vertexAttributes,
		BufferPtr vertexData,
		unsigned int offset,
		const std::string &label
	) = 0;

	virtual int getUniformIndex(
		const std::string &shaderName,
		const std::string &stage,
		uint32_t properties,
		const std::string &id
	) = 0;

	/*!
	 * Execute tasks for the renderer which are not drawing related
	 */
	virtual void update();
	virtual void drawFrame() = 0;

protected:
	struct RenderTask {
		Model *model;
		std::vector<unsigned int> partMeshsToRender;
	};

	struct RenderPassId {
		std::string programName;
		uint32_t properties;

		bool operator<(const RenderPassId &rhs) const {
			return std::tie(programName, properties) < std::tie(rhs.programName, rhs.properties);
		}

		bool operator==(const RenderPassId &rhs) const {
			return std::tie(programName, properties) == std::tie(rhs.programName, rhs.properties);
		}

		RenderPassId(const std::string &programName, uint32_t properties) :
			programName(programName),
			properties(properties) {
		}
	};

	struct RenderPass {
		RenderPassId id;
		std::vector<RenderTask> renderTasks;

		RenderPass(const std::string &programName, uint32_t properties) : id(programName, properties) {
		}
	};

	float _fov;
	glm::mat4 _view;
	glm::mat4 _projection;

	SkyPtr _sky;

	std::optional<std::reference_wrapper<Camera>> _camera;
	AmbianceState _ambiance;

	TexturePtr _skyLUT;

	Common::Frustrum _frustrum;

	std::vector<RenderPass> _renderPasses;
	std::vector<ImGuiElement *> _imguiElements;
	std::vector<GUIElement *> _guiElements;
	std::vector<Light *> _lights;
};

} // End of namespace Graphics

#endif //AWE_RENDERER_H
