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

#include "timer.h"

Timer::Timer(float created, float duration) :
	_state(kNotStarted),
	_created(created),
	_startTime(0.0f),
	_duration(duration) {
}

Timer::Timer(float created, float startTime, float duration) :
	_state(kNotStarted),
	_created(created),
	_startTime(startTime),
	_duration(duration) {
}

void Timer::setStartBytecodeOffset(uint32_t offset) {
	_startBytecodeOffset = offset;
}

void Timer::setEndBytecodeOffset(uint32_t offset) {
	_endBytecodeOffset = offset;
}

float Timer::getCreated() const {
	return _created;
}

float Timer::getStartTime() const {
	return _startTime;
}

float Timer::getDuration() const {
	return _duration;
}

const std::optional<uint32_t> &Timer::getStartBytecodeOffset() const {
	return _startBytecodeOffset;
}

const std::optional<uint32_t> &Timer::getEndBytecodeOffset() const {
	return _endBytecodeOffset;
}

Timer::TimerState Timer::getState() const {
	return _state;
}

void Timer::setState(Timer::TimerState state) {
	_state = state;
}
