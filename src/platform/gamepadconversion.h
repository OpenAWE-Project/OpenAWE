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

#ifndef OPENAWE_GAMEPADCONVERSION_H
#define OPENAWE_GAMEPADCONVERSION_H

#include "src/events/gamepad.h"

namespace Platform {
/*!
 * Convert a GLFW gamepad stick (combined from two separate axes) 
 * to an OpenAWE stick enum.
 * 
 * \param stick The GLFW stick to convert.
 */
Events::Gamepad2DAxis convertGLFW2GamepadStick(int stick);

/*!
 * Convert a GLFW gamepad trigger to an OpenAWE trigger enum.
 * 
 * \param trigger The GLFW trigger to convert.
 */
Events::Gamepad1DAxis convertGLFW2GamepadTrigger(int trigger);

} // End of namespace Platform

#endif //OPENAWE_KEYCONVERSION_H
