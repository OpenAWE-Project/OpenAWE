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

static constexpr uint32_t kRotateLeft     = Common::crc32("FREECAM_ROTATE_LEFT");
static constexpr uint32_t kRotateRight    = Common::crc32("FREECAM_ROTATE_RIGHT");
static constexpr uint32_t kRotateUp       = Common::crc32("FREECAM_ROTATE_UP");
static constexpr uint32_t kRotateDown     = Common::crc32("FREECAM_ROTATE_DOWN");

ControlledFreeCamera::ControlledFreeCamera() {
	Events::EventCallback callback = [this](auto && PH1) { handleEvent(std::forward<decltype(PH1)>(PH1)); };

	EventMan.setActionCallback(
		{kMoveForward, kMoveBackward, kMoveLeft, kMoveRight, kMoveUp, kMoveDown, kRotateLeft, kRotateRight},
		callback
	);

	EventMan.addBinding(kMoveForward, Events::kKeyW);
	EventMan.addBinding(kMoveLeft, Events::kKeyA);
	EventMan.addBinding(kMoveBackward, Events::kKeyS);
	EventMan.addBinding(kMoveRight, Events::kKeyD);
	EventMan.addBinding(kMoveUp, Events::kKeyR);
	EventMan.addBinding(kMoveDown, Events::kKeyF);

	EventMan.addBinding(kRotateLeft, Events::kKeyQ);
	EventMan.addBinding(kRotateRight, Events::kKeyE);
}

void ControlledFreeCamera::handleEvent(const Events::Event &event) {
	const auto keyEvent = std::get<Events::KeyEvent>(event.data);
	switch (event.action) {
		case kMoveUp:       _movementDirection.y = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveDown:     _movementDirection.y = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
		case kMoveForward:  _movementDirection.z = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveBackward: _movementDirection.z = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
		case kMoveLeft:     _movementDirection.x = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kMoveRight:    _movementDirection.x = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;

		case kRotateLeft:  _movementRotation.y = (keyEvent.state == Events::kPress) ?  1.0 : 0.0; break;
		case kRotateRight: _movementRotation.y = (keyEvent.state == Events::kPress) ? -1.0 : 0.0; break;
	}
}
