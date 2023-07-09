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

#include <glm/glm.hpp>
#include <fmt/format.h>

#define OS_LINUX linux || __linux
#define OS_ANDROID __ANDROID__
#define OS_MACOS macintosh || Macintosh || __APPLE__ || __MACH__
#define OS_IOS (__APPLE__ || __MACH__) && __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
#define OS_WINDOWS _WIN32 || _WIN64 || __WIN32__ || __TOS_WIN__ || __WINDOWS__

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

struct BoundBox {
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;
};

struct BoundSphere {
	glm::vec3 position;
	float radius;

	inline bool intersect(const BoundSphere &boundSphere) const {
		return glm::distance(position, boundSphere.position) < radius + boundSphere.radius;
	}
};

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
