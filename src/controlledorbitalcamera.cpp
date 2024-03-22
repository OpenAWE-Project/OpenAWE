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

#include "src/events/eventman.h"

#include "src/physics/charactercontroller.h"
#include "src/physics/physicsman.h"

#include "src/controlledorbitalcamera.h"

static constexpr uint32_t kRotateMouse    = Common::crc32("ORBITALCAM_ROTATE_MOUSE");
static constexpr uint32_t kRotateGamepad  = Common::crc32("ORBITALCAM_ROTATE_GAMEPAD");

static constexpr uint32_t kSwitchShoulder = Common::crc32("ORBITALCAM_SWITCH_SHOULDER");
static constexpr uint32_t kAimWeapon      = Common::crc32("ORBITALCAM_AIM_WEAPON");
static constexpr uint32_t kFocusCamera    = Common::crc32("ORBITALCAM_FOCUS_CAMERA");

ControlledOrbitalCamera::ControlledOrbitalCamera() : _castSphere(0.75f) {
	Events::EventCallback callbackRotate = [this](auto && PH1) { handleRotation(std::forward<decltype(PH1)>(PH1)); };
	Events::EventCallback callbackStates = [this](auto && PH1) { handleStates(std::forward<decltype(PH1)>(PH1)); };

	EventMan.setActionCallback(
		{kSwitchShoulder, kAimWeapon, kFocusCamera},
		callbackStates
	);

	EventMan.setActionCallback(
		{kRotateMouse, kRotateGamepad},
		callbackRotate
	);

	EventMan.addBinding(kSwitchShoulder, Events::kKeyTab);
	EventMan.addBinding(kAimWeapon, Events::kMouseLeft);
	EventMan.addBinding(kFocusCamera, Events::kKeyF);

	EventMan.addBinding(kSwitchShoulder, Events::kGamepadButtonLeftBumper);
	EventMan.addBinding(kAimWeapon, Events::kGamepadButtonRightBumper);
	EventMan.addBinding(kFocusCamera, Events::kGamepadButtonA);

	EventMan.add2DAxisBinding(kRotateMouse, Events::kMousePosition);
	EventMan.add2DAxisBinding(kRotateGamepad, Events::kGamepadAxisRight);
}

void ControlledOrbitalCamera::handleRotation(const Events::Event &event) {
	const Events::AxisEvent<glm::vec2> axisEvent = std::get<Events::AxisEvent<glm::vec2>>(event.data);
	if (event.action == kRotateMouse) {
		_movementRotation.x += axisEvent.delta.x * _mouseSensitivity;
		_movementRotation.y += axisEvent.delta.y * _mouseSensitivity;
	} else if (event.action == kRotateGamepad) {
		_movementRotation.x += axisEvent.absolute.x * _gamepadSensitivity;
		_movementRotation.y += axisEvent.absolute.y * _gamepadSensitivity;
	}
}

void ControlledOrbitalCamera::handleStates(const Events::Event &event) {
	const Events::KeyEvent keyEvent = std::get<Events::KeyEvent>(event.data);
	if (event.action == kAimWeapon) {
		if (keyEvent.state == Events::kPress && _state == Graphics::kCameraDefault) {
			setAiming(true);
		} else if (keyEvent.state == Events::kRelease && _state == Graphics::kCameraAiming) {
			setAiming(false);
		}
	} else if (keyEvent.state == Events::kPress) {
		if (event.action == kSwitchShoulder) {
			switchShoulder();
		} else if (event.action == kFocusCamera) {
			if (_state == Graphics::kCameraDefault) {
				focusOn(glm::vec3(1000, 1000, 1000));
			} else if (_state == Graphics::kCameraFocused) {
				unfocus();
			}
		}
	}
}

void ControlledOrbitalCamera::attachTo(Physics::CharacterControllerPtr object) {
	_followedObject = object;
	glm::vec3 newOrigin = _followedObject->getUpperPosition();
	newOrigin.y += 0.6f;
	setOrbitOrigin(newOrigin);
	_orbitOriginCurrent = _orbitOriginTarget;
	_rotationOrientation = glm::vec3(-M_PI, 0.0f, 0.0f) * object->getRotation();
	_position = calcOrbitPosition(_orbitRadiusCurrent);
	_direction = _rotationDirection;
}

void ControlledOrbitalCamera::update(float delta) {
	glm::vec3 newOrigin = _followedObject->getUpperPosition();
	newOrigin.y += 0.6f;
	setOrbitOrigin(newOrigin);
	Graphics::OrbitalCamera::update(delta);
	// Resolving colisions with static objects
	const glm::vec4 
		mat1(1.f, 0.f, 0.f, 0.f),
		mat2(0.f, 1.f, 0.f, 0.f),
		mat3(0.f, 0.f, 1.f, 0.f);
	const glm::vec3 basePoint{calcOrbitPosition(_orbitRadiusTargetFull)};
	const glm::vec4 cameraOrigin{_orbitOriginCurrent.x, _orbitOriginCurrent.y, -_orbitOriginCurrent.z, 1.f};
	glm::mat4
		from{mat1, mat2, mat3, cameraOrigin}, 
		to{mat1, mat2, mat3, glm::vec4(basePoint.x, basePoint.y, -basePoint.z, 1.f)};
	Physics::CastResult camCast = PhysicsMan.shapeCastStatic(&_castSphere, from, to);
	if (camCast.hasHit) {
		float rad = glm::length(camCast.rayHitPoint - glm::vec3(_orbitOriginCurrent.x, _orbitOriginCurrent.y, -_orbitOriginCurrent.z));
		snapRadius(rad);
	};
}