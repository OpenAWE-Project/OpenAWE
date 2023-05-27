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
#include <iostream>

#include "src/common/threadpool.h"

#include "src/sound/stream.h"
#include "src/common/exception.h"

namespace Sound {

Stream::Stream(Codecs::AudioStream *stream) :
	_format(getAudioFormat(stream->getChannelCount(), stream->getBitsPerSample())),
	_stream(stream),
	_playing(false) {
	setRelative(true);
	_buffers.resize(32);
	alGenBuffers(32, _buffers.data());
	for (const auto &buffer: _buffers) {
		_availableBuffers.push_back(buffer);
	}
}

Stream::~Stream() {
	if (isPlaying())
		stop();

	// Unqueue all used buffers to delete them
	for (const auto &usedBuffer: _usedBuffers)
		unqueueBuffer(usedBuffer);

	alDeleteBuffers(32, _buffers.data());
}

void Stream::play() {
	_playing = true;
	update();
	Source::play();
}

void Stream::stop() {
	_stopped.lock();
	_playing = false;
	Source::stop();
	_stopped.lock();
	_stopped.unlock();
}

void Stream::update() {
	if (!_playing) {
		_stopped.unlock();
		return;
	}

	// Try to fill as many buffers as possible
	while (!_availableBuffers.empty() && !_stream->eos()) {
		ALuint nextBuffer = _availableBuffers.front();
		_availableBuffers.pop_front();

		const auto sampleData = _stream->read(4096);
		alBufferData(
			nextBuffer,
			_format,
			sampleData.data(),
			sampleData.size(),
			_stream->getSampleRate()
		);

		queueBuffer(nextBuffer);
		_usedBuffers.push_back(nextBuffer);
	}

	// And unqueue buffers already played
	for (unsigned int i = 0; i < getBuffersProcessed(); ++i) {
		ALuint nextBuffer = _usedBuffers.front();
		_usedBuffers.pop_front();
		unqueueBuffer(nextBuffer);
		_availableBuffers.push_back(nextBuffer);
	}

	// If there are no more used buffers and the stream is at its end, stop the whole thing
	if (_usedBuffers.empty() && _stream->eos()) {
		_playing = false;
		return;
	}

	Threads.add([this]{ update(); });
}

LoopableStream::LoopableStream(Codecs::SeekableAudioStream *stream) :
	Stream(stream),
	_seekableStream(stream),
	_loopStart(0),
	_loopEnd(std::numeric_limits<size_t>::max()) {
	setRelative(true);
	_buffers.resize(32);
	alGenBuffers(32, _buffers.data());
	for (const auto &buffer: _buffers) {
		_availableBuffers.push_back(buffer);
	}
}

void LoopableStream::setLoopRange(unsigned int start, unsigned int end) {
	_loopStart = start;
	_loopEnd = end;
}

void LoopableStream::update() {
	if (!_playing) {
		_stopped.unlock();
		return;
	}

	// Try to fill as many buffers as possible
	while (!_availableBuffers.empty()) {
		ALuint nextBuffer = _availableBuffers.front();
		_availableBuffers.pop_front();

		const auto samplesToRead = std::min<size_t>(std::min<size_t>(_seekableStream->getTotalSamples(), _loopEnd) - _stream->pos(), 4096);
		const auto sampleData = _stream->read(samplesToRead);
		alBufferData(
			nextBuffer,
			_format,
			sampleData.data(),
			sampleData.size(),
			_stream->getSampleRate()
		);

		if (_stream->pos() >= _loopEnd)
			_seekableStream->seek(_loopStart);

		queueBuffer(nextBuffer);
		_usedBuffers.push_back(nextBuffer);
	}

	// And unqueue buffers already played
	for (unsigned int i = 0; i < getBuffersProcessed(); ++i) {
		ALuint nextBuffer = _usedBuffers.front();
		_usedBuffers.pop_front();
		unqueueBuffer(nextBuffer);
		_availableBuffers.push_back(nextBuffer);
	}

	Threads.add([this]{ update(); });
}

} // End of namespace Sound
