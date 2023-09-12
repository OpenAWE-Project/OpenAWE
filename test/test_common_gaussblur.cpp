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
#include "src/common/gaussblur.h"

#include <gtest/gtest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const float kResultKernel05_3[] = {
		0.46583557,
		0.3627931,
		0.17137133
};
static const float kResultKernel10_5[] = {
		7.21334907e-01,
		2.65364282e-01,
		1.32117097e-02,
		8.90197995e-05,
		8.11755498e-08
};
static const float kResultKernel15_6[] = {
		9.04549547e-01,
		9.53388209e-02,
		1.11630282e-04,
		1.45200831e-09,
		2.09812333e-16,
		3.36796643e-25
};

TEST(GaussBlur, generateGaussBlurWeights) {
	const auto kKernel05 = Common::generateGaussBlurWeights(3, 0.5);
	const auto kKernel10 = Common::generateGaussBlurWeights(5, 1.0);
	const auto kKernel15 = Common::generateGaussBlurWeights(6, 1.5);

	ASSERT_EQ(kKernel05.size(), 3);
	ASSERT_EQ(kKernel10.size(), 5);
	ASSERT_EQ(kKernel15.size(), 6);

	EXPECT_FLOAT_EQ(std::reduce(kKernel05.begin(), kKernel05.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel10.begin(), kKernel10.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel15.begin(), kKernel15.end()), 1.0);

	for (int i = 0; i < 3; ++i)
		EXPECT_FLOAT_EQ(kKernel05[i], kResultKernel05_3[i]);
	for (int i = 0; i < 5; ++i)
		EXPECT_FLOAT_EQ(kKernel10[i], kResultKernel10_5[i]);
	for (int i = 0; i < 6; ++i)
		EXPECT_FLOAT_EQ(kKernel15[i], kResultKernel15_6[i]);
}
