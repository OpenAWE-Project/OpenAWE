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

#include <GLFW/glfw3.h>

#include "src/common/exception.h"

#include "src/events/gamepad.h"

#include "src/platform/gamepadconversion.h"

namespace Platform {

static constexpr int GLFW_GAMEPAD_AXIS_LEFT = GLFW_GAMEPAD_AXIS_LEFT_X | GLFW_GAMEPAD_AXIS_LEFT_Y;
static constexpr int GLFW_GAMEPAD_AXIS_RIGHT = GLFW_GAMEPAD_AXIS_RIGHT_X | GLFW_GAMEPAD_AXIS_RIGHT_Y;

Events::Gamepad2DAxis convertGLFW2GamepadStick(int stick) {
    switch (stick) {
        case GLFW_GAMEPAD_AXIS_LEFT: return Events::Gamepad2DAxis::kGamepadAxisLeft;
        case GLFW_GAMEPAD_AXIS_RIGHT: return Events::Gamepad2DAxis::kGamepadAxisRight;
        default: throw CreateException("Trying to convert unknown gamepad stick {}", stick);
    }
}

Events::Gamepad1DAxis convertGLFW2GamepadTrigger(int trigger) {
    switch (trigger) {
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return Events::Gamepad1DAxis::kGamepadAxisLeftTrigger;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return Events::Gamepad1DAxis::kGamepadAxisRightTrigger;
        default: throw CreateException("Trying to convert unknown gamepad trigger {}", trigger);
    }
}

} // End of namespace Platform
