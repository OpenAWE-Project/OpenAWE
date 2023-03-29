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

#ifndef OPENAWE_MOUSE_H
#define OPENAWE_MOUSE_H

namespace Events {

/*!
 * An enum representing mouse buttons. The list is mirrored with the macros from glfw.
 * For info on glfw mouse button macros see https://www.glfw.org/docs/latest/group__buttons.html 
 */
enum MouseButton {
    kMouseButton1,
    kMouseButton2,
    kMouseButton3,
    kMouseButton4,
    kMouseButton5,
    kMouseButton6,
    kMouseButton7,
    kMouseButton8,
    kMouseLeft = kMouseButton1,
    kMouseRight = kMouseButton2,
    kMouseMiddle = kMouseButton3,
    kMouseLast = kMouseButton8
};

/*
 * Axis values follow the underlying logic:
 * - If first two bits of a value are both zeroes - we're dealing with a 2D axis
 * - If the first bit is one - we use a horizontal 1D part of a 2D axis
 * - If the second bit is one - we use a vertical 1D part of a 2D axis
 */

enum Mouse2DAxis {
    kMousePosition = 4,
    kMouseScroll = 8
};

enum AxisConverter {
    kSliceHorizontal = 1,
    kSliceVertical = 2
};

enum Mouse1DAxis {
    kMousePositionHorizontal = kMousePosition | kSliceHorizontal,
    kMousePositionVertical = kMousePosition | kSliceVertical,
    kMouseScrollHorizontal = kMouseScroll | kSliceHorizontal,
    kMouseScrollVertical = kMouseScroll | kSliceVertical
};

} // End of namespace Events

#endif //OPENAWE_MOUSE_H
