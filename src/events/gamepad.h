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

#ifndef OPENAWE_GAMEPAD_H
#define OPENAWE_GAMEPAD_H

#include "src/events/mouse.h"

namespace Events {

/*!
 * An enum representing gamepad buttons. The list is mirrored with the macros from glfw.
 * For info on glfw gamepad button macros see https://www.glfw.org/docs/latest/group__gamepad__buttons.html 
 */
enum GamepadButton {
    kGamepadButtonA,
    kGamepadButtonB,
    kGamepadButtonX,
    kGamepadButtonY,
    kGamepadButtonLeftBumper,
    kGamepadButtonRightBumper,
    kGamepadButtonBack,
    kGamepadButtonStart,
    kGamepadButtonGuide,
    kGamepadButtonLeftThumb,
    kGamepadButtonRightThumb,
    kGamepadButtonDPadUp,
    kGamepadButtonDPadRight,
    kGamepadButtonDPadDown,
    kGamepadButtonDPadLeft,
    kGamepadButtonLast = kGamepadButtonDPadLeft,
    kGamepadButtonCross = kGamepadButtonA,
    kGamepadButtonCircle = kGamepadButtonB,
    kGamepadButtonSquare = kGamepadButtonX,
    kGamepadButtonTriangle = kGamepadButtonY
};

/*
 * Axis values follow the underlying logic:
 * - If first two bits of a value are both zeroes - we're dealing with a 2D axis
 * - If the first bit is one - we use a horizontal 1D part of a 2D axis
 * - If the second bit is one - we use a vertical 1D part of a 2D axis
 */

enum Gamepad2DAxis {
    kGamepadAxisLeft = 4,
    kGamepadAxisRight = 8
};

enum Gamepad1DAxis {
    kGamepadAxisLeftX = kGamepadAxisLeft | kSliceHorizontal,
    kGamepadAxisLeftY = kGamepadAxisLeft | kSliceVertical,
    kGamepadAxisRightX = kGamepadAxisRight | kSliceHorizontal,
    kGamepadAxisRightY = kGamepadAxisRight | kSliceVertical,
    kGamepadAxisLeftTrigger = 0,
    kGamepadAxisRightTrigger = 1
};

} // End of namespace Events

#endif //OPENAWE_GAMEPAD_H