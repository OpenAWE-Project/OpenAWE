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

#include "src/common/crc32.h"

#include "src/events/eventman.h"

#include "src/controlledfreecamera.h"

static constexpr uint32_t kMoveForward  = Common::crc32("FREECAM_MOVE_FORWARD");
static constexpr uint32_t kMoveBackward = Common::crc32("FREECAM_MOVE_BACKWARD");
static constexpr uint32_t kMoveLeft     = Common::crc32("FREECAM_MOVE_LEFT");
static constexpr uint32_t kMoveRight    = Common::crc32("FREECAM_MOVE_RIGHT");
static constexpr uint32_t kMoveUp       = Common::crc32("FREECAM_MOVE_UP");
static constexpr uint32_t kMoveDown     = Common::crc32("FREECAM_MOVE_DOWN");
static constexpr uint32_t kMoveGamepad  = Common::crc32("FREECAM_MOVE_GAMEPAD");

static constexpr uint32_t kRotateLeft          = Common::crc32("FREECAM_ROTATE_LEFT");
static constexpr uint32_t kRotateRight         = Common::crc32("FREECAM_ROTATE_RIGHT");
static constexpr uint32_t kRotateUp            = Common::crc32("FREECAM_ROTATE_UP");
static constexpr uint32_t kRotateDown          = Common::crc32("FREECAM_ROTATE_DOWN");
static constexpr uint32_t kRotateMouse         = Common::crc32("FREECAM_ROTATE_MOUSE");
static constexpr uint32_t kRotateGamepad       = Common::crc32("FREECAM_ROTATE_GAMEPAD");
static constexpr uint32_t kSwitchMouseControls = Common::crc32("FREECAM_SWITCH_MOUSE_CONTROLS");

static constexpr uint32_t kIncreaseSpeed       = Common::crc32("FREECAM_INCREASE_SPEED");
static constexpr uint32_t kDecreaseSpeed       = Common::crc32("FREECAM_DECREASE_SPEED");
static constexpr uint32_t kIncreaseSensitivity = Common::crc32("FREECAM_INCREASE_SENSITIVITY");
static constexpr uint32_t kDecreaseSensitivity = Common::crc32("FREECAM_DECREASE_SENSITIVITY");

ControlledFreeCamera::ControlledFreeCamera() {
	Events::EventCallback callbackMove = [this](auto && PH1) { handleMovement(std::forward<decltype(PH1)>(PH1)); };
	Events::EventCallback callbackRotate = [this](auto && PH1) { handleRotation(std::forward<decltype(PH1)>(PH1)); };
	Events::EventCallback callbackSwitch = [this](auto && PH1) { switchMouseInput(std::forward<decltype(PH1)>(PH1)); };

	EventMan.setActionCallback(
		{kMoveForward, kMoveBackward, kMoveLeft, kMoveRight, kMoveUp, kMoveDown, kMoveGamepad, kIncreaseSpeed, kDecreaseSpeed},
		callbackMove
	);

	EventMan.setActionCallback(
		{kRotateLeft, kRotateRight, kRotateDown, kRotateUp, kRotateMouse, kRotateGamepad, kIncreaseSensitivity, kDecreaseSensitivity},
		callbackRotate
	);

	EventMan.setActionCallback({ kSwitchMouseControls }, callbackSwitch);

	EventMan.addBinding(kMoveForward, Events::kKeyW);
	EventMan.addBinding(kMoveLeft, Events::kKeyA);
	EventMan.addBinding(kMoveBackward, Events::kKeyS);
	EventMan.addBinding(kMoveRight, Events::kKeyD);
	EventMan.addBinding(kMoveUp, Events::kKeyR);
	EventMan.addBinding(kMoveDown, Events::kKeyF);

	EventMan.addBinding(kRotateLeft, Events::kKeyLeft);
	EventMan.addBinding(kRotateRight, Events::kKeyRight);
	EventMan.addBinding(kRotateUp, Events::kKeyUp);
	EventMan.addBinding(kRotateDown, Events::kKeyDown);
	EventMan.addBinding(kSwitchMouseControls, Events::kKeyQ);

	EventMan.addBinding(kIncreaseSpeed, Events::kKeyT);
	EventMan.addBinding(kDecreaseSpeed, Events::kKeyG);
	EventMan.addBinding(kIncreaseSensitivity, Events::kKeyY);
	EventMan.addBinding(kDecreaseSensitivity, Events::kKeyH);

	EventMan.add2DAxisBinding(kRotateMouse, Events::kMousePosition);
	_mouseInputAllowed = false;

	EventMan.add2DAxisBinding(kMoveGamepad, Events::kGamepadAxisLeft);
	EventMan.add2DAxisBinding(kRotateGamepad, Events::kGamepadAxisRight);

	EventMan.addBinding(kMoveUp, Events::kGamepadButtonDPadUp);
	EventMan.addBinding(kMoveDown, Events::kGamepadButtonDPadDown);
	EventMan.addBinding(kIncreaseSensitivity, Events::kGamepadButtonA);
	EventMan.addBinding(kDecreaseSensitivity, Events::kGamepadButtonB);
	EventMan.addBinding(kIncreaseSpeed, Events::kGamepadButtonX);
	EventMan.addBinding(kDecreaseSpeed, Events::kGamepadButtonY);
}

