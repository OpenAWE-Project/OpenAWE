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

#include "eventman.h"

namespace Events {

void EventManager::injectKeyboardInput(Events::Key key, Events::KeyState state) {
	const auto actions = _keyBindings.equal_range(key);

	KeyEvent keyEvent{state};
	Event event;
	event.data = keyEvent;

	for (auto actionIter = actions.first; actionIter != actions.second; ++actionIter) {
		const auto action = actionIter->second;

		const auto callback = _actionCallbacks[action];
		event.action = action;

		callback(event);
	}
}

void EventManager::injectMouseInput(Events::Mouse mouse, Events::KeyState state) {
	const auto actions = _mouseBindings.equal_range(mouse);

	KeyEvent keyEvent{state};
	Event event;
	event.data = keyEvent;

	for (auto actionIter = actions.first; actionIter != actions.second; ++actionIter) {
		const auto action = actionIter->second;

		const auto callback = _actionCallbacks[action];
		event.action = action;

		callback(event);
	}
}

void EventManager::injectMousePositionInput(glm::vec2 position, glm::vec2 delta) {
	const auto actions = _mousePositionBindings;

	AxisEvent<glm::vec2> axis{position, delta};
	Event event;
	event.data = axis;

	for (auto &action: actions) {
		const auto callback = _actionCallbacks[action];
		event.action = action;

		callback(event);
	}
}

void EventManager::setActionCallback(std::initializer_list<uint32_t> actions, EventCallback callback) {
	for (const auto &action: actions) {
		_actionCallbacks[action] = callback;
	}
}

void EventManager::addBinding(uint32_t action, Key key) {
	_keyBindings.insert(std::make_pair(key, action));
}

void EventManager::addBinding(uint32_t action, Mouse mouse) {
	_mouseBindings.insert(std::make_pair(mouse, action));
}

void EventManager::addMouseBinding(uint32_t action) {
	_mousePositionBindings.push_back(action);
}

}
