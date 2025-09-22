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

#include <cmath>

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
 * \param theta The latitude angle in radians
 * \param phi The longitude/azimutal angle in radians
 * \return The basis function value
 */
std::complex<double> shBasisFunc(int l, int m, double theta, double phi) {
	const double normalization = std::sqrt(
			((2.0 * l + 1) / (4 * std::numbers::pi)) * boost::math::factorial<double>(l - std::abs(m)) /
			boost::math::factorial<double>(l + m));

	// TODO Replace this with std::assoc_legendre once it is supported in Clang and Apple Clang
	double p = boost::math::legendre_p(l, m, std::cos(theta));

	if (m < 0)
		p *= std::pow(-1, m);

	const auto exponential = std::polar(1.0, m * phi);

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
	const auto theta = std::acos(dir.y / r);
	const auto phi = std::atan2(dir.z, dir.x);

	return shBasisFunc(l, m, theta, phi);
}

/*!
 * Calculate the real basis value for spherical harmonics using a certain degree l an order m, a longitude/azimutal
 * angle theta and the latitude angle phi. The whole equation is described below:
 * \f[
 * Y_l^m=\begin{cases}
 * \sqrt{2} K_l^m cos(m\phi)P_l^{m}(cos(\theta)) & m>0\\
 * \sqrt{2} K_l^m sin(\vert m\vert\phi)P_l^{\vert m\vert}(cos(\theta)) & m<0\\
 * K_l^0P_l^0(cos(\theta)) & m=0
 * \end{cases}
 * \f]
 * It is based on the description of \url https://www.cse.chalmers.se/~uffe/xjobb/Readings/GlobalIllumination/Spherical%20Harmonic%20Lighting%20-%20the%20gritty%20details.pdf
 * and the modifications made there.
 *
 * \param l The degree of the function, must be \f$l \geq 0\f$
 * \param m The order which has to be \f$|m|\leq l\f$
 * \param theta The latitude angle in radians
 * \param phi The longitude/azimutal angle in radians
 * \return The basis function value
 */
double shRealBasisFunc(int l, int m, double theta, double phi) {
	double p = 0.0;

	if (m > 0)
		p = std::numbers::sqrt2 * std::pow(-1, m) * shBasisFunc(l, std::abs(m), theta, phi).real();
	else if (m < 0)
		p = std::numbers::sqrt2 * std::pow(-1, m) * shBasisFunc(l, std::abs(m), theta, phi).imag();
	else
		p = shBasisFunc(l, 0, theta, phi).real();

	return p;
}

/*!
* Calculate the real basis value for spherical harmonics using a certain degree l an order m, a longitude/azimutal
 * angle theta and the latitude angle phi. The whole equation is described below:
 * \f[
 * Y_l^m=\begin{cases}
 * \sqrt{2} K_l^m cos(m\phi)P_l^{m}(cos(\theta)) & m>0\\
 * \sqrt{2} K_l^m sin(\vert m\vert\phi)P_l^{\vert m\vert}(cos(\theta)) & m<0\\
 * K_l^0P_l^0(cos(\theta)) & m=0
 * \end{cases}
 * \f]
 * It is based on the description of \url https://www.cse.chalmers.se/~uffe/xjobb/Readings/GlobalIllumination/Spherical%20Harmonic%20Lighting%20-%20the%20gritty%20details.pdf
 * and the modifications made there.
 *
 * \param l The degree of the function, must be \f$ l \geq 0\f$
 * \param m The order which has to be \f$|m|\leq l\f$
 * \param dir The direction of the basis function to generate
 * \return The basis function value
 */
double shRealBasisFunc(int l, int m, glm::vec3 dir) {
	const auto r = glm::length(dir);
	const auto theta = std::acos(dir.y / r);
	const auto phi = std::atan2(dir.z, dir.x);

	return shRealBasisFunc(l, m, theta, phi);
}

} // End of namespace Common

#endif //OPENAWE_SH_H
