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

#include <optional>

#include <glm/geometric.hpp>

#include "convexshape.h"

namespace Common {

ConvexShape::ConvexShape(const std::vector<glm::vec2> &points) : _lines(generateLines(points)) {

}

bool ConvexShape::intersect(const glm::vec3 &position) const {
	return intersect(glm::vec2(position.x, position.y));
}

bool ConvexShape::intersect(const glm::vec2 &position) const {
	/*
	 * Shoot a ray out of the testing point and check, if it intersects with an odd number of lines
	 */
	Line ray;
	ray.p1 = position;
	ray.p2 = glm::vec2(10000, position.y);

	unsigned int intersectedLines = 0;
	for (const auto &line : _lines) {
		if (intersect(line, ray))
			intersectedLines++;
	}

	return intersectedLines % 2 == 1;
}

bool ConvexShape::intersect(ConvexShape::Line l1, ConvexShape::Line l2) const {
	/*
	 * Line intersection algorithm taken from:
	 * https://wiki2.org/en/Line%E2%80%93line_intersectionv
	 */
	const float x1 = l1.p1.x;
	const float x2 = l1.p2.x;
	const float x3 = l2.p1.x;
	const float x4 = l2.p2.x;
	const float y1 = l1.p1.y;
	const float y2 = l1.p2.y;
	const float y3 = l2.p1.y;
	const float y4 = l2.p2.y;

	// Check if the lines are paralell
	const float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (d == 0)
		return false;

	// Calculate the intersection point
	const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
	const float u = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

	const glm::vec2 p1 = glm::vec2(
			x1 + t * (x2 - x1),
			y1 + t * (y2 - y1)
	);
	const glm::vec2 p2 = glm::vec2(
			x3 + u * (x4 - x3),
			y3 + u * (y4 - y3)
	);

	// And check if it is in the range of both line segments
	bool inL1 =
			std::min(x1, x2) <= p1.x && p1.x <= std::max(x1, x2) &&
			std::min(y1, y2) <= p1.y && p1.y <= std::max(y1, y2);
	bool inL2 =
			std::min(x3, x4) <= p2.x && p2.x <= std::max(x3, x4) &&
			std::min(y3, y4) <= p2.y && p2.y <= std::max(y3, y4);

	return inL1 && inL2;
}

std::vector<ConvexShape::Line> ConvexShape::generateLines(std::vector<glm::vec2> points) const {
	std::vector<Line> lines;
	std::optional<glm::vec2> p1;
	for (const auto &point : points) {
		if (!p1) {
			p1 = point;
		} else {
			Line line;
			line.p1 = *p1;
			line.p2 = point;
			lines.emplace_back(line);
			p1 = point;
		}
	}

	if (points.size() > 2) {
		Line beginEndLine;
		beginEndLine.p1 = points.front();
		beginEndLine.p2 = points.back();
		lines.emplace_back(beginEndLine);
	}

	return lines;
}

}
