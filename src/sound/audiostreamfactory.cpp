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

#include "src/common/exception.h"

#include "src/awe/resman.h"

#include "src/codecs/fsbfile.h"

#include "src/sound/audiostreamfactory.h"

namespace Sound {

AudioStreamFactory::AudioStreamFactory() : _rid(0) {}

AudioStreamFactory::AudioStreamFactory(rid_t rid) : _rid(rid) {
}

LoopableStream * AudioStreamFactory::createStream() const {
	Common::ReadStream *fsbStream(ResMan.getResource(_rid));
	if (!fsbStream)
		throw CreateException("Cannot find fsb resource {:X}", _rid);

	Codecs::FSBFile fsb(fsbStream);

	if (fsb.getNumEntries() == 0)
		throw CreateException("Empty FMOD sound bank file {:x}", _rid);

	Codecs::FSBFile::ExtraData streamData{};
	Codecs::SeekableAudioStream *audioStream = fsb.getStream(0, streamData);
	auto *stream = new Sound::LoopableStream(audioStream);
	stream->setLoopRange(streamData.loopStart, streamData.loopEnd);

	return stream;
}

} // End of namespace Sound
