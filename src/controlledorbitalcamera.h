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

#ifndef OPENAWE_CONTROLLEDORBITALCAMERA_H
#define OPENAWE_CONTROLLEDORBITALCAMERA_H

#include <memory>

#include <bullet/btBulletDynamicsCommon.h>

#include "src/events/event.h"

#include "src/graphics/orbitalcamera.h"

#include "src/physics/charactercontroller.h"

class ControlledOrbitalCamera : public Graphics::OrbitalCamera {
public:
	ControlledOrbitalCamera();

	void attachTo(Physics::CharacterControllerPtr object);

	void update(float delta) override;

private:
	void handleRotation(const Events::Event &event);
	void handleStates(const Events::Event &event);

	Physics::CharacterControllerPtr _followedObject;

	btSphereShape _castSphere;
};

typedef std::shared_ptr<ControlledOrbitalCamera> ControlledOrbitalCameraPtr;


#endif //OPENAWE_CONTROLLEDORBITALCAMERA_H
