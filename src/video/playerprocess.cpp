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

#include <spdlog/spdlog.h>
#include "playerprocess.h"

namespace Video {

PlayerProcess::PlayerProcess(Player &player) : _player(player) {

}

void PlayerProcess::update(double time, void *) {
	if (!_player.isPlaying())
		succeed();

	_player.update();
}

void PlayerProcess::succeeded() {
	spdlog::debug("Successfully finished video");
}

void PlayerProcess::aborted() {
	spdlog::debug("Video aborted");
	_player.stop();
}

} // Video