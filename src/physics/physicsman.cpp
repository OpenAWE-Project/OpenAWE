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
#include "src/physics/debugdraw.h"

namespace Physics {

PhysicsManager::PhysicsManager() : _debugDraw(false), _configuration(new btDefaultCollisionConfiguration()) {
	_dispatcher = std::make_unique<btCollisionDispatcher>(_configuration.get());
	_broadphase = std::make_unique<btDbvtBroadphase>();
	_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	_world = std::make_unique<btDiscreteDynamicsWorld>(
			_dispatcher.get(),
			_broadphase.get(),
			_solver.get(),
			_configuration.get()
	);

	_debugDrawInterface = std::make_unique<DebugDraw>();

	_world->setGravity(btVector3(0, -9.8, 0));
	_world->setDebugDrawer(_debugDrawInterface.get());
	_world->getDebugDrawer()->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe |
		btIDebugDraw::DBG_DrawContactPoints |
		btIDebugDraw::DBG_DrawText
	);
}

void PhysicsManager::setDebugDraw(bool enable) {
	_debugDraw = enable;
}

void PhysicsManager::update(float delta) {
	_world->stepSimulation(delta, 20);
	if (_debugDraw)
		_world->debugDrawWorld();
}

void PhysicsManager::add(btCollisionObject *collisionObject) {
	_world->addCollisionObject(collisionObject);
}

void PhysicsManager::add(btCollisionObject *collisionObject, int collisionFilterGroup, int collisionFilterMask) {
	_world->addCollisionObject(collisionObject, collisionFilterGroup, collisionFilterMask);
}

void PhysicsManager::add(btRigidBody *collisionObject) {
	_world->addRigidBody(collisionObject);
}

void PhysicsManager::add(btActionInterface *actionInterface) {
	_world->addAction(actionInterface);
}

void PhysicsManager::remove(btCollisionObject *collisionObject) {
	_world->removeCollisionObject(collisionObject);
}

void PhysicsManager::remove(btRigidBody *collisionObject) {
	_world->removeRigidBody(collisionObject);
}

void PhysicsManager::remove(btActionInterface *actionInterface) {
	_world->removeAction(actionInterface);
}

btCollisionWorld::ClosestRayResultCallback PhysicsManager::raycastStatic(btVector3& from, btVector3& to) {
	btCollisionWorld::ClosestRayResultCallback results(from, to);
	results.m_collisionFilterMask = btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter;
	_world->rayTest(from, to, results);
	return results;
}

btCollisionWorld::ClosestConvexResultCallback PhysicsManager::shapeCastStatic(btConvexShape* castShape, btTransform& from, btTransform& to) {
	btCollisionWorld::ClosestConvexResultCallback results(from.getOrigin(), to.getOrigin());
	results.m_collisionFilterMask = btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter;
	_world->convexSweepTest(castShape, from, to, results);
	return results;
}

}
