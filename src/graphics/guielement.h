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

#include "src/graphics/buffer.h"
#include "src/graphics/texture.h"
#include "src/graphics/attributeobject.h"

namespace Graphics {

/*!
 * \brief Base class for user interface elements
 *
 * This class is the base for all elements of the user interface. It can contain multiple parts, which are defined by
 * their vertices, indices, a texture, and an absolute and relative Size.
 *
 * The element is placed by a relative and an absolute position. The resulting position of both is calculated as
 * relativePosition * screenResolution + absolutePosition.
 */
class GUIElement {
public:
	struct GUIElementPart {
		glm::vec2 position{0}, absoluteSize{0}, relativeSize{0};
		unsigned short indicesCount{0}, indicesOffset{0}, verticesOffset{0};
		BufferPtr vertices, indices;
		TexturePtr texture;
	};

	GUIElement();
	~GUIElement();


	AttributeObjectPtr getVertexAttributes() const;
	[[nodiscard]] const std::vector<GUIElementPart> &getParts() const;

	void show();
	void hide();

	/*!
	 * Return the relative position of this gui element in a range of [0, 1]
	 * \return The relative position of this gui element
	 */
	const glm::vec2 & getRelativePosition() const;

	/*!
	 * Return the absolute offset from the position in pixels
	 * \return The absolute pixel offset of this gui element
	 */
	const glm::vec2 & getAbsolutePosition() const;

	/*!
	 * Set the relative positioning of the gui element
	 * \param relativePosition The new relative position to set
	 */
	void setRelativePosition(const glm::vec2 &relativePosition);

	/*!
	 * Set the absolute pixel offset of the gui element
	 * @param absolutePosition The new absolute pixel offset to set
	 */
	void setAbsolutePosition(const glm::vec2 &absolutePosition);

protected:
	AttributeObjectPtr _vao;

	std::vector<GUIElementPart> _parts;

private:
	glm::vec2 _relativePosition;
	glm::vec2 _absolutePosition;
};

}

#endif //AWE_GUIELEMENT_H
