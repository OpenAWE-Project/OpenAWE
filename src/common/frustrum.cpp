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

#include <glm/ext/matrix_transform.hpp>

#include "frustrum.h"

/*
 * Frustrum calculations based on:
 * https://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
 */

namespace Common {

Frustrum::Frustrum() : _projection(glm::identity<glm::mat4>()), _view(glm::identity<glm::mat4>()) {
}

Frustrum::Frustrum(glm::mat4 view, glm::mat4 projection) : _projection(projection) {

}

void Frustrum::setViewMatrix(glm::mat4 view) {
	_view = view;
	generatePlanes();
}

void Frustrum::setProjectionMatrix(glm::mat4 projection) {
	_projection = projection;
	generatePlanes();
}

bool Frustrum::test(const glm::vec3 &point) const {
	for (const auto &plane: _planes) {
		const float d = glm::dot(glm::vec4(point, 1.0f), plane);
		if (d <= 0)
			return false;
	}

	return true;
}

bool Frustrum::test(const BoundSphere &sphere) const {
	for (const auto &plane: _planes) {
		const float d = glm::dot(glm::vec4(sphere.position, 1.0f), plane);
		if (d + sphere.radius <= 0)
			return false;
	}

	return true;
}

void Frustrum::generatePlanes() {
	const glm::mat4 vp = glm::transpose(_projection * _view);

	_planes.resize(6);

	// Left plane
	_planes[0] = vp[3] + vp[0];

	// Right plane
	_planes[1] = vp[3] - vp[0];

	// Top plane
	_planes[2] = vp[3] - vp[1];

	// Bottom plane
	_planes[3] = vp[3] + vp[1];

	// Near Plane
	_planes[4] = vp[3] + vp[2];

	// Far plane
	_planes[5] = vp[3] - vp[2];

	// Normalize plane equations
	for (auto &plane: _planes) {
		plane /= glm::length(glm::vec3(plane));
	}
}

} // End of namespace Common
