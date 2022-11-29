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

#ifndef OPENAWE_NURBS_H
#define OPENAWE_NURBS_H

#include <cstring>

namespace Common {

/*!
 * \brief Class for handling NURBS Curves
 *
 * This class represents a Non-Uniform Rational B-Spline, also known as NURBS. It is used for interpolating values over
 * a range of control points and knots.
 */
template<typename T>
class NURBS {
public:
	NURBS(std::vector<T> controlPoints, std::vector<uint8_t> knots, unsigned int degree) :
		_degree(degree),
		_controlPoints(controlPoints),
		_knots(knots) {
	}

	T interpolate(unsigned int t) const {
		const size_t span = findSpan(t);
		const auto basis = bsplineBasis(span, t);

		T value;
		std::memset(&value, 0, sizeof(T));
		for (unsigned int i = 0; i <= _degree; ++i) {
			value += _controlPoints[span - i] * basis[i];
		}

		return value;
	}

private:
	/*
	 * bsplineBasis and findSpan are based on the implementations of
	 * https://github.com/PredatorCZ/HavokLib
	 */

	std::vector<float> bsplineBasis(size_t span, float t) const {
		std::vector<float> N(_degree + 1);
		std::fill(N.begin(), N.end(), 0.0f);

		N[0] = 1.0f;

		for (unsigned int i = 0; i < _degree; ++i) {
			for (int j = i; j >= 0; --j) {
				float a = (t - _knots[span - j]) / (_knots[span + i + 1 - j] - _knots[span - j]);
				float tmp = N[j] * a;
				N[j + 1] += N[j] - tmp;
				N[j] = tmp;
			}
		}

		return N;
	}

	size_t findSpan(unsigned int t) const {
		if (t >= _knots[_controlPoints.size()])
			return _controlPoints.size() - 1;

		size_t low = _degree;
		size_t high = _controlPoints.size();
		size_t mid = (low + high) / 2;

		while(t < _knots[mid] || t >= _knots[mid + 1]) {
			if (t < _knots[mid]) {
				high = mid;
			} else {
				low = mid;
			}

			mid = (low + high) / 2;
		}

		return mid;
	}

	unsigned int _degree;
	std::vector<T> _controlPoints;
	std::vector<uint8_t> _knots;
};

} // End of namespace Common

#endif //OPENAWE_NURBS_H
