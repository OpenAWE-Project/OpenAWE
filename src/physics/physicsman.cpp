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
#define GLM_ENABLE_EXPERIMANTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "LinearMath/btQuaternion.h"
#include "LinearMath/btVector3.h"

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

const CastResult PhysicsManager::raycastStatic(glm::vec3& from, glm::vec3& to) {
	btVector3 bFrom{from.x, from.y, from.z}, bTo{to.x, to.y, to.z}; 
	btCollisionWorld::ClosestRayResultCallback results(bFrom, bTo);
	results.m_collisionFilterMask = btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter;

	_world->rayTest(bFrom, bTo, results);
	if (results.hasHit()) {
		glm::vec3 rayHitPoint{
			results.m_hitPointWorld.x(),
			results.m_hitPointWorld.y(),
			results.m_hitPointWorld.z()
		};
		return CastResult{
			true,
			rayHitPoint,
			rayHitPoint
		};	
	};

	return CastResult{
		false,
		glm::zero<glm::vec3>(),
		glm::zero<glm::vec3>()
	};
}

const CastResult PhysicsManager::shapeCastStatic(btConvexShape* castShape, glm::mat4& from, glm::mat4& to) {
	// To make btTransform, we'll need to get orientation quaternion and translation vector
	// glm::decompose API returns a lot of params, so we'll try to ignore some
	glm::vec3 null3;
	glm::vec4 null4;
	glm::vec3 fromTranslation, toTranslation;
	glm::quat fromRotation, toRotation;
	glm::decompose(from, null3, fromRotation, fromTranslation, null3, null4);
	glm::decompose(to, null3, toRotation, toTranslation, null3, null4);
	const btVector3 bFrom{fromTranslation.x, fromTranslation.y, fromTranslation.z}, bTo{toTranslation.x, toTranslation.y, toTranslation.z}; 
	const btQuaternion bFromQuat{fromRotation.x, fromRotation.y, fromRotation.z, fromRotation.w}, bToQuat{toRotation.x, toRotation.y, toRotation.z, toRotation.w};
	const btTransform btFrom{bFromQuat, bFrom}, btTo{bToQuat, bTo};
	btCollisionWorld::ClosestConvexResultCallback results(bFrom, bTo);
	results.m_collisionFilterMask = btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter;

	_world->convexSweepTest(castShape, btFrom, btTo, results);
	// Since hit point can be anywhere on the shape, we should project hit point onto movement ray
	const glm::vec3 hitPoint{results.m_hitPointWorld.x(), results.m_hitPointWorld.y(), results.m_hitPointWorld.z()};
	const glm::vec3 origVec = toTranslation - fromTranslation;
	const glm::vec3 hitVec = hitPoint - fromTranslation;
	const glm::vec3 projectedHit = glm::dot(hitVec, origVec) / glm::dot(origVec, origVec) * origVec;
	return CastResult {
		results.hasHit(),
		projectedHit + fromTranslation,
		hitPoint
	};
}

}
