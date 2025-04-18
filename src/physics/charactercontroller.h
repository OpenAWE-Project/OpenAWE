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

#ifndef OPENAWE_CHARACTERCONTROLLER_H
#define OPENAWE_CHARACTERCONTROLLER_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "src/physics/collisionobject.h"

namespace Physics {

class CharacterController;

typedef std::shared_ptr<CharacterController> CharacterControllerPtr;

/*!
 * \brief Class for representing controllable elements in the scene
 *
 * This class represents a movable physics object whose velocity can be controlled to achieve a movement in the world.
 * It reacts furthermore to the gravity of the world.
 */
class CharacterController : public CollisionObject {
public:
	CharacterController(float capsuleHeight, float capsuleRadius);

    /*!
     * Set the linear velocity which is a force moving the object to a certain direction
     *
     * \param direction The direction in which the velocity shall move
     */
	void setLinearVelocity(glm::vec3 direction);

    /*!
     * Set the angular velocity which is the force rotating the object around the z Axis.
     *
     * \param factor The signed factor in which to rate around the up axis
     */
	void setAngularVelocity(float factor);

    /*!
     * Make the character jump. Jumping only works if the character is on the ground
     */
	void jump();

    /*!
     * Set the controller active or inactive
     *
     * \param active If the controller should be set active
     */
	void setActive(bool active) override;

    /*!
     * Get the current position of the character
     *
     * \return The current character position as vec3
     */
	glm::vec3 getPosition();

    /*!
     * Get the current position of the character
     * including its ground offset.
     * 
     * \return The current character position as vec3
     */
    glm::vec3 getUpperPosition();
    
    /*!
     * Get the current rotation of the character
     *
     * \return The current rotation as 3x3 matrix
     */
	glm::mat3 getRotation();

private:
	float _groundOffset;
	btPairCachingGhostObject *_ghostObject;
	std::unique_ptr<btCapsuleShape> _shape;
	btKinematicCharacterController *_characterController;
};

} // End of namespace Physics

#endif //OPENAWE_CHARACTERCONTROLLER_H
