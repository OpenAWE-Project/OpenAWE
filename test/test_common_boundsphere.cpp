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

#include "src/common/types.h"

TEST(BoundSphere, combine) {
	const Common::BoundSphere sphere1{{0.0, 0.0, 0.0}, 1.0};
	const Common::BoundSphere sphere2{{1.0, 0.0, 0.0}, 1.0};
	const Common::BoundSphere sphere3{{2.0, 0.0, 0.0}, 1.0};

	const auto combinedSphere1 = Common::combine(sphere1, sphere2);
	const auto combinedSphere2 = Common::combine(sphere1, sphere3);

	EXPECT_FLOAT_EQ(combinedSphere1.position.x, 0.5f);
	EXPECT_FLOAT_EQ(combinedSphere1.position.y, 0.0f);
	EXPECT_FLOAT_EQ(combinedSphere1.position.z, 0.0f);
	EXPECT_FLOAT_EQ(combinedSphere1.radius, 1.5f);

	EXPECT_FLOAT_EQ(combinedSphere2.position.x, 1.0f);
	EXPECT_FLOAT_EQ(combinedSphere2.position.y, 0.0f);
	EXPECT_FLOAT_EQ(combinedSphere2.position.z, 0.0f);
	EXPECT_FLOAT_EQ(combinedSphere2.radius, 2.0f);
}
