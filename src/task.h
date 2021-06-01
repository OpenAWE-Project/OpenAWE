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

#ifndef OPENAWE_TASK_H
#define OPENAWE_TASK_H

#include <vector>
#include <string>

#include "src/awe/types.h"

/*!
 * \brief Class representing a task
 *
 * This class represents a task in the game which can be activated and holds several information, like if it is a
 * startup, including the specific round for that or the player character to be assigned. For american nightmare, this
 * class has support for accessing the calues by rounds.
 */
class Task {
public:
	Task();

	/*!
	 * Create a new task
	 * \param name The name of the task
	 * \param playerCharacter A vector of gids, which character is the player controlled one
	 * \param activateOnStartup Activate the task on startup
	 * \param activateOnStartupRound Activate the task on startup of a specific round
	 */
	Task(
		const std::string &name,
		const std::vector<GID> &playerCharacter,
		bool activateOnStartup,
		std::vector<bool> activateOnStartupRound
	);
	Task(const Task &) = default;

	/*!
	 * Get the name of the task
	 * \return The name of the task
	 */
	const std::string &getName() const;

	/*!
	 * If this task should be activated on startup
	 * \return If the task is activated on startup
	 */
	bool isActiveOnStartup() const;

	/*!
	 * If this task should be activated on startup of a specific round
	 * \param round The round on which to startup
	 * \return If this task should be activated on a specific round
	 */
	bool isActiveOnStartupRound(unsigned int round) const;

	/*!
	 * Get which character will be marked as the player controlled character, in the case of American Nightmare
	 * depending on the current round
	 * \param round The round for which to get the character
	 * \return The GId of the character that should be marked as player character
	 */
	GID getPlayerCharacter(unsigned int round = 0) const;

	void activate();
	void complete();

private:
	std::string _name;
	bool _activateOnStartup;
	std::vector<bool> _activateOnStartupRound;
	std::vector<GID> _playerCharacter;
	bool _active;
};


#endif //OPENAWE_TASK_H
