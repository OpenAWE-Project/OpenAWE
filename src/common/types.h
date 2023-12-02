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

#include <glm/glm.hpp>
#include <fmt/format.h>

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

#define MKTAG(a0,a1,a2,a3) ((uint32_t)((a3) | ((a2) << 8) | ((a1) << 16) | ((a0) << 24)))

typedef unsigned char byte;

namespace Common {

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
};

/*!
 * Combine two bounding spheres into one, encapsulating bothe
 * \param sphere1 The first sphere
 * \param sphere2 The second sphere
 * \return The newly created combined sphere
 */
inline BoundSphere combine(Common::BoundSphere sphere1, Common::BoundSphere sphere2) {
	const float distance = glm::distance(sphere1.position, sphere2.position);
	const float newRadius = (sphere1.radius + sphere2.radius + distance) / 2.0f;
	const auto newCenter = glm::mix(sphere1.position, sphere2.position, sphere1.radius / (sphere1.radius + sphere2.radius));

	return {newCenter, newRadius};
}

}

// Polyfill function for fmt::underlying for fmt version < 9.0.0
#if FMT_VERSION < 90000
namespace fmt {

template <typename Enum>
constexpr auto underlying(Enum e) noexcept -> std::underlying_type_t<Enum> {
	return static_cast<std::underlying_type_t<Enum>>(e);
}

}
#endif

#endif //SRC_COMMON_TYPES_H
