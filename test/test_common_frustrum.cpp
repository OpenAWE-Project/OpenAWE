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

#include "src/common/frustrum.h"

#include <gtest/gtest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST(Frustrum, simpleShape) {
	const glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 2.0f);
	const Common::Frustrum frustrum(view, projection);

	// Points
	//

	// Far outside points
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 0.0f, 10.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 10.0f, 0.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(10.0f, 0.0f, 0.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 0.0f, -10.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, -10.0f, 0.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(-10.0f, 0.0f, 0.0f)));

	// Close outside points
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 0.0f, 0.0f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 0.0f, -2.1f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, 0.0f, -0.9f)));

	// Inner points
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 0.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.0f, 1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.0f, 0.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, -1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.0f, -1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.0f, 0.0f, -1.5f)));

	// close before plane tests
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 1.4999f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.4999f, 1.4999f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.4999f, 0.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, -1.4999f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.4999f, -1.4999f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.4999f, 0.0f, -1.5f)));

	// In-plane tests
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, -1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.0f, -1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(-1.0f, 0.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.0f, 1.0f, -1.5f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(1.0f, 0.0f, -1.5f)));

	// Close after tests
	EXPECT_FALSE(frustrum.test(glm::vec3(0.0f, -1.5001f, -1.5f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(-1.5001f, -1.5001f, -1.5f)));
	EXPECT_FALSE(frustrum.test(glm::vec3(-1.5001f, 0.0f, -1.5f)));

	// Close to near and far plane
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 0.0f, -1.0001f)));
	EXPECT_TRUE(frustrum.test(glm::vec3(0.0f, 0.0f, -1.9999f)));

	// Spheres
	//
	const Common::BoundSphere sphere1{glm::vec3(0.0f), 0.5f};
	const Common::BoundSphere sphere2{glm::vec3(0.0f), 1.5f};
	const Common::BoundSphere sphere3{glm::vec3(0.0f, 0.0f, -1.5f), 1.5f};
	const Common::BoundSphere sphere4{glm::vec3(0.0f, 2.0f, -1.5f), 1.5f};
	const Common::BoundSphere sphere5{glm::vec3(2.0f, 2.0f, -1.5f), 1.5f};
	const Common::BoundSphere sphere6{glm::vec3(0.0f, 0.0f, -3.0f), 0.9f};

	EXPECT_FALSE(frustrum.test(sphere1));
	EXPECT_TRUE(frustrum.test(sphere2));
	EXPECT_TRUE(frustrum.test(sphere3));
	EXPECT_TRUE(frustrum.test(sphere4));
	EXPECT_TRUE(frustrum.test(sphere5));
	EXPECT_FALSE(frustrum.test(sphere6));
}
