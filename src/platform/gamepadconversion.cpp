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

#include "src/events/gamepad.h"

#include "src/platform/gamepadconversion.h"

namespace Platform {

Events::Gamepad2DAxis convertGamepadStick(int stick) {
	switch (stick) {
        case 0: return Events::Gamepad2DAxis::kGamepadAxisLeft;
        case 1: return Events::Gamepad2DAxis::kGamepadAxisRight;
    }
}

Events::Gamepad1DAxis convertGamepadTrigger(int trigger) {
	switch (trigger) {
        case 0: return Events::Gamepad1DAxis::kGamepadAxisLeftTrigger;
        case 1: return Events::Gamepad1DAxis::kGamepadAxisRightTrigger;
    }
}

} // End of namespace Platform
