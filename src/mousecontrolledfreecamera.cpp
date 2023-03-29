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
#include <glm/gtx/rotate_vector.hpp>

#include "src/common/crc32.h"

#include "src/events/eventman.h"

#include "src/mousecontrolledfreecamera.h"

static constexpr uint32_t kMoveForward  = Common::crc32("FREECAM_MOVE_FORWARD");
static constexpr uint32_t kMoveBackward = Common::crc32("FREECAM_MOVE_BACKWARD");
static constexpr uint32_t kMoveLeft     = Common::crc32("FREECAM_MOVE_LEFT");
static constexpr uint32_t kMoveRight    = Common::crc32("FREECAM_MOVE_RIGHT");
static constexpr uint32_t kMoveUp       = Common::crc32("FREECAM_MOVE_UP");
static constexpr uint32_t kMoveDown     = Common::crc32("FREECAM_MOVE_DOWN");

static constexpr uint32_t kRotate     = Common::crc32("FREECAM_ROTATE");

static constexpr uint32_t kIncreaseSpeed = Common::crc32("FREECAM_INCREASE_SPEED");
static constexpr uint32_t kDecreaseSpeed = Common::crc32("FREECAM_DECREASE_SPEED");

MouseControlledFreeCamera::MouseControlledFreeCamera(): _yaw(0.0f), _pitch(0.0f), _roll(0.0f), _invertY(kDoNotInvert) {
	_mouseSensitivity = 10.f;
	Events::EventCallback callbackMovement = [this](auto && PH1) { handleMovement(std::forward<decltype(PH1)>(PH1)); };
	Events::EventCallback callbackRotation = [this](auto && PH1) { handleRotation(std::forward<decltype(PH1)>(PH1)); };

	EventMan.setActionCallback(
		{kMoveForward, kMoveBackward, kMoveLeft, kMoveRight, kMoveUp, kMoveDown, kIncreaseSpeed, kDecreaseSpeed},
		callbackMovement
	);

	EventMan.setActionCallback(
		{kRotate},
		callbackRotation
	);

	EventMan.addBinding(kMoveForward, Events::kKeyW);
	EventMan.addBinding(kMoveLeft, Events::kKeyA);
	EventMan.addBinding(kMoveBackward, Events::kKeyS);
	EventMan.addBinding(kMoveRight, Events::kKeyD);
	EventMan.addBinding(kMoveUp, Events::kKeyR);
	EventMan.addBinding(kMoveDown, Events::kKeyF);

	EventMan.add2DAxisBinding(kRotate, Events::kMousePosition);

	EventMan.addBinding(kIncreaseSpeed, Events::kKeyT);
	EventMan.addBinding(kDecreaseSpeed, Events::kKeyG);
}

void MouseControlledFreeCamera::update(float delta) {
	_pitch -= glm::radians(_movementRotation.y * delta * _mouseSensitivity) * _invertY;
	_yaw += glm::radians(_movementRotation.x * delta * _mouseSensitivity);
	// roll is not used so far
	_roll += glm::radians(_movementRotation.z * delta * _mouseSensitivity);
	// get rid of stored rotation
	_movementRotation = glm::zero<glm::vec3>();
	// limit pitch to -90..90 degree range
	_pitch = glm::clamp(_pitch, -M_PI_2, M_PI_2);
	_direction.x = cos(_yaw) * cos(_pitch);
	_direction.y = sin(_pitch);
	_direction.z = sin(_yaw) * cos(_pitch);
	_direction = glm::normalize(_direction);

	glm::vec3 right = glm::cross(_up, _direction);

	_position += delta * _movementFactor * _movementDirection.x * 100.0f * right;
	_position += delta * _movementFactor * _movementDirection.y * 100.0f * _up;
	_position += delta * _movementFactor * _movementDirection.z * 100.0f * _direction;
}

void MouseControlledFreeCamera::handleRotation(const Events::Event &event) {
	const auto axisEvent = std::get<Events::AxisEvent<glm::vec2>>(event.data); 
	_movementRotation.x = axisEvent.delta.x; 
	_movementRotation.y = axisEvent.delta.y; 
}

void MouseControlledFreeCamera::handleMovement(const Events::Event &event) {
	const auto keyEvent = std::get<Events::KeyEvent>(event.data);
	switch (event.action) {
		case kMoveUp:       _movementDirection.y = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveDown:     _movementDirection.y = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
		case kMoveForward:  _movementDirection.z = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveBackward: _movementDirection.z = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
		case kMoveLeft:     _movementDirection.x = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveRight:    _movementDirection.x = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
	}

	if (keyEvent.state == Events::kRelease) {
		switch (event.action) {
			case kIncreaseSpeed:
				setMovementFactor(getMovementFactor() * 2.0f);
				break;
			case kDecreaseSpeed:
				setMovementFactor(getMovementFactor() * 0.5f);
				break;
		}
	}
}
