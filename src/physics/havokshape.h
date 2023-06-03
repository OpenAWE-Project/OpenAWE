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

#ifndef OPENAWE_HAVOKSHAPE_H
#define OPENAWE_HAVOKSHAPE_H

#include "src/awe/havokfile.h"
#include "src/awe/types.h"

#include "src/physics/shape.h"

namespace Physics {

class HavokShape : public Shape {
public:
	HavokShape(rid_t rid);



private:
	btCollisionShape *getShape(
		AWE::HavokFile &havok,
		const AWE::HavokFile::hkpShape &shape,
		btTransform &shapeOffset
	);
};

} // Physics

#endif //OPENAWE_HAVOKSHAPE_H
