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

#include <glm/gtx/quaternion.hpp>

#include "src/common/exception.h"

#include "src/awe/resman.h"

#include "src/physics/havokshape.h"

namespace Physics {

HavokShape::HavokShape(rid_t rid) {
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
	_rootShape = getShape(havokFile, shape, shapeTransform);

	_offset.setIdentity();
	_offset.setOrigin(btVector3(
			rigidBody.position.x,
			rigidBody.position.y,
			rigidBody.position.z
	));
	_offset.setRotation(btQuaternion(
			rigidBody.rotation.x,
			rigidBody.rotation.y,
			rigidBody.rotation.z,
			rigidBody.rotation.w
	));
}

btCollisionShape *HavokShape::getShape(AWE::HavokFile &havok, const AWE::HavokFile::hkpShape &shape,
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

		case AWE::HavokFile::kCapsule: {
			const auto capsuleShape = std::get<AWE::HavokFile::hkpCapsuleShape>(shape.shape);

			shapeObject = new btCapsuleShape(
				shape.radius,
				glm::distance(capsuleShape.p1.xyz(), capsuleShape.p2.xyz())
			);

			const auto midPoint = glm::mix(capsuleShape.p1.xyz(), capsuleShape.p2.xyz(), 0.5f);
			shapeOffset.setOrigin(btVector3(midPoint.x, midPoint.y, midPoint.z));

			const auto rotationAxis = glm::cross(
				glm::normalize(capsuleShape.p1.xyz()),
				glm::vec3(0.0, 1.0, 0.0)
			);

			const auto rotationAngle = std::acos(glm::dot(
				glm::normalize(capsuleShape.p1.xyz()),
				glm::vec3(0.0, 1.0, 0.0)
			));

			shapeOffset.setRotation(btQuaternion(
				btVector3(rotationAxis.x, rotationAxis.y, rotationAxis.z).normalize(),
				rotationAngle
			).normalize());

			break;
		}

		case AWE::HavokFile::kSimpleMesh: {
			const auto simpleMeshShape = std::get<AWE::HavokFile::hkpSimpleMeshShape>(shape.shape);

			btTriangleMesh *triangleMesh = _meshes.emplace_back(new btTriangleMesh);

			for (unsigned int i = 0; i < simpleMeshShape.indices.size(); i+=3) {
				const auto v1 = simpleMeshShape.vertices[simpleMeshShape.indices[i]];
				const auto v2 = simpleMeshShape.vertices[simpleMeshShape.indices[i + 1]];
				const auto v3 = simpleMeshShape.vertices[simpleMeshShape.indices[i + 2]];
				triangleMesh->addTriangle(
					btVector3(v1.x, v1.y, v1.z),
					btVector3(v2.x, v2.y, v2.z),
					btVector3(v3.x, v3.y, v3.z)
				);
			}

			shapeObject = new btBvhTriangleMeshShape(triangleMesh, true);

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

		case AWE::HavokFile::kMoppBvTreeShape: {
			// TODO: It is currently just taking the child shape. It should generate a BVH and give it to the child mesh
			const auto moppTreeShape = std::get<AWE::HavokFile::hkpMoppBvTreeShape>(shape.shape);

			shapeObject = getShape(havok, havok.getShape(moppTreeShape.childShape), shapeOffset);
			break;
		}

		case AWE::HavokFile::kStorageExtendedMeshShape: {
			const auto storageMeshShape = std::get<AWE::HavokFile::hkpStorageExtendedMeshShape>(shape.shape);

			btTriangleMesh *fullMesh = _meshes.emplace_back(new btTriangleMesh);

			for (const auto &meshStorage: storageMeshShape.meshStorage) {
				const auto meshSubpartStorage = havok.getMeshSubpartStorage(meshStorage);

				const auto &vertices = meshSubpartStorage.vertices;
				std::vector<uint32_t> indices;
				indices.insert(indices.end(), meshSubpartStorage.indices8.begin(), meshSubpartStorage.indices8.end());
				indices.insert(indices.end(), meshSubpartStorage.indices16.begin(), meshSubpartStorage.indices16.end());
				indices.insert(indices.end(), meshSubpartStorage.indices32.begin(), meshSubpartStorage.indices32.end());

				for (unsigned int i = 0; i < indices.size(); i+=4) {
					const auto v1 = vertices[indices[i]];
					const auto v2 = vertices[indices[i + 1]];
					const auto v3 = vertices[indices[i + 2]];

					fullMesh->addTriangle(
						btVector3(v1.x, v1.y, v1.z),
						btVector3(v2.x, v2.y, v2.z),
						btVector3(v3.x, v3.y, v3.z)
					);
				}
			}

			btBvhTriangleMeshShape *meshShape = new btBvhTriangleMeshShape(fullMesh, true);
			shapeObject = meshShape;

			break;
		}

		case AWE::HavokFile::kConvexVerticesShape: {
			const auto convexVerticesShape = std::get<AWE::HavokFile::hkpConvexVerticesShape>(shape.shape);

			Common::BoundBox aabb(
				convexVerticesShape.aabbCenter + convexVerticesShape.aabbHalfExtents,
				convexVerticesShape.aabbCenter - convexVerticesShape.aabbHalfExtents
			);
			for (const auto &vertices: convexVerticesShape.rotatedVertices) {
				assert(aabb.isInside(vertices[0]));
				assert(aabb.isInside(vertices[1]));
				assert(aabb.isInside(vertices[2]));
				assert(aabb.isInside(vertices[3]));
			}

			auto *hullShape = new btConvexHullShape(
				reinterpret_cast<const btScalar*>(convexVerticesShape.rotatedVertices.data()),
				convexVerticesShape.numVertices,
				sizeof(glm::vec3)
			);

			hullShape->optimizeConvexHull();
			hullShape->initializePolyhedralFeatures();

			shapeObject = hullShape;

			break;
		}

        case AWE::HavokFile::kConvexTransform: {
            const auto convexTransformShape = std::get<AWE::HavokFile::hkpConvexTransformShape>(shape.shape);

            btTransform transform = btTransform::getIdentity();
            transform.setOrigin(btVector3(
                    convexTransformShape.translation.x,
                    convexTransformShape.translation.y,
                    convexTransformShape.translation.z
            ));

            glm::quat rotation = glm::toQuat(convexTransformShape.rotation);
            transform.setRotation(btQuaternion(
                    rotation.x,
                    rotation.y,
                    rotation.z,
                    rotation.w
            ));

            shapeOffset *= transform;
            shapeObject = getShape(havok, havok.getShape(convexTransformShape.shape), transform);

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
            throw CreateException("Invalid shape type");
    }

    return shapeObject;
}

} // Physics