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

#ifndef AWE_PHYSICSMAN_H
#define AWE_PHYSICSMAN_H

#include <memory>

#include <bullet/btBulletDynamicsCommon.h>

#include "src/common/singleton.h"

namespace Physics {

class PhysicsManager : public Common::Singleton<PhysicsManager> {
public:
	PhysicsManager();

	void setDebugDraw(bool enable);

	void update(float delta);

	void add(btCollisionObject *collisionObject);
	void remove(btCollisionObject *collisionObject);

private:
	bool _debugDraw;

	std::unique_ptr<btCollisionDispatcher> _dispatcher;
	std::unique_ptr<btBroadphaseInterface> _broadphase;
	std::unique_ptr<btSequentialImpulseConstraintSolver> _solver;
	std::unique_ptr<btDiscreteDynamicsWorld> _world;
	std::unique_ptr<btIDebugDraw> _debugDrawInterface;
};

} // End of Physics

#define PhysicsMan Physics::PhysicsManager::instance()

#endif //AWE_PHYSICSMAN_H
