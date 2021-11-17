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

#ifndef OPENAWE_FRUSTRUM_H
#define OPENAWE_FRUSTRUM_H

#include <vector>

#include "src/common/types.h"

namespace Common {

/*!
 * \brief Class for testing if shapes are in a frustrum
 *
 * This class gets a view and projection matrices and generates 6 planes from it which are used to check if simplified
 * shapes are in the frustrum. Through that it can be determined which
 */
class Frustrum {
public:
	Frustrum();
	Frustrum(glm::mat4 view, glm::mat4 projection);

	/*!
	 * Set the view matrix for the frustrum and recalculate the planes with both the projection and view matrix
	 * \param view The view matrix
	 */
	void setViewMatrix(glm::mat4 view);

	/*!
	 * Set the projection matrix for the frustrum and recalcualte the planes with both the projection and view matrix
	 * \param projection The projection matrix
	 */
	void setProjectionMatrix(glm::mat4 projection);

	/*!
	 * Test a given point if it lies inside the frustrum
	 * \param point The point to test
	 * \return If the point is in the frustrum
	 */
	bool test(const glm::vec3 &point) const;

	/*!
	 * Test if a given bopunding sphere is contained or intersects the
	 * \param sphere The sphere to test
	 * \return If the sphere is in the frustrum or intersects it
	 */
	bool test(const BoundSphere &sphere) const;

private:
	void generatePlanes();

	std::vector<glm::vec4> _planes;

	glm::mat4 _view, _projection;
};

}

#endif //OPENAWE_FRUSTRUM_H
