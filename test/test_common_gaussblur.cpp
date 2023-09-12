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

static const float kFullResultKernel05_3[] = {
		0.11170336,
		0.23647602,
		0.30364122,
		0.23647602,
		0.11170336
};
static const float kFullResultKernel10_5[] = {
		6.34846061e-08,
		6.96193241e-05,
		1.03324238e-02,
		2.07532280e-01,
		5.64131226e-01,
		2.07532280e-01,
		1.03324238e-02,
		6.96193241e-05,
		6.34846061e-08
};
static const float kFullResultKernel15_6[] = {
		3.07450366e-25,
		1.91530646e-16,
		1.32548972e-09,
		1.01903543e-04,
		8.70316139e-02,
		8.25732962e-01,
		8.70316139e-02,
		1.01903543e-04,
		1.32548972e-09,
		1.91530646e-16,
		3.07450366e-25
};

static const float kHalfResultKernel05_3[] = {
		0.46583557,
		0.3627931,
		0.17137133
};
static const float kHalfResultKernel10_5[] = {
		7.21334907e-01,
		2.65364282e-01,
		1.32117097e-02,
		8.90197995e-05,
		8.11755498e-08
};
static const float kHalfResultKernel15_6[] = {
		9.04549547e-01,
		9.53388209e-02,
		1.11630282e-04,
		1.45200831e-09,
		2.09812333e-16,
		3.36796643e-25
};

TEST(GaussBlur, generateFullGaussBlurWeights) {
	const auto kKernel05 = Common::generateFullGaussBlurWeights(3, 0.5);
	const auto kKernel10 = Common::generateFullGaussBlurWeights(5, 1.0);
	const auto kKernel15 = Common::generateFullGaussBlurWeights(6, 1.5);

	ASSERT_EQ(kKernel05.size(), 5);
	ASSERT_EQ(kKernel10.size(), 9);
	ASSERT_EQ(kKernel15.size(), 11);

	EXPECT_FLOAT_EQ(std::reduce(kKernel05.begin(), kKernel05.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel10.begin(), kKernel10.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel15.begin(), kKernel15.end()), 1.0);

	for (int i = 0; i < 5; ++i)
		EXPECT_FLOAT_EQ(kKernel05[i], kFullResultKernel05_3[i]);
	for (int i = 0; i < 9; ++i)
		EXPECT_FLOAT_EQ(kKernel10[i], kFullResultKernel10_5[i]);
	for (int i = 0; i < 11; ++i)
		EXPECT_FLOAT_EQ(kKernel15[i], kFullResultKernel15_6[i]);
}

TEST(GaussBlur, generateHalfGaussBlurWeights) {
	const auto kKernel05 = Common::generateHalfGaussBlurWeights(3, 0.5);
	const auto kKernel10 = Common::generateHalfGaussBlurWeights(5, 1.0);
	const auto kKernel15 = Common::generateHalfGaussBlurWeights(6, 1.5);

	ASSERT_EQ(kKernel05.size(), 3);
	ASSERT_EQ(kKernel10.size(), 5);
	ASSERT_EQ(kKernel15.size(), 6);

	EXPECT_FLOAT_EQ(std::reduce(kKernel05.begin(), kKernel05.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel10.begin(), kKernel10.end()), 1.0);
	EXPECT_FLOAT_EQ(std::reduce(kKernel15.begin(), kKernel15.end()), 1.0);

	for (int i = 0; i < 3; ++i)
		EXPECT_FLOAT_EQ(kKernel05[i], kHalfResultKernel05_3[i]);
	for (int i = 0; i < 5; ++i)
		EXPECT_FLOAT_EQ(kKernel10[i], kHalfResultKernel10_5[i]);
	for (int i = 0; i < 6; ++i)
		EXPECT_FLOAT_EQ(kKernel15[i], kHalfResultKernel15_6[i]);
}
