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
#include <src/graphics/images/decoder.h>

#include "src/graphics/model.h"
#include "src/graphics/camera.h"
#include "src/graphics/vertexattribute.h"
#include "src/graphics/guielement.h"
#include "src/graphics/ambiancestate.h"
#include "src/graphics/texture.h"
#include "src/graphics/buffer.h"
#include "attributeobject.h"


namespace Graphics {

class Renderer {
public:
	Renderer();
	virtual ~Renderer();

	void addModel(Model *model);
	void removeModel(Model *model);
	void addGUIElement(GUIElement *gui);

	void setCamera(Camera &camera);

	void setAmbianceState(const AmbianceState ambiance);

	virtual TexturePtr createTexture(TextureType type) = 0;
	virtual BufferPtr createBuffer(BufferType type, bool modifiable) = 0;
	virtual AttributeObjectPtr createAttributeObject(
		const std::string &shader,
		const std::vector<VertexAttribute> &vertexAttributes,
		BufferPtr vertexData,
		unsigned int offset
	) = 0;

	virtual int getUniformIndex(const std::string &shaderName, const std::string &id) = 0;

	void setCurrentVideoFrame(const Common::UUID &id);

	void update();
	virtual void drawFrame() = 0;

protected:
	struct RenderTask {
		Model *model;
		std::vector<unsigned int> partMeshsToRender;
	};

	struct RenderPass {
		std::string programName;
		std::vector<RenderTask> renderTasks;

		RenderPass(const std::string &programName) : programName(programName) {
		}
	};

	glm::mat4 _view;
	glm::mat4 _projection;

	std::optional<std::reference_wrapper<Camera>> _camera;
	AmbianceState _ambiance;

	Common::UUID _currentVideoFrame;

	std::vector<RenderPass> _renderPasses;
	std::vector<GUIElement *> _guiElements;
};

} // End of namespace Graphics

#endif //AWE_RENDERER_H
