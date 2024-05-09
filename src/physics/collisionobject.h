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

#ifndef OPENAWE_COLLISIONOBJECT_H
#define OPENAWE_COLLISIONOBJECT_H

#include <memory>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "src/awe/collisionsfile.h"
#include "src/awe/havokfile.h"
#include "src/awe/types.h"

namespace Physics {

class CollisionObject;

typedef std::shared_ptr<CollisionObject> CollisionObjectPtr;

class CollisionObject {
public:
	void setTransform(const glm::vec3 &position, const glm::mat3 &rotation);
	virtual void setActive(bool active);

protected:
	CollisionObject();

	void setCollisionShape(btCollisionShape *shape);
	void setCollisionObject(btCollisionObject *collisionObject);
	void setOffset(btTransform offset);

	btCollisionShape &getCollisionShape();

	bool _active{};

private:
	btTransform _offset;
	std::unique_ptr<btCollisionShape> _shape;
	std::unique_ptr<btCollisionObject> _collision;
};

} // End of namespace Physics

#endif //OPENAWE_COLLISIONOBJECT_H
