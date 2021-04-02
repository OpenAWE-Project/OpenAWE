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

#include <assert.h>
#include "src/sound/soundman.h"
#include "buffer.h"

namespace Sound {

Buffer::Buffer() {
	_id = SoundMan.registerBuffer();
	assert(alIsBuffer(_id) == AL_TRUE);
}

Buffer::~Buffer() {
	SoundMan.deregisterBuffer(_id);
}

void Buffer::bufferData(Format format, unsigned int frequency, void *data, size_t size) const {
	alBufferData(_id, format, data, size, frequency);
	assert(alGetError() == AL_NO_ERROR);
}

void Buffer::queueBuffer(ALuint sourceId) {
	alSourceQueueBuffers(sourceId, 1, &_id);
	assert(alGetError() == AL_NO_ERROR);
}

void Buffer::unqueueBuffer(ALuint sourceId) {
	alSourceUnqueueBuffers(sourceId, 1, &_id);
}

}
