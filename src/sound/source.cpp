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

#include <stdexcept>
#include <assert.h>

#include "src/sound/soundman.h"
#include "source.h"

namespace Sound {

Source::Source() {
	_id = SoundMan.registerSource();
	assert(alIsSource(_id));

	alSourcef(_id, AL_PITCH, 1.0f);
	alSourcef(_id, AL_GAIN, 1.0f);
	alSource3f(_id, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(_id, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(_id, AL_LOOPING, AL_FALSE);
	alSourcei(_id, AL_BUFFER, 0);
	//alSourcei(_id, AL_DIRECT_CHANNELS_SOFT, 1);
}

Source::~Source() {
	SoundMan.deregisterSource(_id);
}

void Source::play() {
	alSourcePlay(_id);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
		throw std::runtime_error("OpenAL Error");
}

void Source::pause() {
	alSourcePause(_id);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
		throw std::runtime_error("OpenAL Error");
}

void Source::stop() {
	alSourceStop(_id);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
		throw std::runtime_error("OpenAL Error");
}

bool Source::isPlaying() {
	int state;
	alGetSourcei(_id, AL_SOURCE_STATE, &state);
	assert(alGetError() == AL_NO_ERROR);
	return state == AL_PLAYING;
}

void Source::setRelative(bool relative) {
	alSourcei(_id, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
	assert(alGetError() == AL_NO_ERROR);
}

unsigned int Source::getBuffersQueued() {
	ALint buffersQueued;
	alGetSourcei(_id, AL_BUFFERS_QUEUED, &buffersQueued);
	return buffersQueued;
}

unsigned int Source::getBuffersProcessed() {
	ALint buffersProcessed;
	alGetSourcei(_id, AL_BUFFERS_PROCESSED, &buffersProcessed);
	return buffersProcessed;
}

void Source::queueBuffer(ALuint buffer) {
	alSourceQueueBuffers(_id, 1, &buffer);
}

void Source::unqueueBuffer(ALuint buffer) {
	alSourceUnqueueBuffers(_id, 1, &buffer);
}

}
