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
    kGamepadButtonLast = kGamepadButtonDPadLeft
};

/*!
 * An enum representing the one dimensional trigger axis on gamepads
 */
enum Gamepad1DAxis {
	kGamepadAxisLeftTrigger,
	kGamepadAxisRightTrigger
};

/*!
 * An enum representing the two dimensional stick axis on gamepads
 */
enum Gamepad2DAxis {
    kGamepadAxisLeft,
    kGamepadAxisRight
};

} // End of namespace Events

#endif //OPENAWE_GAMEPAD_H