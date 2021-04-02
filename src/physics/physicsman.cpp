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

#include "physicsman.h"

namespace Physics {

PhysicsManager::PhysicsManager() {
	btDefaultCollisionConfiguration configuration;

	_dispatcher = std::make_unique<btCollisionDispatcher>(&configuration);
	_broadphase = std::make_unique<btDbvtBroadphase>();
	_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	_world = std::make_unique<btDiscreteDynamicsWorld>(
			_dispatcher.get(),
			_broadphase.get(),
			_solver.get(),
			&configuration
	);

	_world->setGravity(btVector3(0, -10, 0));
}

void PhysicsManager::update(float delta) {
	_world->stepSimulation(1.0f, 10);
}

}
