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

#ifndef OPENAWE_STD140BUFFER_H
#define OPENAWE_STD140BUFFER_H

#include <cstring>

#include <boost/pfr.hpp>

#include "src/common/memwritestream.h"
#include "src/common/types.h"
#include "src/common/glmutil.h"

namespace Graphics {

/**
 * Pack a simple arithmetic value into a std140 layout
 *
 * \tparam T The type of the value to pack
 * \param val The value to pack
 * \param stream The stream in which to pack the value
 */
template<typename T>
	requires std::is_arithmetic_v<T>
void packValue(const T &val, Common::DynamicMemoryWriteStream &stream) {
	stream.align(sizeof(T));
	Common::write(stream, val);
}

/**
 * Pack a glm based vector to the given stream  using the std140 layout
 *
 * \tparam L The length of the vector to pack
 * \tparam T The type of the vectors elements to pack
 * \tparam Q The qualifier of the vectors elements
 * \param val The vector to pack
 * \param stream The stream in which to pack the value to an std140 layout
 */
template<glm::length_t L, typename T, glm::qualifier Q>
	requires std::is_arithmetic_v<T>
void packVec(const glm::vec<L, T, Q> &val, Common::DynamicMemoryWriteStream &stream) {
	static_assert(L > 0 && L <= 4);
	switch (L) {
		case 1:
			stream.align(sizeof(T));
			Common::write(stream, val);
			break;
		case 2:
			stream.align(2 * sizeof(T));
			Common::write(stream, val);
			break;
		case 3: [[fallthrough]];
		case 4:
			stream.align(4 * sizeof(T));
			Common::write(stream, val);
			break;
		default:
			break;
	}
}

/**
 * Pack an array of glm based vectors to the given stream using the std140 layout
 *
 * \tparam L The length of the vector to pack. Only values in [1, 4] are allowed
 * \tparam T The type of the value inside the vector
 * \tparam Q The qualifier of the vectors elements
 * \param val The vector to pack
 * \param stream The stream in which to pack the array to an std140 layout
 */
template<glm::length_t L, typename T, glm::qualifier Q>
	requires std::is_arithmetic_v<T>
void packVecArray(const std::vector<glm::vec<L, T, Q> > &val, Common::DynamicMemoryWriteStream &stream) {
	static_assert(L > 0 && L <= 4);
	for (const auto &v : val) {
		packVec(v, stream);
	}
	stream.align(sizeof(glm::vec4));
}

/**
 * Pack a matrix to the given stream using the std140 layout
 *
 * \tparam W The number of columns in the matrix
 * \tparam H The number of rows in the matrix
 * \tparam T The type of the matrix elements
 * \tparam Q The qualifiers of the matrix
 * \param val The matrix to pack
 * \param stream The stream in which to pack the matrix
 */
template<glm::length_t W, glm::length_t H, typename T, glm::qualifier Q>
	requires std::is_arithmetic_v<T>
void packMat(const glm::mat<W, H, T, Q> &val, Common::DynamicMemoryWriteStream &stream) {
	static_assert(W > 0 && W <= 4 && H > 0 && H <= 4);
	for (glm::length_t i = 0; i < W; ++i) {
		const auto &col = val[i];
		packVec(col, stream);
	}
}

/**
 * Pack an array of matrices to the given stream using the std140 layout
 *
 * \tparam W The number of columns in the matrix
 * \tparam H The number of rows in the matrix
 * \tparam T The type of the matrix elements
 * \tparam Q The Qualifiers of the matrix
 * \param val The matrices to pack
 * \param stream The stream in which to pack the matrices
 */
template<glm::length_t W, glm::length_t H, typename T, glm::qualifier Q>
	requires std::is_arithmetic_v<T>
void packMatArray(const std::vector<glm::mat<W, H, T, Q> > &val, Common::DynamicMemoryWriteStream &stream) {
	static_assert(W > 0 && W <= 4 && H > 0 && H <= 4);
	for (const auto &column : val) {
		packMat(column, stream);
	}
	stream.align(sizeof(glm::vec4));
}


/**
 * Pack a struct to the given stream using the std140 layout
 *
 * \tparam T The type of the struct to pack
 * \param s The struct to pack
 * \param stream The stream in which to pack the struct
 */
template<typename T>
	requires std::is_aggregate_v<T>
void packStruct(const T &s, Common::DynamicMemoryWriteStream &stream) {
	boost::pfr::for_each_field(s, [&]<typename T2>(const T2 &val) {
		if constexpr (std::is_arithmetic_v<T2>) {
			packValue(val, stream);
		} else if constexpr (Common::is_glm_vec_v<T2>) {
			packVec(val, stream);
		} else if constexpr (Common::is_glm_mat_v<T2>) {
			packMat(val, stream);
		} else if constexpr (std::is_aggregate_v<T2>) {
			packStruct(val, stream);
		}

		std::unreachable_sentinel_t{};
	});

	stream.align(sizeof(glm::vec4));
}

/**
 * Pack the given value to a byte buffer and return it in std140 layout
 *
 * \tparam T The type of the struct to pack
 * \param v The value to pack
 * \return A byte buffer containing the packed data
 */
template<typename T>
	requires std::is_aggregate_v<T>
Common::ByteBuffer pack(const T &v) {
	Common::DynamicMemoryWriteStream stream(true);
	Common::ByteBuffer buffer;

	packStruct(v, stream);

	buffer.resize(stream.getLength());
	std::memcpy(buffer.data(), stream.getData(), buffer.size());
	return std::move(buffer);
}

} // End of namespace Graphics

#endif //OPENAWE_STD140BUFFER_H
