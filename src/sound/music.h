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

#ifndef OPENAWE_MUSIC_H
#define OPENAWE_MUSIC_H

#include <memory>


#include "src/sound/source.h"

namespace Sound {

class Music;

typedef std::shared_ptr<Music> MusicPtr;

/*!
 * @brief Source for playing a globally hearable music in the scene
 *
 * This class is responsible for handling the music currently to be played in the scenery and hearable everywhere
 */
class Music {
public:
	Music();

	/*!
	 * Directly stops the current music and starts the new one
	 *
	 * @param stream The new music to start
	 */
	void setMusic(Sound::Source *stream);

private:
	std::unique_ptr<Source> _music;
};

} // Sound

#endif //OPENAWE_MUSIC_H
