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

#include <gtest/gtest.h>

#include <glm/glm.hpp>

#include "src/common/convexshape.h"

TEST(ConvexShape, SimpleShape) {
	const std::vector<glm::vec2> points = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f)
	};

	const Common::ConvexShape shape(points);

	EXPECT_TRUE(shape.intersect(glm::vec2(0.5f, 0.5f)));

	EXPECT_FALSE(shape.intersect(glm::vec2(0.0f, 0.0f)));
	EXPECT_FALSE(shape.intersect(glm::vec2(2.0f, 2.0f)));
	EXPECT_FALSE(shape.intersect(glm::vec2(-1.0f, -1.0f)));
	EXPECT_FALSE(shape.intersect(glm::vec2(0.0f, 0.0f - FLT_MIN)));
}

TEST(ConvexShape, ComplexShape) {
	const std::vector<glm::vec2> points = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(5.0f, 0.0f),
			glm::vec2(5.0f, 2.0f),
			glm::vec2(4.0f, 2.0f),
			glm::vec2(4.0f, 1.0f),
			glm::vec2(3.0f, 1.0f),
			glm::vec2(3.0f, 2.0f),
			glm::vec2(2.0f, 2.0f),
			glm::vec2(2.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 2.0f),
			glm::vec2(0.0f, 2.0f),
			glm::vec2(-10.0f, 1.0f),
	};

	const Common::ConvexShape shape(points);

	EXPECT_TRUE(shape.intersect(glm::vec2(0.5f, 0.5f)));
	EXPECT_TRUE(shape.intersect(glm::vec2(0.5f, 1.5f)));
	EXPECT_TRUE(shape.intersect(glm::vec2(4.5f, 1.5f)));
	EXPECT_TRUE(shape.intersect(glm::vec2(2.5f, 1.5f)));
}
