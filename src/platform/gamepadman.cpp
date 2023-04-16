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
#include <spdlog/spdlog.h>

#include "gamepadman.h"

namespace Platform {

GamepadManager::GamepadManager() {
    glfwSetJoystickCallback(&GamepadManager::callbackJoystickConnectionChanged);
}

void GamepadManager::callbackJoystickConnectionChanged(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        if (!GamepadMan._activeGamepadId && glfwJoystickIsGamepad(jid)) {
            GamepadMan._activeGamepadId = jid;
        }
    } else if (event == GLFW_DISCONNECTED) {
        if (GamepadMan._activeGamepadId == jid) {
            GamepadMan._activeGamepadId = {};
            for (int gid = 0; gid < GLFW_JOYSTICK_LAST; gid++) {
                if (glfwJoystickIsGamepad(gid)) {
                    GamepadMan._activeGamepadId = gid;
                    break;
                }
            }
        }
    }
}

void GamepadManager::pollGamepadEvents() {
    if (!_activeGamepadId)
        return;
    
    GLFWgamepadstate state;
    glfwGetGamepadState(_activeGamepadId.value(), &state);
    for (auto &window_pair: _windows) {
        GLFWwindow *window = window_pair.first;
        windowGamepadInfo *wInfo = &(window_pair.second);

        for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++) {
            if (state.buttons[i] == GLFW_PRESS) {
                if (wInfo->gamepadButtonsHeld.count(i)) {
                    if (wInfo->gamepadButtonCallback) {
                        InputGamepadButtonCallback callback = wInfo->gamepadButtonCallback.value();
                        callback(window, i, kActionHold);
                    }
                } else {
                    wInfo->gamepadButtonsHeld.insert(i);

                    if (wInfo->gamepadButtonCallback) {
                        InputGamepadButtonCallback callback = wInfo->gamepadButtonCallback.value();
                        callback(window, i, kActionPress);
                    }
                }
            } else if (state.buttons[i] == GLFW_RELEASE) {
                if (wInfo->gamepadButtonsHeld.count(i)) {
                    wInfo->gamepadButtonsHeld.erase(i);

                    if (wInfo->gamepadButtonCallback) {
                        InputGamepadButtonCallback callback = wInfo->gamepadButtonCallback.value();
                        callback(window, i, kActionRelease);
                    }
                }
            }
        }
        if (wInfo->gamepadStickCallback) {
            static constexpr int GLFW_GAMEPAD_AXIS_LEFT = GLFW_GAMEPAD_AXIS_LEFT_X | GLFW_GAMEPAD_AXIS_LEFT_Y;
            static constexpr int GLFW_GAMEPAD_AXIS_RIGHT = GLFW_GAMEPAD_AXIS_RIGHT_X | GLFW_GAMEPAD_AXIS_RIGHT_Y;

            InputGamepadStickCallback callback = wInfo->gamepadStickCallback.value();
            if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] != 0.0f && state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] != 0.0f)
                callback(window, GLFW_GAMEPAD_AXIS_LEFT, state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
            if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] != 0.0f && state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] != 0.0f)
                callback(window, GLFW_GAMEPAD_AXIS_RIGHT, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        }
        if (wInfo->gamepadTriggerCallback) {
            InputGamepadTriggerCallback callback = wInfo->gamepadTriggerCallback.value();
            if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] != 0.0f)
                callback(window, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
            if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] != 0.0f)
                callback(window, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
        }
    }
}

void GamepadManager::setGamepadButtonCallback(GLFWwindow *window, InputGamepadButtonCallback function) {
    windowGamepadInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadButtonCallback = function;
}

void GamepadManager::setGamepadStickCallback(GLFWwindow *window, InputGamepadStickCallback function) {
    windowGamepadInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadStickCallback = function;
}

void GamepadManager::setGamepadTriggerCallback(GLFWwindow *window, InputGamepadTriggerCallback function) {
    windowGamepadInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadTriggerCallback = function;
}

bool GamepadManager::hasActiveGamepad() {
    return _activeGamepadId.has_value();
}

std::optional<std::string> GamepadManager::getActiveGamepadName() {
    if (hasActiveGamepad())
        return std::string(glfwGetGamepadName(_activeGamepadId.value()));
    else
        return {};
}

} // End of namespace Platform
