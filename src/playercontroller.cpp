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

#include "src/common/crc32.h"

#include "playercontroller.h"

static constexpr uint32_t kMoveForward  = Common::crc32("MOVE_FORWARD");
static constexpr uint32_t kMoveBackward = Common::crc32("MOVE_BACKWARD");
static constexpr uint32_t kMoveLeft     = Common::crc32("MOVE_LEFT");
static constexpr uint32_t kMoveRight    = Common::crc32("MOVE_RIGHT");
static constexpr uint32_t kJump         = Common::crc32("JUMP");

PlayerController::PlayerController(entt::registry &registry) : _time(0.0f), _baseDirection(0), _registry(registry) {
	Events::EventCallback callback = [this](auto && PH1) { handleEvent(std::forward<decltype(PH1)>(PH1)); };

	EventMan.setActionCallback(
		{kMoveForward, kMoveBackward, kMoveLeft, kMoveRight, kJump},
		callback
	);

	EventMan.addBinding(kMoveForward, Events::Key::kKeyUp);
	EventMan.addBinding(kMoveBackward, Events::Key::kKeyDown);
	EventMan.addBinding(kMoveLeft, Events::Key::kKeyLeft);
	EventMan.addBinding(kMoveRight, Events::Key::kKeyRight);
	EventMan.addBinding(kJump, Events::Key::kKeySpace);
}

void PlayerController::setPlayerCharacter(entt::entity pc) {
	if (pc == entt::null) {
		_pc.reset();
		return;
	} else {
		_pc = pc;
	}

	_animController = _registry.get<Graphics::AnimationControllerPtr>(*_pc);
	_charController = _registry.get<Physics::CharacterControllerPtr>(*_pc);
}

void PlayerController::update(float time) {
	_time = time;

	if (!_pc)
		return;

    // Set linear velocity according to the current rotation
    _charController->setLinearVelocity(_charController->getRotation() * _baseDirection);

	_registry.patch<Transform>(*_pc, [&](auto &transform){
		transform.setTranslation(_charController->getPosition());
		transform.setRotation(_charController->getRotation());
	});
}

void PlayerController::handleEvent(const Events::Event &event) {
	if (!_pc)
		return;

	const auto keyEvent = std::get<Events::KeyEvent>(event.data);

	if (keyEvent.state == Events::kRelease) {
        switch (event.action) {
            case kMoveForward:
            case kMoveBackward:
                _animController->play("Stand", _time);
                _baseDirection = glm::zero<glm::vec3>();
                break;

            case kMoveLeft:
            case kMoveRight:
                _charController->setAngularVelocity(0.0f);
                break;
        }
	} else {
        switch (event.action) {
            case kMoveForward:
                _animController->play("Walk_Front", _time);
                _baseDirection = glm::vec3(0.0, 0.0, 1.0);
                break;

            case kMoveBackward:
                _animController->play("Walk_Backwards", _time);
                _baseDirection = glm::vec3(0.0, 0.0, -1.0);
                break;

            case kMoveLeft:
                _charController->setAngularVelocity(-1.0f);
                break;

            case kMoveRight:
                _charController->setAngularVelocity(1.0f);
                break;

            case kJump:
                _charController->jump();
                break;
        }
    }
}
