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

#ifndef AWE_SOURCE_H
#define AWE_SOURCE_H

#include "src/common/types.h"

#include "src/sound/openal.h"

namespace Sound {

class Source : Common::Noncopyable {
public:
	Source();
	~Source();

	virtual void play();
	void pause();
	virtual void stop();

	bool isPlaying();

protected:
	void setRelative(bool relative);

	unsigned int getBuffersQueued();
	unsigned int getBuffersProcessed();

	void queueBuffer(ALuint buffer);
	void unqueueBuffer(ALuint buffer);

private:
	ALuint _id;
};

}

#endif //AWE_SOURCE_H
