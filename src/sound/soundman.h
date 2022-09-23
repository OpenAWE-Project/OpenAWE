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

#ifndef AWE_SOUNDMANAGER_H
#define AWE_SOUNDMANAGER_H

#include <vector>

#include "src/common/singleton.h"

#include "src/codecs/audiostream.h"

#include "src/sound/openal.h"
#include "src/sound/stream.h"

namespace Sound {

class SoundManager : public Common::Singleton<SoundManager> {
public:
	SoundManager();
	~SoundManager();

	void setMusic(Sound::Source *stream);
	Source & getMusic();

	void init();

	void update();

	ALuint registerSource();
	void deregisterSource(ALuint id);

	ALuint registerBuffer();
	void deregisterBuffer(ALuint id);

private:
	std::unique_ptr<Source> _music;

	std::vector<ALuint> _sourcePool;
	std::vector<ALuint> _bufferPool;

	ALCdevice *_device;
	ALCcontext *_context;
};

} // End of namespace Sound

#define SoundMan Sound::SoundManager::instance()

#endif //AWE_SOUNDMANAGER_H
