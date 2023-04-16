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

#ifndef OPENAWE_INPUTMAN_H
#define OPENAWE_INPUTMAN_H

#include <set>
#include <map>
#include <optional>
#include <functional>

#include <GLFW/glfw3.h>

#include "src/common/singleton.h"

namespace Platform {

// Mimics actions from GLFW, but adds hold state
enum KeyActions {
	kActionRelease = 0,
	kActionPress   = 1,
	kActionRepeat  = 2,
	kActionHold    = 4
};

typedef std::function<void (GLFWwindow *window, int button, int action)> InputGamepadButtonCallback;
typedef std::function<void (GLFWwindow *window, int trigger, double pos)> InputGamepadTriggerCallback;
typedef std::function<void (GLFWwindow *window, int stick, double xpos, double ypos)> InputGamepadStickCallback;

typedef struct {
	std::set<std::pair<int, int>> keysHeld;
	std::set<int> mouseButtonsHeld;
	std::set<int> gamepadButtonsHeld;
	std::optional<GLFWkeyfun> keyCallback;
	std::optional<GLFWmousebuttonfun> mouseButtonCallback;
	std::optional<InputGamepadButtonCallback> gamepadButtonCallback;
	std::optional<InputGamepadTriggerCallback> gamepadTriggerCallback;
	std::optional<InputGamepadStickCallback> gamepadStickCallback;
} windowInfo;

/*!
 * \brief Class for supplementing hold inputs
 *
 * This class is a singleton for providing additional callbacks to inputs. It allows to overcome some of the
 * current shortcomings of the GLFW library, such as lack of gamepad input callbacks and implementing hold
 * state for keys, which is required for easier implementation of in-game controls. 
 */
class InputManager: public Common::Singleton<InputManager> {
public:
	InputManager();
	void pollHoldEvents();

	// These functions mimic equivalent GLFW functions, thus the return type
	GLFWkeyfun setKeyCallback(GLFWwindow *window, GLFWkeyfun function);
	GLFWmousebuttonfun setMouseButtonCallback(GLFWwindow *window, GLFWmousebuttonfun function);

	void pollGamepadEvents();
	void setGamepadButtonCallback(GLFWwindow *window, InputGamepadButtonCallback function);
	void setGamepadTriggerCallback(GLFWwindow *window, InputGamepadTriggerCallback function);
	void setGamepadStickCallback(GLFWwindow *window, InputGamepadStickCallback function);
	bool hasActiveGamepad();
	std::optional<std::string> getActiveGamepadName();

private:
	static void injectKeyHoldState(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void injectMouseButtonHoldState(GLFWwindow *window, int button, int action, int mods);

	static void callbackJoystickConnectionChanged(int jid, int event);

	std::map<GLFWwindow *, windowInfo> _windows;
	std::optional<int> _activeGamepadId;
};

} // End of namespace Platform

#define InputMan InputManager::instance()

#endif // OPENAWE_INPUTMAN_H
