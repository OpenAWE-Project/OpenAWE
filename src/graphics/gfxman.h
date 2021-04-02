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

#ifndef AWE_GFXMAN_H
#define AWE_GFXMAN_H

#include <memory>
#include <queue>
#include <future>

#include "src/common/singleton.h"

#include "src/graphics/renderer.h"
#include "src/graphics/window.h"
#include "src/graphics/vertexattribute.h"
#include "src/graphics/camera.h"

namespace Graphics {

class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	void initOpenGL(Window &window);

	Camera getCamera() const;
	void setCamera(const Camera &camera);

	void setAmbianceState(const std::string &id);

	void addModel(Model *model);
	void removeModel(Model *model);

	void addGUIElement(GUIElement *gui);

	Common::UUID registerVertices(byte *data, size_t length, Common::UUID vertexLayout);
	Common::UUID registerIndices(byte* data, size_t length);

	Common::UUID registerTexture(const ImageDecoder &decoder);
	std::future<Common::UUID> registerTextureAsync(const ImageDecoder &decoder);

	void deregisterTexture(Common::UUID &id);
	void deregisterTextureAsync(Common::UUID &id);

	Common::UUID
	registerVertexAttributes(const std::string &shader, const std::vector<VertexAttribute> &vertexAttributes,
							 Common::UUID vertexData);

	void setCurrentVideoFrame(Common::UUID &id);

	void drawFrame();

private:
	struct AsyncTexture {
		const ImageDecoder &decoder;
		std::promise<Common::UUID> promise;

		explicit AsyncTexture(const ImageDecoder &decoder) : decoder(decoder) {
		}
	};

	Camera _camera;

	bool _dirty;

	std::mutex _textureAccess;
	std::vector<AsyncTexture> _texturesToProcess;
	std::vector<Common::UUID> _texturesToDelete;

	std::unique_ptr<Renderer> _renderer;
};

} // End of namespace Graphics

#define GfxMan Graphics::GraphicsManager::instance()

#endif //AWE_GFXMAN_H
