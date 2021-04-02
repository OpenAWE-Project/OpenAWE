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

#ifndef AWE_GUIELEMENT_H
#define AWE_GUIELEMENT_H

#include <vector>

#include <glm/glm.hpp>

#include "src/common/uuid.h"

namespace Graphics {

class GUIElement {
public:
	struct GUIElementPart {
		glm::vec2 position, scale;
		unsigned short indicesCount, indicesOffset, verticesOffset;
		Common::UUID vertices, indices, texture;
	};

	GUIElement();
	~GUIElement();


	Common::UUID getVertexAttributes() const;
	[[nodiscard]] const std::vector<GUIElementPart> &getParts() const;

	void show();

	glm::vec2 getPosition();

protected:
	Common::UUID _vao;

	std::vector<GUIElementPart> _parts;

private:
	glm::vec2 _position;
};

}

#endif //AWE_GUIELEMENT_H
