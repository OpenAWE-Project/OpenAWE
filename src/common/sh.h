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

#ifndef OPENAWE_SH_H
#define OPENAWE_SH_H

#include <complex>

#include <boost/math/special_functions.hpp>

namespace Common {

/*!
 * Calculate the basis value for spherical harmonics using a certain degree l an order m, a longitude/azimutal angle
 * theta and the latitude angle phi. The whole equation is described below:
 * \f[
 * Y_l^m(\theta,\phi)=\sqrt{\frac{(2l+1)}{4\pi}\cdot\frac{(l-m)!}{(l+m)!}}P_l^m(cos(\theta))\cdot e^{im\phi}
 * \f]
 *
 * \param l The degree of the function, must be \f$l \geq 0\f$
 * \param m The order which has to be \f$|m|\leq l\f$
 * \param theta The longitude/azimutal angle in radians
 * \param phi The latitude angle in radians
 * \return The basis function value
 */
std::complex<double> shBasisFunc(int l, int m, double theta, double phi) {
	const double normalization = std::sqrt(
			((2.0 * l + 1) / (4 * std::numbers::pi)) * boost::math::factorial<double>(l - std::abs(m)) /
			boost::math::factorial<double>(l + std::abs(m)));
	double p = std::assoc_legendre(l, std::abs(m), std::cos(theta));

	if (m < 0)
		p *= std::pow(-1, m);

	const auto exponential = std::complex<double>(std::cos(m * phi), std::sin(m * phi));

	return normalization * p * exponential;
}

/*!
 * Calculate the basis value for spherical harmonics using a certain degree l an order m and a cartesian direction. The
 * whole equation is described below:
 * \f[
 * Y_l^m(\theta,\phi)=\sqrt{\frac{(2l+1)}{4\pi}\cdot\frac{(l-m)!}{(l+m)!}}P_l^m(cos(\theta))\cdot e^{im\phi}
 * \f]
 *
 * \param l The degree of the function, must be \f$l \geq 0\f$
 * \param m The order which has to be \f$|m|\leq l\f$
 * \param dir The direction of the basis function to generate
 * \return The basis function value
 */
std::complex<double> shBasisFunc(int l, int m, glm::vec3 dir) {
	const auto r = glm::length(dir);
	const auto theta = std::atan2(dir.z, dir.x);
	const auto phi = std::acos(dir.y / r);

	return shBasisFunc(l, m, theta, phi);
}

} // End of namespace Common

#endif //OPENAWE_SH_H
