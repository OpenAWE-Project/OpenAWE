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

#ifndef OPENAWE_IMGUIELEMENT_H
#define OPENAWE_IMGUIELEMENT_H

namespace Graphics {

/*!
 * \brief Base class for imgui elements
 *
 * This class represents an abstract immediate mode gui element based on the imgui library. Elements need to implement
 * the draw function which will be called by the renderer.
 */
class ImGuiElement {
public:
	~ImGuiElement();

	/**
	 * Make the ImGui Element visible or not
	 * \param visible If the imgui element should be visible or not
	 */
	void setVisible(bool visible);

	/*!
	 * Function which will be called when the window should be rendered
	 */
	virtual void draw() = 0;
};

} // End of namespace Graphics

#endif //OPENAWE_IMGUIELEMENT_H
