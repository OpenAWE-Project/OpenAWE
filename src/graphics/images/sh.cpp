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

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "src/common/sh.h"

#include "src/graphics/images/sh.h"

namespace Graphics {

SurfacePtr generateSHMap(unsigned int order, size_t imageSize) {
	const auto iotaSum = [](unsigned int v) {
		unsigned int sum = 0;
		for (unsigned int i = 1; i <= v; ++i) {
			sum += i;
		}

		return sum;
	};

	const auto directionVector = [](unsigned int d, float u, float v) -> glm::vec3 {
		switch (d) {
			case 0: return {1, -u, -v};
			case 1: return {-1, -u, v};
			case 2: return {v, 1, u};
			case 3: return {v, -1,  -u};
			case 4: return {v, -u, 1};
			case 5: return {-v, -u, -1};
			default:
				return glm::zero<glm::vec3>();
		}
	};

	const auto degreeCount = 1 + order + iotaSum(order) * 2;
	const auto stride = degreeCount * imageSize;
	SurfacePtr surface = std::make_shared<Surface>(stride, 6 * imageSize, kR32F);

	std::vector<std::tuple<int, int> > degrees(degreeCount);
	int lc = 0, mc = 0;

	for (auto &[l, m]: degrees) {
		l = lc;
		m = mc;

		if (l == m) {
			lc++;
			mc = -lc;
		} else {
			mc++;
		}
	}

	auto date = surface->getData<float>();
	const auto quarterSize = std::pow(imageSize / 2, 2);
	for (int p = 0; p < 6; ++p) {
		for (size_t i = 0; i < degrees.size(); ++i) {
			const auto xsubimage = i;
			const auto ysubimage = p;
			const auto &[l, m] = degrees[i];

			for (size_t y = 0; y < imageSize; ++y) {
				for (size_t x = 0; x < imageSize; ++x) {
					const auto xoffset = y + imageSize * xsubimage;
					const auto yoffset = x + imageSize * ysubimage;
					const auto halfImageSize = static_cast<float>(imageSize) / 2.0f;

					const auto u = (static_cast<float>(x) - (halfImageSize - 0.5f)) / halfImageSize;
					const auto v = (static_cast<float>(y) - (halfImageSize - 0.5f)) / halfImageSize;

					auto targetPixelVector = directionVector(p, u, v);
					const auto weight = std::pow(glm::length(targetPixelVector), std::abs(l)) /
										(std::pow(glm::length(targetPixelVector), 3) * quarterSize * std::numbers::pi);

					date[yoffset * stride + xoffset] = weight * Common::shRealBasisFunc(l, m, targetPixelVector);
				}
			}
		}
	}

	return surface;
}

} // End of namespace Graphics