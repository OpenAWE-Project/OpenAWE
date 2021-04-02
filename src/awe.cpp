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

#include <iostream>
#include <spdlog/spdlog.h>

#include "src/common/readfile.h"

#include "src/sound/soundman.h"

#include "src/awe/resman.h"
#include "src/awe/havokfile.h"
#include "src/awe/worldfile.h"
#include "src/awe/dpfile.h"
#include "src/world.h"
#include "src/game.h"

int main(int argc, char** argv) {
	Game game;

	if (!game.parseArguments(argc, argv))
		return EXIT_SUCCESS;

	try {
		game.init();
		game.start();
	} catch (const std::exception &e) {
		spdlog::critical(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
