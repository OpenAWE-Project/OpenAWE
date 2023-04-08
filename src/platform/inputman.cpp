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

#include "inputman.h"

namespace Platform {

InputManager::InputManager() {
    glfwSetJoystickCallback(&InputManager::callbackJoystickConnectionChanged);
}

GLFWkeyfun InputManager::setKeyCallback(GLFWwindow *window, GLFWkeyfun function) {
    glfwSetKeyCallback(window, &InputManager::injectKeyHoldState);
    auto windowState = _windows.find(window);
    if (windowState != _windows.end()) {
        windowInfo *wInfo = &(windowState->second);
        GLFWkeyfun prev = wInfo->keyCallback.value_or(nullptr);
        wInfo->keyCallback = function;
        return prev;
    } else {
        // Init new windowInfo
        windowInfo newInfo;
        newInfo.keyCallback = function;
        _windows.insert({window, newInfo});
        return nullptr;
    }
}

GLFWmousebuttonfun InputManager::setMouseButtonCallback(GLFWwindow *window, GLFWmousebuttonfun function) {
    glfwSetMouseButtonCallback(window, &InputManager::injectMouseButtonHoldState);
    auto windowState = _windows.find(window);
    if (windowState != _windows.end()) {
        windowInfo *wInfo = &(windowState->second);
        GLFWmousebuttonfun prev = wInfo->mouseButtonCallback.value_or(nullptr);
        wInfo->mouseButtonCallback = function;
        return prev;
    } else {
        // Init new windowInfo
        windowInfo newInfo;
        newInfo.mouseButtonCallback = function;
        _windows.insert({window, newInfo});
        return nullptr;
    }
}

void InputManager::injectKeyHoldState(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;
    
    windowInfo *wInfo = &(InputMan._windows.find(window)->second);
    if (action == GLFW_PRESS) {
        wInfo->keysHeld.insert({key, scancode});
    } else if (action == GLFW_RELEASE) {
        wInfo->keysHeld.erase({key, scancode});
    }
    if (wInfo->keyCallback)
        wInfo->keyCallback.value()(window, key, scancode, action, mods);
}

void InputManager::injectMouseButtonHoldState(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;
    
    windowInfo *wInfo = &(InputMan._windows.find(window)->second);
    if (action == GLFW_PRESS) {
        wInfo->mouseButtonsHeld.insert(button);
    } else if (action == GLFW_RELEASE) {
        wInfo->mouseButtonsHeld.erase(button);
    }
    if (wInfo->mouseButtonCallback)
        wInfo->mouseButtonCallback.value()(window, button, action, mods);
}

void InputManager::pollHoldEvents() {
    for (auto &window_pair: _windows) {
        GLFWwindow *window = window_pair.first;
        windowInfo wInfo = window_pair.second;
        if (wInfo.keyCallback) {
            GLFWkeyfun callback = wInfo.keyCallback.value();
            for (auto &key: wInfo.keysHeld) {
                callback(window, key.first, key.second, kActionHold, 0);
            }
        }
        if (wInfo.mouseButtonCallback) {
            GLFWmousebuttonfun callback = wInfo.mouseButtonCallback.value();
            for (auto &key: wInfo.mouseButtonsHeld) {
                callback(window, key, kActionHold, 0);
            }
        }
    }
}

void InputManager::callbackJoystickConnectionChanged(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        if (!InputMan._activeGamepadId && glfwJoystickIsGamepad(jid)) {
            InputMan._activeGamepadId = jid;
        }
    } else if (event == GLFW_DISCONNECTED) {
        if (InputMan._activeGamepadId == jid) {
            InputMan._activeGamepadId = {};
            for (int gid = 0; gid < GLFW_JOYSTICK_LAST; gid++) {
                if (glfwJoystickIsGamepad(gid)) {
                    InputMan._activeGamepadId = gid;
                    break;
                }
            }
        }
    }
}

void InputManager::pollGamepadEvents() {
    if (!_activeGamepadId)
        return;
    
    GLFWgamepadstate state;
    glfwGetGamepadState(_activeGamepadId.value(), &state);
    for (auto &window_pair: _windows) {
        GLFWwindow *window = window_pair.first;
        windowInfo *wInfo = &(window_pair.second);

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
            InputGamepadStickCallback callback = wInfo->gamepadStickCallback.value();
            if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] != 0.0f && state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] != 0.0f)
                callback(window, 0, state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
            if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] != 0.0f && state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] != 0.0f)
                callback(window, 1, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        }
        if (wInfo->gamepadTriggerCallback) {
            InputGamepadTriggerCallback callback = wInfo->gamepadTriggerCallback.value();
            if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] != 0.0f)
                callback(window, 0, state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
            if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] != 0.0f)
                callback(window, 1, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
        }
    }
}

void InputManager::setGamepadButtonCallback(GLFWwindow *window, InputGamepadButtonCallback function) {
    windowInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadButtonCallback = function;
}

void InputManager::setGamepadStickCallback(GLFWwindow *window, InputGamepadStickCallback function) {
    windowInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadStickCallback = function;
}

void InputManager::setGamepadTriggerCallback(GLFWwindow *window, InputGamepadTriggerCallback function) {
    windowInfo *wInfo = &(_windows.find(window)->second);
    wInfo->gamepadTriggerCallback = function;
}

bool InputManager::hasActiveGamepad() {
    return _activeGamepadId.has_value();
}

std::optional<std::string> InputManager::getActiveGamepadName() {
    if (hasActiveGamepad())
        return std::string(glfwGetGamepadName(_activeGamepadId.value()));
    else
        return {};
}

} // End of namespace Platform
