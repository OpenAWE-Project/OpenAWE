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

#ifndef OPENAWE_CONFIGURATION_H
#define OPENAWE_CONFIGURATION_H

struct ResolutionData {
	unsigned int width, height;
	bool fullscreen;
};

/*!
 * \brief Class representing the global configuration of the engine
 *
 * This class represents the global configuration of a specific game, not
 * the state of a certain save. It is used for storing for example the manuscript
 * pages in both Alan Wake games
 */
class Configuration {
public:
	virtual ~Configuration() = default;

	ResolutionData resolution;

	/*!
	 * Write the configuration
	 */
	virtual void write() = 0;

	/*!
	 * Read the configuration
	 */
	virtual void read() = 0;
};

#endif //OPENAWE_CONFIGURATION_H
