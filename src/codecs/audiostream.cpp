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

#include "audiostream.h"

namespace Codecs {

unsigned int AudioStream::getSampleRate() const {
	return _sampleRate;
}

unsigned int AudioStream::getBitsPerSample() const {
	return _bitsPerSample;
}

unsigned short AudioStream::getChannelCount() const {
	return _channelCount;
}

bool AudioStream::isSigned() const {
	return _signed;
}

AudioStream::AudioStream(
	unsigned int sampleRate,
	unsigned int bitsPerSample,
	unsigned short channelCount,
	bool isSigned) :
	_sampleRate(sampleRate),
	_bitsPerSample(bitsPerSample),
	_channelCount(channelCount),
	_signed(isSigned) {
}

unsigned int SeekableAudioStream::getTotalSamples() const {
	return _totalSamples;
}

std::vector<byte> SeekableAudioStream::readAll() {
	return read(getTotalSamples());
}

SeekableAudioStream::SeekableAudioStream(
	unsigned int sampleRate,
	unsigned int bitsPerSample,
	unsigned int totalSamples,
	unsigned short channelCount,
	bool isSigned
	) :
	AudioStream(sampleRate, bitsPerSample, channelCount, isSigned),
	_totalSamples(totalSamples) {
}

} // End of namespace Codecs
