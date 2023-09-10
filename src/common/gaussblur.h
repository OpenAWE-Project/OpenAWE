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

#ifndef OPENAWE_BLOOM_H
#define OPENAWE_BLOOM_H

#include <cmath>

#include <vector>
#include <numeric>

namespace Common {

/*!
 * Generate the weights for a gauss blur filter using the standard formula: \f$G(x)=e^{-x^2\sigma^2}\f$ The resulting
 * values are normalized using the sum of all weights using the formula: \f$G'(x)=\frac{G(x)}{\sum_{x'} G(x')}\f$
 * \tparam T The type of values to generate the weights for
 * \param size The size of the kernel
 * \param sigma The sigma value
 * \return A vector containing the gauss weights
 */
template<typename T = float> std::vector<T> generateGaussBlurWeights(unsigned int size, T sigma) {
	assert(size > 0);

	std::vector<T> weights(size);

	for (unsigned int i = 0; i < size; ++i)
		weights[i] = std::exp(-(std::pow(i, 2) * std::pow(sigma, 2)));

	float sum = std::reduce(weights.begin(), weights.end());

	for (auto &weight: weights)
		weight /= sum;

	return weights;
}

} // End of namespace Common

#endif // OPENAWE_BLOOM_H
