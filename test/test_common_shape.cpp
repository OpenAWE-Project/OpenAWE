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

#include <string>

#include <gtest/gtest.h>

#include "src/common/shape.h"

TEST(Shape, generateIcoSphere) {
	const Common::Shape icoSphere0 = Common::generateIcoSphere(5.0, 0);
	const Common::Shape icoSphere1 = Common::generateIcoSphere(1.0, 1);
	const Common::Shape icoSphere2 = Common::generateIcoSphere(3.0, 2);

	EXPECT_EQ(icoSphere0.positions.size(), 12);
	EXPECT_EQ(icoSphere0.indices.size(), 60);

	EXPECT_EQ(icoSphere1.positions.size(), 42);
	EXPECT_EQ(icoSphere1.indices.size(), 240);

	EXPECT_EQ(icoSphere2.positions.size(), 162);
	EXPECT_EQ(icoSphere2.indices.size(), 960);

	for (const auto &position: icoSphere0.positions) {
		EXPECT_NEAR(glm::length(position), 5.0, 0.0001);
	}

	for (const auto &position: icoSphere1.positions) {
		EXPECT_NEAR(glm::length(position), 1.0, 0.0001);
	}

	for (const auto &position: icoSphere2.positions) {
		EXPECT_NEAR(glm::length(position), 3.0, 0.0001);
	}

	for (unsigned int i = 0; i < icoSphere0.positions.size(); ++i) {
		EXPECT_EQ(std::count(icoSphere0.indices.begin(), icoSphere0.indices.end(), i), 5);
	}

	for (unsigned int i = 0; i < icoSphere1.positions.size(); ++i) {
		EXPECT_GE(std::count(icoSphere1.indices.begin(), icoSphere1.indices.end(), i), 5);
		EXPECT_LE(std::count(icoSphere1.indices.begin(), icoSphere1.indices.end(), i), 6);
	}

	for (unsigned int i = 0; i < icoSphere2.positions.size(); ++i) {
		EXPECT_GE(std::count(icoSphere2.indices.begin(), icoSphere2.indices.end(), i), 5);
		EXPECT_LE(std::count(icoSphere2.indices.begin(), icoSphere2.indices.end(), i), 6);
	}
}

TEST(Shape, generatePyramid) {
	const auto pyramid0 = Common::generatePyramid(10, 90.0f);
	const auto pyramid1 = Common::generatePyramid(10, 45.0f);

	EXPECT_EQ(pyramid0.positions.size(), 5);
	EXPECT_EQ(pyramid0.indices.size(), 18);

	EXPECT_EQ(pyramid1.positions.size(), 5);
	EXPECT_EQ(pyramid1.indices.size(), 18);

	for (const auto &position: pyramid0.positions) {
		if (position.y == 0.0f) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 10.0f);
			EXPECT_FLOAT_EQ(std::abs(position.z), 10.0f);
		} else {
			EXPECT_EQ(position.x, 0.0f);
			EXPECT_EQ(position.z, 0.0f);
			EXPECT_EQ(position.y, 10.0f);
		}
	}

	for (const auto &position: pyramid1.positions) {
		if (position.y == 0.0f) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 4.1421356);
			EXPECT_FLOAT_EQ(std::abs(position.z), 4.1421356);
		} else {
			EXPECT_EQ(position.x, 0.0f);
			EXPECT_EQ(position.z, 0.0f);
			EXPECT_EQ(position.y, 10.0f);
		}
	}

	for (const auto &index1: pyramid0.indices) {
		EXPECT_LT(index1, pyramid0.positions.size());
	}

	for (const auto &index1: pyramid1.indices) {
		EXPECT_LT(index1, pyramid1.positions.size());
	}
}

TEST(Shape, generateFrustrum) {
	const auto frustrum0 = Common::generateFrustrum(1, 10, 90.0f);
	const auto frustrum1 = Common::generateFrustrum(1, 10, 45.0f);

	EXPECT_EQ(frustrum0.positions.size(), 8);
	EXPECT_EQ(frustrum0.indices.size(), 36);

	EXPECT_EQ(frustrum1.positions.size(), 8);
	EXPECT_EQ(frustrum1.indices.size(), 36);

	for (const auto &position: frustrum0.positions) {
		if (position.y == 0.0) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 10.0f);
			EXPECT_FLOAT_EQ(std::abs(position.z), 10.0f);
		} else if (position.y == 9.0) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 1.0f);
			EXPECT_FLOAT_EQ(std::abs(position.z), 1.0f);
		} else {
			FAIL();
		}
	}

	for (const auto &position: frustrum1.positions) {
		if (position.y == 0.0) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 4.1421356);
			EXPECT_FLOAT_EQ(std::abs(position.z), 4.1421356);
		} else if (position.y == 9.0) {
			EXPECT_FLOAT_EQ(std::abs(position.x), 0.41421357);
			EXPECT_FLOAT_EQ(std::abs(position.z), 0.41421357);
		} else {
			FAIL();
		}
	}

	for (const auto &index1: frustrum0.indices) {
		EXPECT_LT(index1, frustrum0.positions.size());
	}

	for (const auto &index1: frustrum1.indices) {
		EXPECT_LT(index1, frustrum1.positions.size());
	}
}

TEST(Shape, reverseTriangles) {
	Common::Shape icoSphere = Common::generateIcoSphere(5.0, 2);
	const Common::Shape icoSphereOld = icoSphere;

	EXPECT_EQ(icoSphere.positions.size(), 162);
	EXPECT_EQ(icoSphere.indices.size(), 960);

	Common::reverseTriangles(icoSphere);

	EXPECT_EQ(icoSphere.positions.size(), 162);
	EXPECT_EQ(icoSphere.indices.size(), 960);

	for (size_t i = 0; i < icoSphere.indices.size() / 3; i+=3) {
		EXPECT_EQ(icoSphere.indices[i * 3]    , icoSphereOld.indices[i * 3 + 2]);
		EXPECT_EQ(icoSphere.indices[i * 3 + 1], icoSphereOld.indices[i * 3 + 1]);
		EXPECT_EQ(icoSphere.indices[i * 3 + 2], icoSphereOld.indices[i * 3]);
	}
}
