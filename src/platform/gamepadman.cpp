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

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "gamepadman.h"

namespace Platform {

GamepadManager::GamepadManager() {
    glfwSetJoystickCallback(&GamepadManager::callbackJoystickConnectionChanged);

    // see if there are any connected gamepads already
    searchForActiveGamepad();
}

void GamepadManager::callbackJoystickConnectionChanged(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        if (!GamepadMan._activeGamepadId && glfwJoystickIsGamepad(jid)) {
            GamepadMan.setActiveGamepad(jid);
            spdlog::debug("Gamepad {} with id {} has been connected", GamepadMan.getActiveGamepadName().value(), jid);
        }
    } else if (event == GLFW_DISCONNECTED) {
        if (GamepadMan._activeGamepadId == jid) {
            GamepadMan._activeGamepadId = {};
            spdlog::debug("Gamepad with id {} has been disconnected", jid);
            GamepadMan.searchForActiveGamepad();
        }
    }
}

void GamepadManager::setActiveGamepad(int id) {
    _activeGamepadId = id;

    // set default last axis/trigger values
    stickLastValues[GLFW_GAMEPAD_AXIS_LEFT] = glm::vec2(0, 0);
    stickLastValues[GLFW_GAMEPAD_AXIS_RIGHT] = glm::vec2(0, 0);
    triggerLastValues[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = 0.0;
    triggerLastValues[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = 0.0;
}

void GamepadManager::searchForActiveGamepad() {
    for (int gid = 0; gid < GLFW_JOYSTICK_LAST; gid++) {
        if (glfwJoystickIsGamepad(gid)) {
            setActiveGamepad(gid);
            spdlog::debug("Found connected gamepad {} with id {} to use", getActiveGamepadName().value(), gid);
            break;
        }
    }
}

void GamepadManager::pollGamepadEvents() {
    if (!_activeGamepadId)
        return;
    
    GLFWgamepadstate state;
    glfwGetGamepadState(_activeGamepadId.value(), &state);

    for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++) {
        if (state.buttons[i] == GLFW_PRESS) {
            if (buttonsHeld.count(i) == 0) {
                buttonsHeld.insert(i);

                if (buttonCallback) {
                    InputGamepadButtonCallback callback = buttonCallback.value();
                    callback(i, GLFW_PRESS);
                }
            }
        } else if (state.buttons[i] == GLFW_RELEASE) {
            if (buttonsHeld.count(i)) {
                buttonsHeld.erase(i);

                if (buttonCallback) {
                    InputGamepadButtonCallback callback = buttonCallback.value();
                    callback(i, GLFW_RELEASE);
                }
            }
        }
    }
    if (stickCallback) {
        InputGamepadStickCallback callback = stickCallback.value();

        glm::vec2 axisNew = glm::vec2(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
        glm::vec2 axisLast = stickLastValues.find(GLFW_GAMEPAD_AXIS_LEFT)->second;
        if (axisNew.x != 0.0f && axisNew.y != 0.0f) {
            glm::vec2 delta = axisNew - axisLast;
            callback(GLFW_GAMEPAD_AXIS_LEFT, axisNew, delta);
            stickLastValues[GLFW_GAMEPAD_AXIS_LEFT] = axisNew;
        }
        else if (axisLast != axisNew) // where axisNew is presumably a zero vector
            stickLastValues[GLFW_GAMEPAD_AXIS_LEFT] = axisNew;
        
        axisNew = glm::vec2(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        axisLast = stickLastValues.find(GLFW_GAMEPAD_AXIS_RIGHT)->second;
        if (axisNew.x != 0.0f && axisNew.y != 0.0f) {
            glm::vec2 delta = axisNew - axisLast;
            callback(GLFW_GAMEPAD_AXIS_RIGHT, axisNew, delta);
            stickLastValues[GLFW_GAMEPAD_AXIS_RIGHT] = axisNew;
        }
        else if (axisLast != axisNew) // where axisNew is presumably a zero vector
            stickLastValues[GLFW_GAMEPAD_AXIS_RIGHT] = axisNew;
    }
    if (triggerCallback) {
        InputGamepadTriggerCallback callback = triggerCallback.value();

        double triggerNew = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
        double triggerLast = triggerLastValues.find(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER)->second;
        if (triggerNew != 0.0f) {
            double delta = triggerNew - triggerLast;
            callback(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, triggerNew, delta);
            triggerLastValues[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = triggerNew;
        }
        else if (triggerLast != triggerNew) // where triggerNew is presumably a zero
            triggerLastValues[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = triggerNew;

        triggerNew = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
        triggerLast = triggerLastValues.find(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)->second;
        if (triggerNew != 0.0f) {
            double delta = triggerNew - triggerLast;
            callback(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, triggerNew, delta);
            triggerLastValues[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = triggerNew;
        }
        else if (triggerLast != triggerNew) // where triggerNew is presumably a zero
            triggerLastValues[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = triggerNew;
    }
}

void GamepadManager::setGamepadButtonCallback(InputGamepadButtonCallback function) {
    buttonCallback = function;
}

void GamepadManager::setGamepadStickCallback(InputGamepadStickCallback function) {
    stickCallback = function;
}

void GamepadManager::setGamepadTriggerCallback(InputGamepadTriggerCallback function) {
    triggerCallback = function;
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
