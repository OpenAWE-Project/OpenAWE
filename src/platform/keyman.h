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

#ifndef OPENAWE_KEYMAN_H
#define OPENAWE_KEYMAN_H

#include <set>
#include <map>
#include <optional>
#include <functional>

#include <GLFW/glfw3.h>

#include "src/common/singleton.h"

namespace Platform {

// Mimics actions from GLFW, but adds hold state
#ifndef OPENAWE_KEY_ACTIONS
#define OPENAWE_KEY_ACTIONS
enum KeyActions {
	kActionRelease = 0,
	kActionPress   = 1,
	kActionRepeat  = 2,
	kActionHold    = 4
};
#endif // OPENAWE_KEY_ACTIONS

typedef struct {
	std::set<std::pair<int, int>> keysHeld;
	std::set<int> mouseButtonsHeld;
	std::optional<GLFWkeyfun> keyCallback;
	std::optional<GLFWmousebuttonfun> mouseButtonCallback;
} windowButtonInfo;

/*!
 * \brief Class for supplementing hold inputs
 *
 * This class is a singleton for providing additional events to inputs, 
 * which is required for easier implementation of in-game controls. 
 */
class KeyManager: public Common::Singleton<KeyManager> {
public:
	void pollHoldEvents();

	// These functions mimic equivalent GLFW functions, thus the return type
	GLFWkeyfun setKeyCallback(GLFWwindow *window, GLFWkeyfun function);
	GLFWmousebuttonfun setMouseButtonCallback(GLFWwindow *window, GLFWmousebuttonfun function);

private:
	static void injectKeyHoldState(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void injectMouseButtonHoldState(GLFWwindow *window, int button, int action, int mods);

	std::map<GLFWwindow *, windowButtonInfo> _windows;
};

} // End of namespace Platform

#define KeyMan Platform::KeyManager::instance()

#endif // OPENAWE_KEYMAN_H
