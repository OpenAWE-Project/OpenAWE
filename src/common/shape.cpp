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

#include <algorithm>
#include <map>

#include <glm/vec4.hpp>
#include <glm/gtx/vec_swizzle.hpp>

#include "src/common/shape.h"
#include "src/common/exception.h"

namespace Common {

Shape loopSubdivision(const Shape &shape) {
	// Get all triangles from the shape
	std::vector<glm::u16vec3> triangles;
	for (unsigned int i = 0; i < shape.indices.size()/3; i++) {
		triangles.emplace_back(
			shape.indices[i * 3],
			shape.indices[i * 3 + 1],
			shape.indices[i * 3 + 2]
		);
	}

	// Add all old points to the shape
	Common::Shape newShape;
	for (const auto &position: shape.positions) {
		newShape.positions.emplace_back(position);
	}

	// Map for storing generated vertices
	std::map<std::tuple<uint16_t, uint16_t>, uint16_t> newVertices;

	/*
	 * Loop over all triangles to generate for every edge an interpolated vertex and interconenct them. If a vertex
	 * is already generated, retrieve it from the map
	 */
	for (const auto &triangle: triangles) {
		// Get the indices of the triangle
		const auto index1 = triangle[0];
		const auto index2 = triangle[1];
		const auto index3 = triangle[2];

		// Get the old vertices
		const auto v1 = shape.positions[index1];
		const auto v2 = shape.positions[index2];
		const auto v3 = shape.positions[index3];

		// Generate new vertices
		const auto nv1 = glm::mix(v1, v2, 0.5f);
		const auto nv2 = glm::mix(v2, v3, 0.5f);
		const auto nv3 = glm::mix(v3, v1, 0.5f);

		// Make identification tuples for the vertex on a specific edge
		const auto nv1t = std::make_tuple(std::min(index1, index2), std::max(index1, index2));
		const auto nv2t = std::make_tuple(std::min(index2, index3), std::max(index2, index3));
		const auto nv3t = std::make_tuple(std::min(index3, index1), std::max(index3, index1));

		uint16_t nv1i, nv2i, nv3i;
		if (newVertices.find( nv1t) != newVertices.end()) {
			nv1i = newVertices[nv1t];
		} else {
			nv1i = newShape.positions.size();
			newShape.positions.emplace_back(nv1);
			newVertices[nv1t] = nv1i;
		}
		if (newVertices.find( nv2t) != newVertices.end()) {
			nv2i = newVertices[nv2t];
		} else {
			nv2i = newShape.positions.size();
			newShape.positions.emplace_back(nv2);
			newVertices[nv2t] = nv2i;
		}
		if (newVertices.find( nv3t) != newVertices.end()) {
			nv3i = newVertices[nv3t];
		} else {
			nv3i = newShape.positions.size();
			newShape.positions.emplace_back(nv3);
			newVertices[nv3t] = nv3i;
		}

		newShape.indices.emplace_back(index1);
		newShape.indices.emplace_back(nv1i);
		newShape.indices.emplace_back(nv3i);

		newShape.indices.emplace_back(index2);
		newShape.indices.emplace_back(nv2i);
		newShape.indices.emplace_back(nv1i);

		newShape.indices.emplace_back(index3);
		newShape.indices.emplace_back(nv3i);
		newShape.indices.emplace_back(nv2i);

		newShape.indices.emplace_back(nv1i);
		newShape.indices.emplace_back(nv2i);
		newShape.indices.emplace_back(nv3i);
	}

	return newShape;
}

Shape generateIcoSphere(float radius, unsigned int numSubdivisions) {
	Shape icoSphere;

	// Calculate some parameters for the base icosahedron
	const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f;
	const float a = 1.0f;
	const float b = 1.0f / phi;

	// Generate base icosahedron
	icoSphere.positions.emplace_back(0, b, -a);
	icoSphere.positions.emplace_back(b, a, 0);
	icoSphere.positions.emplace_back(-b, a, 0);
	icoSphere.positions.emplace_back(0, b, a);
	icoSphere.positions.emplace_back(0, -b, a);
	icoSphere.positions.emplace_back(-a, 0, b);
	icoSphere.positions.emplace_back(0, -b, -a);
	icoSphere.positions.emplace_back(a, 0, -b);
	icoSphere.positions.emplace_back(a, 0, b);
	icoSphere.positions.emplace_back(-a, 0, -b);
	icoSphere.positions.emplace_back(b, -a, 0);
	icoSphere.positions.emplace_back(-b, -a, 0);

	icoSphere.indices = {
		2, 1, 0,
		1, 2, 3,
		5, 4, 3,
		4, 8, 3,
		7, 6, 0,
		6, 9, 0,
		11, 10, 4,
		10, 11, 6,
		9, 5, 2,
		5, 9, 11,
		8, 7, 1,
		7, 8, 10,
		2, 5, 3,
		8, 1, 3,
		9, 2, 0,
		1, 7, 0,
		11, 9, 6,
		7, 10, 6,
		5, 11, 4,
		10, 8, 4
	};

	// Loop over subdivisions
	for (unsigned int i = 0; i < numSubdivisions; ++i) {
		icoSphere = loopSubdivision(icoSphere);
	}

	// Normalize the positions to a unit sphere
	for (auto &position: icoSphere.positions) {
		position = glm::normalize(position);
	}

	// scale the sphere up to a radius
	for (auto &position: icoSphere.positions) {
		position *= radius;
	}

	return icoSphere;
}

void reverseTriangles(Shape &shape) {
	if (shape.indices.size() % 3 != 0)
		throw CreateException("This shape does not contain valid triangles");

	for (size_t i = 0; i < shape.indices.size(); i+=3) {
		std::reverse(
			shape.indices.begin() + i,
			shape.indices.begin() + i + 2
		);
	}
}

Shape generateCone(float radius, float height, unsigned int resolution) {
	Shape coneShape;

	// Generate base vertices
	for (unsigned int i = 0; i < resolution; ++i) {
		const float ratio = static_cast<float>(i) / static_cast<float>(resolution);
		const float r = ratio * (M_PI * 2.0f);
		const float x = std::cos(r) * radius;
		const float y = std::sin(r) * radius;
		coneShape.positions.emplace_back(x, y, 0.0f);
	}

	// Generate base indices
	for (unsigned int i = 1; i < resolution - 1; ++i) {
		coneShape.indices.emplace_back(0);
		coneShape.indices.emplace_back(i);
		coneShape.indices.emplace_back(i + 1);
	}
	
	// Generate top vertex
	coneShape.positions.emplace_back(0.0, height, 0.0);
	
	// Generate base to top indices
	const uint16_t topIndex = coneShape.positions.size() - 1;
	for (unsigned int i = 0; i < resolution; ++i) {
		coneShape.indices.emplace_back((i + 1) % resolution);
		coneShape.indices.emplace_back(i);
		coneShape.indices.emplace_back(topIndex);
	}

	return coneShape;
}

}
