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

#ifndef SRC_COMMON_TYPES_H
#define SRC_COMMON_TYPES_H

#include <algorithm>
#include <vector>
#include <span>

#include <glm/glm.hpp>

/*
 * Operating System Macros
 */
#define OS_LINUX linux || __linux
#define OS_ANDROID __ANDROID__
#define OS_MACOS macintosh || Macintosh || __APPLE__ || __MACH__
#define OS_IOS (__APPLE__ || __MACH__) && __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
#define OS_WINDOWS _WIN32 || _WIN64 || __WIN32__ || __TOS_WIN__ || __WINDOWS__

/*
 * Architecture Macros
 */
#define ARCH_ARM __arm__ || __arm64 || __thumb__ || __TARGET_ARCH_ARM || __TARGET_ARCH_THUMB || _M_ARM
#define ARCH_X86_32 i386 || __i386__ || __i486__ || __i586__ || __i686__ || __i386 || _M_IX86 || _X86_ || \
	__THW_INTEL__ || __I86__  || __INTEL__
#define ARCH_X86_64 __x86_64 || __x86_64__ || __amd64 || __amd64__ || _M_X64
#define ARCH_X86 ARCH_X86_32 || ARCH_X86_64

/*
 * Standard library macros
 */
#define LIB_STDCXX __GLIBCXX__ || __GLIBCPP__
#define LIB_CPP _LIBCPP_VERSION

#define MKTAG(a0,a1,a2,a3) ((uint32_t)((a3) | ((a2) << 8) | ((a1) << 16) | ((a0) << 24)))

typedef unsigned char byte;

namespace Common {

typedef std::vector<std::byte> ByteBuffer;

/*!
 * Reinterpret a byte buffer to a span of arbitrary type
 *
 * \tparam T The type to which to reinterpret the byte buffer as span
 * \param bb The byte buffer to be reinterpreted as span with arbitrary type
 * \return The newly created span
 */
template<typename T>
std::span<T> toSpan(ByteBuffer& bb) {
	assert(bb.size() % sizeof(T) == 0);
	return {reinterpret_cast<T*>(bb.data()), bb.size()};
}

/*!
 * Reinterpret a constant byte buffer to a span of arbitrary constant type
 *
 * \tparam T The type to which to reinterpret the byte buffer as span
 * \param bb The byte buffer to be reinterpreted as span with arbitrary type
 * \return The newly created span
 */
template<typename T>
std::span<const T> toSpan(const ByteBuffer& bb) {
	assert(bb.size() % sizeof(T) != 0);
	return std::span<const T>(reinterpret_cast<const T*>(bb.data()), bb.size());
}

/*!
 * Reinterpret an array to a span of arbitrary type
 *
 * \tparam T The type to which to reinterpret the array as span
 * \param a The array to be reinterpreted as span with arbitrary type
 * \return The newly created span
 */
template<typename T, typename OT, size_t N>
requires (sizeof(OT) * N % sizeof(T) == 0)
std::span<T> toSpan(std::array<OT, N>& a) {
	return {reinterpret_cast<T*>(a.data()), a.size() * N / sizeof(T)};
}

/*!
 * Reinterpret a constant array to a span of arbitrary type
 *
 * \tparam T The type to which to reinterpret the array as span
 * \param a The array to be reinterpreted as span with arbitrary type
 * \return The newly created span
 */
template<typename T, typename OT, size_t N>
requires (sizeof(OT) * N % sizeof(T) == 0)
std::span<const T> toSpan(const std::array<OT, N>& a) {
	return {reinterpret_cast<const T*>(a.data()), a.size() * N / sizeof(T)};
}

/*!
 * \brief Util class for making non copyable
 */
class Noncopyable {
public:
	Noncopyable() = default;
	~Noncopyable() = default;

private:
	Noncopyable(const Noncopyable &) = delete;
	Noncopyable &operator=(const Noncopyable &) = delete;
};

/*!
 * Helper type for std::visit. It enables to assign multiple type dependent lambda functions
 * to std::visit
 *
 * @tparam Ts The types of the overload
 */
template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

/*!
 * \brief Util class for representing a axis aligned bounding box
 *
 * This class represents an axis aligned bounding box and has helper functions to test if
 * a point is contained in the box
 */
struct BoundBox {
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;

	BoundBox() {}

	BoundBox(const glm::vec3 &v1, const glm::vec3 &v2) {
		xmin = std::min(v1.x, v2.x);
		ymin = std::min(v1.y, v2.y);
		zmin = std::min(v1.z, v2.z);
		xmax = std::max(v1.x, v2.x);
		ymax = std::max(v1.y, v2.y);
		zmax = std::max(v1.z, v2.z);
	}

	inline bool isInside(const glm::vec3 &p, float margin = 0.00001) {
		return
			xmin - margin <= p.x && p.x <= xmax + margin &&
			ymin - margin <= p.y && p.y <= ymax + margin &&
			zmin - margin <= p.z && p.z <= zmax + margin;
	}
};

struct BoundSphere {
	glm::vec3 position;
	float radius;

	inline bool intersect(const BoundSphere &boundSphere) const {
		return glm::distance(position, boundSphere.position) < radius + boundSphere.radius;
	}

	inline bool contains(const glm::vec3 &v) const {
		return glm::distance(position, v) <= radius;
	}

	inline bool contains(const BoundSphere &s) const {
		return glm::distance(position, s.position) + s.radius <= radius;
	}
};

/*!
 * Combine two bounding spheres into one, encapsulating both
 *
 * \param sphere1 The first sphere
 * \param sphere2 The second sphere
 * \return The newly created combined sphere
 *
 * \note Algorithm taken from
 * https://github.com/sharpdx/SharpDX/blob/master/Source/SharpDX.Mathematics/BoundingSphere.cs
 */
inline BoundSphere combine(Common::BoundSphere sphere1, Common::BoundSphere sphere2) {
	if (sphere1.contains(sphere2))
		return sphere1;
	if (sphere2.contains(sphere1))
		return sphere2;

	const auto diff = sphere2.position - sphere1.position;
	const auto length = glm::length(diff);
	const auto radius1 = sphere1.radius;
	const auto radius2 = sphere2.radius;

	const auto dir = glm::normalize(diff);
	const auto min = std::min(-radius1, length - radius2);
	const auto max = (std::max(radius1, length + radius2) - min) * 0.5f;

	return {sphere1.position + dir * (max + min), max};
}

/*!
 * Helper function to get the underlying value of a scoped enum
 *
 * @tparam E The type of the enum
 * @param e The enum value
 * @return The enum value as its underlying type
 */
template<typename E>
requires std::is_enum_v<E> // TODO Replace this through std::is_scoped_enum with c++23
inline std::underlying_type_t<E> toUnderlying(E e) {
	return static_cast<std::underlying_type_t<E>>(e);
}

}

#endif //SRC_COMMON_TYPES_H
