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

#include "src/common/exception.h"

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

void EventManager::injectMouseInput(Events::MouseButton mouse, Events::KeyState state) {
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

void EventManager::injectMouse2DAxisInput(Events::Mouse2DAxis axis, glm::vec2 position, glm::vec2 delta) {
	const auto actions = _mouse2DAxisBindings.equal_range(axis);

	AxisEvent<glm::vec2> axisEvent{position, delta};
	Event event;
	event.data = axisEvent;

	for (auto actionIter = actions.first; actionIter != actions.second; ++actionIter) {
		const auto action = actionIter->second;

		const auto callback = _actionCallbacks[action];
		event.action = action;

		callback(event);
	}

	// Also inject separate 1D axis inputs
	injectMouse1DAxisInput(static_cast<Mouse1DAxis>(axis | kSliceHorizontal), position.x, delta.x);
	injectMouse1DAxisInput(static_cast<Mouse1DAxis>(axis | kSliceVertical), position.y, delta.y);
}

void EventManager::injectMouse1DAxisInput(Events::Mouse1DAxis axis, float position, float delta) {
	const auto actions = _mouse1DAxisBindings.equal_range(axis);

	AxisEvent<float> axisEvent{position, delta};
	Event event;
	event.data = axisEvent;

	for (auto actionIter = actions.first; actionIter != actions.second; ++actionIter) {
		const auto action = actionIter->second;

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

void EventManager::addBinding(uint32_t action, MouseButton mouse) {
	_mouseBindings.insert(std::make_pair(mouse, action));
}

void EventManager::add2DAxisBinding(uint32_t action, Mouse2DAxis axis) {
	// ensure that none of the 1D axes are already busy
	Mouse1DAxis slices[2] = {static_cast<Mouse1DAxis>(axis | kSliceHorizontal), static_cast<Mouse1DAxis>(axis | kSliceVertical)};

	if ((_mouse1DAxisBindings.count(slices[0]) == 0) && (_mouse1DAxisBindings.count(slices[0]) == 0)) {
		_mouse2DAxisBindings.insert(std::make_pair(axis, action));
	} else {
		CreateException("Attempted to add binding to a 2D axis {} while one of its 1D axes was in use", axis);
	}
}

void EventManager::add1DAxisBinding(uint32_t action, Mouse1DAxis axis) {
	// ensure that 2D axis is not already busy
	Mouse2DAxis full = static_cast<Mouse2DAxis>((axis >> 2) << 2);

	if (_mouse2DAxisBindings.count(full) == 0) {
		_mouse1DAxisBindings.insert(std::make_pair(axis, action));
	} else {
		CreateException("Attempted to add binding to a 1D axis {} while its full 2D axis {} was in use", axis, full);
	}
}

}
