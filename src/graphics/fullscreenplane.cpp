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

#include <glm/gtc/constants.hpp>
#include <utility>

#include "fullscreenplane.h"
#include "gfxman.h"

namespace Graphics {

FullScreenPlane::FullScreenPlane() {
	GUIElementPart part;

	constexpr float planeVertices[] = {
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
	};

	constexpr uint16_t planeIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	part.position = glm::zero<glm::vec2>();
	part.absoluteSize = glm::zero<glm::vec2>();
	part.relativeSize = glm::one<glm::vec2>();
	part.indicesOffset = 0;
	part.verticesOffset = 0;
	part.indicesCount = 6;
	part.vertices = GfxMan.createBuffer(
		(byte *)&planeVertices,
		16 * sizeof(float),
		kVertexBuffer
	);
	part.indices = GfxMan.createBuffer(
		(byte *)&planeIndices,
		6 * sizeof(uint16_t),
		kIndexBuffer
	);

	std::vector<VertexAttribute> attributes = {
		{kPosition,  kVec2F},
		{kTexCoord0, kVec2F}
	};
	_vao = GfxMan.createAttributeObject("gui", "material", attributes, part.vertices);

	_parts.emplace_back(part);
}

void FullScreenPlane::setTexture(TexturePtr texture) {
	_parts.back().texture = std::move(texture);
}

} // Graphics