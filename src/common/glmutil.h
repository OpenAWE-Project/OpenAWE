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

#ifndef OPENAWE_GLMUTIL_H
#define OPENAWE_GLMUTIL_H

#include <type_traits>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <boost/container_hash/hash.hpp>

template<> struct boost::hash<glm::vec2> : std::hash<glm::vec2> {};
template<> struct boost::hash<glm::vec3> : std::hash<glm::vec3> {};
template<> struct boost::hash<glm::vec4> : std::hash<glm::vec4> {};

namespace Common {

template<typename T>
struct is_glm_vec : std::false_type {};

template<glm::length_t L, typename T, glm::qualifier Q>
struct is_glm_vec<glm::vec<L, T, Q>> : std::true_type {};

template<typename T>
struct is_glm_mat : std::false_type {};

template<glm::length_t W, glm::length_t H, typename T, glm::qualifier Q>
struct is_glm_mat<glm::mat<W, H, T, Q>> : std::true_type {};

template<typename T> inline constexpr bool is_glm_vec_v = is_glm_vec<T>::value;
template<typename T> inline constexpr bool is_glm_mat_v = is_glm_mat<T>::value;

}

#endif //OPENAWE_GLMUTIL_H
