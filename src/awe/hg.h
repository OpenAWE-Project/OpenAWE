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

#ifndef OPENAWE_HG_H
#define OPENAWE_HG_H

#include <cmath>

#include <array>

namespace AWE {

/*!
 * Generate Henyey-Greenstein constant factors, which are the following:
 * - \$\frac{(1-g)^2}{4\pi}\$
 * - \$g^2+1\$
 * - \$-2g\$
 * These three parameters form the constant parts of the Henyey-Greenstein
 * Phase function. The original phase function is changed is changed in
 * the games from \$1-g^2\$ to \$(1-g)^2\$.
 *
 * @tparam T The type of the Henyey Greenstein parameters
 * @param g The g parameter which should be known in advance
 * @return An array of three alements
 */
template<typename T = float> std::array<T, 3> generateHenyeyGreensteinConstants(T g) {
	return {
		std::pow(1.0 - g, 2) / (4.0 * M_PI),
		std::pow(g, 2) + 1.0,
		-2.0 * g
	};
}

} // End of namespace AWE

#endif //OPENAWE_HG_H
