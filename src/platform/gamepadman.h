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
#include <string>
#include <optional>
#include <functional>

#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

#include "src/common/singleton.h"

namespace Platform {

typedef std::function<void (int button, int action)> InputGamepadButtonCallback;
typedef std::function<void (int trigger, double pos, double delta)> InputGamepadTriggerCallback;
typedef std::function<void (int stick, glm::vec2 pos, glm::vec2 delta)> InputGamepadStickCallback;


/*!
 * \brief Class for supplementing gamepad input callbacks
 *
 * This class is a singleton for providing additional callbacks to inputs. It allows to overcome 
 * lack of gamepad input callbacks in GLFW, which is required for easier implementation of in-game controls. 
 */
class GamepadManager: public Common::Singleton<GamepadManager> {
public:
	GamepadManager();

	void pollGamepadEvents();
	void setGamepadButtonCallback(InputGamepadButtonCallback function);
	void setGamepadTriggerCallback(InputGamepadTriggerCallback function);
	void setGamepadStickCallback(InputGamepadStickCallback function);
	bool hasActiveGamepad();
	std::optional<std::string> getActiveGamepadName();

private:
	void searchForActiveGamepad();
	void setActiveGamepad(int id);

	static void callbackJoystickConnectionChanged(int jid, int event);

	std::optional<int> _activeGamepadId;
	std::set<int> buttonsHeld;
	std::map<int, double> triggerLastValues;
	std::map<int, glm::vec2> stickLastValues;
	std::optional<InputGamepadButtonCallback> buttonCallback;
	std::optional<InputGamepadTriggerCallback> triggerCallback;
	std::optional<InputGamepadStickCallback> stickCallback;

    static constexpr int GLFW_GAMEPAD_AXIS_LEFT = GLFW_GAMEPAD_AXIS_LEFT_X | GLFW_GAMEPAD_AXIS_LEFT_Y;
    static constexpr int GLFW_GAMEPAD_AXIS_RIGHT = GLFW_GAMEPAD_AXIS_RIGHT_X | GLFW_GAMEPAD_AXIS_RIGHT_Y;
};

} // End of namespace Platform

#define GamepadMan Platform::GamepadManager::instance()

#endif // OPENAWE_INPUTMAN_H
