//
// Created by patrick on 23.03.21.
//

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
