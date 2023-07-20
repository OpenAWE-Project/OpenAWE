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

void EventManager::injectKeyboardInput(Events::Key key, Events::KeyState state, uint32_t modifiers) {
	KeyCombination combination = std::make_pair(key, modifiers);
	const auto actions = _keyBindings.equal_range(combination);

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

void EventManager::injectMouseButtonInput(Events::MouseButton mouse, Events::KeyState state) {
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

void EventManager::injectGamepadButtonInput(Events::GamepadButton button, Events::KeyState state) {
	const auto actions = _gamepadBindings.equal_range(button);

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

void EventManager::injectGamepad2DAxisInput(Events::Gamepad2DAxis axis, glm::vec2 position, glm::vec2 delta) {
	const auto actions = _gamepad2DAxisBindings.equal_range(axis);

	AxisEvent<glm::vec2> axisEvent{position, delta};
	Event event;
	event.data = axisEvent;

	for (auto actionIter = actions.first; actionIter != actions.second; ++actionIter) {
		const auto action = actionIter->second;

		const auto callback = _actionCallbacks[action];
		event.action = action;

		callback(event);
	}
}

void EventManager::injectGamepad1DAxisInput(Events::Gamepad1DAxis axis, double position, double delta) {
	const auto actions = _gamepad1DAxisBindings.equal_range(axis);

	AxisEvent<double> axisEvent{position, delta};
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
	_keyBindings.insert(std::make_pair(std::make_pair(key, kNoModifier), action));
}

void EventManager::addBinding(uint32_t action, Key key, uint32_t modifiers) {
	_keyBindings.insert(std::make_pair(std::make_pair(key, modifiers), action));
}

void EventManager::addBinding(uint32_t action, MouseButton mouse) {
	_mouseBindings.insert(std::make_pair(mouse, action));
}

void EventManager::addBinding(uint32_t action, GamepadButton button) {
	_gamepadBindings.insert(std::make_pair(button, action));
}

void EventManager::add2DAxisBinding(uint32_t action, Mouse2DAxis axis) {
	_mouse2DAxisBindings.insert(std::make_pair(axis, action));
}

void EventManager::add1DAxisBinding(uint32_t action, Mouse1DAxis axis) {
	_mouse1DAxisBindings.insert(std::make_pair(axis, action));
}

void EventManager::add2DAxisBinding(uint32_t action, Gamepad2DAxis axis) {
	_gamepad2DAxisBindings.insert(std::make_pair(axis, action));
}

void EventManager::add1DAxisBinding(uint32_t action, Gamepad1DAxis axis) {
	_gamepad1DAxisBindings.insert(std::make_pair(axis, action));
}

}
