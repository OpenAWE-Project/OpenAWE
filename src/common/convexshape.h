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

#ifndef OPENAWE_CONVEXSHAPE_H
#define OPENAWE_CONVEXSHAPE_H

#include <vector>

#include <glm/glm.hpp>

namespace Common {

class ConvexShape {
public:
	ConvexShape() = default;
	ConvexShape(const std::vector<glm::vec2> &points);

	bool intersect(const glm::vec3 &position) const;
	bool intersect(const glm::vec2 &position) const;

private:
	struct Line {
		glm::vec2 p1, p2;
	};

	enum Orientation {
		kCollinear,
		kCW,
		kCCW
	};

	bool intersect(Line l1, Line l2) const;
	std::vector<Line> generateLines(std::vector<glm::vec2> points) const;

	std::vector<Line> _lines;
};

} // End of namespace Common

#endif //OPENAWE_CONVEXSHAPE_H
