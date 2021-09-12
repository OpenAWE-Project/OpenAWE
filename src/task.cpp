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

#include "task.h"

#include <utility>

Task::Task() : _activateOnStartup(false), _active(false) {

}

Task::Task(const std::string &name, const std::vector<GID> &playerCharacter, bool activateOnStartup, std::vector<bool> activateOnStartupRound) :
	_name(name),
	_activateOnStartup(activateOnStartup),
	_activateOnStartupRound(std::move(activateOnStartupRound)),
	_playerCharacter(playerCharacter),
	_active(false) {
}

const std::string &Task::getName() const {
	return _name;
}

bool Task::isActiveOnStartup() const {
	return _activateOnStartup;
}

bool Task::isActiveOnStartupRound(unsigned int round) const {
	if (_activateOnStartupRound.size() <= round)
		return false;

	return _activateOnStartupRound[round];
}

GID Task::getPlayerCharacter(unsigned int round) const {
	if (_playerCharacter.size() > round)
		return GID{0, 0};

	return _playerCharacter[round];
}

void Task::activate() {
	_active = true;
}

void Task::complete() {
	_active = false;
}