void ControlledFreeCamera::handleMovement(const Events::Event &event) {
	const auto keyEvent = std::get_if<Events::KeyEvent>(&event.data);
	const auto axisEvent = std::get_if<Events::AxisEvent<glm::vec2>>(&event.data);
	if (keyEvent) {
		switch (event.action) {
			case kMoveUp:       _movementDirection.y = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0; break;
			case kMoveDown:     _movementDirection.y = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0; break;
			case kMoveForward:  _movementDirection.z = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0; break;
			case kMoveBackward: _movementDirection.z = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0; break;
			case kMoveLeft:     _movementDirection.x = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0; break;
			case kMoveRight:    _movementDirection.x = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0; break;
		}

		if (keyEvent->state == Events::kRelease) {
			switch (event.action) {
				case kIncreaseSpeed:
					setMovementFactor(getMovementFactor() * 2.0f);
					break;
				case kDecreaseSpeed:
					setMovementFactor(getMovementFactor() * 0.5f);
					break;
			}
		}
	} else if (axisEvent) {
		if (event.action == kMoveGamepad) {
			_movementDirection.x = axisEvent->delta.x;
			_movementDirection.y = axisEvent->delta.y;
		}
	}
}

void ControlledFreeCamera::handleRotation(const Events::Event &event) {
	const auto keyEvent = std::get_if<Events::KeyEvent>(&event.data);
	const auto axisEvent = std::get_if<Events::AxisEvent<glm::vec2>>(&event.data);
	if (keyEvent) {
		if (!_mouseInputAllowed) {
			switch (event.action) {
				case kRotateLeft:  _movementRotation.x = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0; break;
				case kRotateRight: _movementRotation.x = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0; break;
				case kRotateUp:    _movementRotation.y = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0; break;
				case kRotateDown:  _movementRotation.y = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0; break;
			}
		}

		if (keyEvent->state == Events::kRelease) {
			switch (event.action) {
				case kIncreaseSensitivity:
					setRotationFactor(getRotationFactor() * 2.0f);
					break;
				case kDecreaseSensitivity:
					setRotationFactor(getRotationFactor() * 0.5f);
					break;
			}
		}
	} else if (axisEvent) {
		if (event.action == kRotateMouse && _mouseInputAllowed) {
			_movementRotation.x = axisEvent->delta.x;
			_movementRotation.y = axisEvent->delta.y;
		} else if (event.action == kRotateGamepad) {
			_movementRotation.x = axisEvent->delta.x;
			_movementRotation.y = axisEvent->delta.y;
		}
	}
}

void ControlledFreeCamera::useMouseInput(bool enabled) {
	_mouseInputAllowed = enabled;
}

bool ControlledFreeCamera::isMouseInputAllowed() {
	return _mouseInputAllowed;
}

void ControlledFreeCamera::switchMouseInput(const Events::Event &event){
	Events::KeyEvent keyEvent = std::get<Events::KeyEvent>(event.data);
	if (keyEvent.state == Events::kPress)
		_mouseInputAllowed = !_mouseInputAllowed;
}
