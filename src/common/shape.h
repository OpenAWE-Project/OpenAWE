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

#ifndef OPENAWE_SHAPE_H
#define OPENAWE_SHAPE_H

#include <vector>

#include <glm/glm.hpp>

namespace Common {

/*!
 * Struct for holding the data of the generated shape
 */
struct Shape {
	std::vector<glm::vec3> positions;
	std::vector<uint16_t> indices;
};

/*!
 * Generate an ico sphere shape using a radius and a number of subdivisions
 *
 * \param radius The radius of the generated icosphere
 * \param numSubdivisions The number of subdivisions
 * \return A newly generated icosphere as shape
 */
Shape generateIcoSphere(float radius, unsigned int numSubdivisions);

/*!
 * Reverse the indices of the shape, assuming that the indices represent triangles. Throw an error if that cannot be
 * done because the number of indices doesn't match a multiple of three and therefore cannot be a triangle only mesh
 *
 * \param shape The shape to modify as a reference
 */
void reverseTriangles(Shape &shape);

}

#endif //OPENAWE_SHAPE_H
