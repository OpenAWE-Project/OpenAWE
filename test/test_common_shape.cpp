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

	for (int i = 0; i < icoSphere0.positions.size(); ++i) {
		EXPECT_EQ(std::count(icoSphere0.indices.begin(), icoSphere0.indices.end(), i), 5);
	}

	for (int i = 0; i < icoSphere1.positions.size(); ++i) {
		EXPECT_GE(std::count(icoSphere1.indices.begin(), icoSphere1.indices.end(), i), 5);
		EXPECT_LE(std::count(icoSphere1.indices.begin(), icoSphere1.indices.end(), i), 6);
	}

	for (int i = 0; i < icoSphere2.positions.size(); ++i) {
		EXPECT_GE(std::count(icoSphere2.indices.begin(), icoSphere2.indices.end(), i), 5);
		EXPECT_LE(std::count(icoSphere2.indices.begin(), icoSphere2.indices.end(), i), 6);
	}
}
