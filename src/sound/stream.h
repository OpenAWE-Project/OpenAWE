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

#ifndef AWE_STREAM_H
#define AWE_STREAM_H

#include <queue>
#include <memory>

#include "src/codecs/audiostream.h"

#include "src/sound/source.h"
#include "src/sound/types.h"

namespace Sound {

class Stream : public Source {
public:
	explicit Stream(Codecs::AudioStream *stream);
	virtual ~Stream();

	void play() override;

protected:
	virtual void update();

	const ALenum _format;

	std::vector<ALuint> _buffers;
	std::deque<ALuint> _availableBuffers;
	std::deque<ALuint> _usedBuffers;
	std::unique_ptr<Codecs::AudioStream> _stream;

	bool _playing;
};

class LoopableStream : public Stream {
public:
	explicit LoopableStream(Codecs::SeekableAudioStream *stream);

	void setLoopRange(unsigned int start, unsigned int end);

protected:
	void update() override;

	Codecs::SeekableAudioStream *_seekableStream;
	size_t _loopStart, _loopEnd;
};

}

#endif //AWE_STREAM_H
