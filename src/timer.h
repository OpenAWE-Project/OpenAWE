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

#ifndef OPENAWE_TIMER_H
#define OPENAWE_TIMER_H

#include <cstdint>
#include <optional>
#include <memory>

class Timer;

typedef std::shared_ptr<Timer> TimerPtr;

class Timer {
public:
	enum TimerState {
		kNotStarted,
		kStarted,
		kStopped
	};

	Timer() {};
	Timer(float created, float duration);
	Timer(float created, float startTime, float duration);

	TimerState getState() const;
	void setState(TimerState state);

	void setStartBytecodeOffset(uint32_t offset);
	void setEndBytecodeOffset(uint32_t offset);

	float getCreated() const;

	float getStartTime() const;

	float getDuration() const;
	const std::optional<uint32_t> &getStartBytecodeOffset() const;
	const std::optional<uint32_t> &getEndBytecodeOffset() const;

private:
	TimerState _state;
	float _created, _startTime, _duration;
	std::optional<uint32_t> _startBytecodeOffset, _endBytecodeOffset;
};


#endif //OPENAWE_TIMER_H
