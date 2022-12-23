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

#include "src/graphics/gfxman.h"
#include "guielement.h"

namespace Graphics {

GUIElement::GUIElement() : _relativePosition(0.0, 0.0), _absolutePosition(0.0, 0.0) {
}

GUIElement::~GUIElement() {
}

AttributeObjectPtr GUIElement::getVertexAttributes() const {
	return _vao;
}

const std::vector<GUIElement::GUIElementPart> &GUIElement::getParts() const {
	return _parts;
}

const glm::vec2 & GUIElement::getRelativePosition() const {
	return _relativePosition;
}

const glm::vec2 & GUIElement::getAbsolutePosition() const {
	return _absolutePosition;
}

void GUIElement::setRelativePosition(const glm::vec2 &relativePosition) {
	_relativePosition = relativePosition;
}

void GUIElement::setAbsolutePosition(const glm::vec2 &absolutePosition) {
	_absolutePosition = absolutePosition;
}

void GUIElement::show() {
	GfxMan.addGUIElement(this);
}

void GUIElement::hide() {
	GfxMan.removeGUIElement(this);
}

}