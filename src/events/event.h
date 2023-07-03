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

#ifndef OPENAWE_EVENT_H
#define OPENAWE_EVENT_H

#include <variant>

#include <glm/glm.hpp>

namespace Events {

/*!
 * Enum representing if a key is pressed or released. This is for keys and mouse and controller buttons
 */
enum KeyState {
	kPress,
	kRelease
};

/*!
 * An event on a specific value
 * @tparam T
 */
template<typename T>
struct AxisEvent {
	T absolute, delta;
};

struct KeyEvent {
	KeyState state;
};

typedef std::variant<
		AxisEvent<float>,
		AxisEvent<double>,
		AxisEvent<glm::vec2>,
		KeyEvent
> EventData;

struct Event {
	uint32_t action;
	EventData data;
};

}

#endif //OPENAWE_EVENT_H
