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

#include <memory>

#include <btBulletDynamicsCommon.h>

#include "src/common/readstream.h"
#include "src/common/exception.h"

#include "src/awe/havokfile.h"
#include "src/awe/resman.h"

#include "src/physics/physicsman.h"
#include "src/physics/havokobject.h"

namespace Physics {

HavokObject::HavokObject(rid_t rid) {
	std::unique_ptr<Common::ReadStream> havok(ResMan.getResource(rid));
	if (!havok)
		throw Common::Exception("Havok file for animation not found with the rid {:x}", rid);

	AWE::HavokFile havokFile(*havok);
	const auto &physicsSystem = havokFile.getPhysicsSystem();
	if (physicsSystem.rigidBodies.empty())
		return;

	const auto rigidBody = havokFile.getRigidBody(physicsSystem.rigidBodies.front());
	const auto shape = havokFile.getShape(rigidBody.shape);

	btTransform shapeTransform = btTransform::getIdentity();
	setCollisionShape(getShape(havokFile, shape, shapeTransform));

	btTransform offset;

	offset.setOrigin(btVector3(
		rigidBody.position.x,
		rigidBody.position.y,
		rigidBody.position.z
	));

	offset.setRotation(btQuaternion(
		rigidBody.rotation.x,
		rigidBody.rotation.y,
		rigidBody.rotation.z,
		rigidBody.rotation.w
	));

	setOffset(offset);
	setActive(true);
}

HavokObject::~HavokObject() {
	for (const auto &additionalShape: _additionalShapes) {
		delete additionalShape;
	}
}

btCollisionShape *HavokObject::getShape(AWE::HavokFile &havok, const AWE::HavokFile::hkpShape &shape,
										btTransform &shapeOffset) {
	btCollisionShape *shapeObject;
	switch (shape.type) {
		case AWE::HavokFile::kBox: {
			const auto boxShape = std::get<AWE::HavokFile::hkpBoxShape>(shape.shape);
			btVector3 halfExtents(
					boxShape.halfExtents.x,
					boxShape.halfExtents.y,
					boxShape.halfExtents.z
			);

			shapeObject = new btBoxShape(halfExtents);
			break;
		}

		case AWE::HavokFile::kCylinder: {
			const auto cylinderShape = std::get<AWE::HavokFile::hkpCylinderShape>(shape.shape);
			btVector3 halfExtents(
					cylinderShape.radius,
					glm::distance(cylinderShape.p1, cylinderShape.p2) / 2,
					cylinderShape.radius
			);

			shapeObject = new btCylinderShape(halfExtents);
			break;
		}

		case AWE::HavokFile::kList: {
			const auto listShape = std::get<AWE::HavokFile::hkpListShape>(shape.shape);
			btCompoundShape *compoundShape = new btCompoundShape();

			for (const auto &shapeId: listShape.shapes) {
				btTransform shapeTransform = btTransform::getIdentity();
				const auto childShape = getShape(havok, havok.getShape(shapeId), shapeTransform);
				_additionalShapes.emplace_back(childShape);
				compoundShape->addChildShape(shapeTransform, childShape);
			}

			shapeObject = compoundShape;
			break;
		}

		case AWE::HavokFile::kConvexTranslate: {
			const auto convexTranslateShape = std::get<AWE::HavokFile::hkpConvexTranslateShape>(shape.shape);

			btTransform transform = btTransform::getIdentity();
			transform.setOrigin(btVector3(
				convexTranslateShape.translation.x,
				convexTranslateShape.translation.y,
				convexTranslateShape.translation.z
			));

			shapeOffset *= transform;
			shapeObject = getShape(havok, havok.getShape(convexTranslateShape.shape), transform);

			break;
		}

		default:
			throw Common::Exception("Invalid shape type");
	}

	return shapeObject;
}

} // End of namespace Physics
