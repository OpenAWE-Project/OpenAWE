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

#include <algorithm>
#include <cstring>

#include "src/common/exception.h"

#include "src/codecs/adpcm.h"

static const int kIMAIndexTable[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

static const int kIMAStepTable[89] = {
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

namespace Codecs {

AdpcmStream::AdpcmStream(
	Common::ReadStream *stream,
	unsigned int sampleRate,
	unsigned short channelCount,
	unsigned int totalSamples
) :
	SeekableAudioStream(sampleRate, 16, totalSamples, channelCount, true),
	_adpcm(stream) {
}

bool AdpcmStream::eos() const {
	return _adpcm->eos();
}

ImaAdpcmStream::ImaAdpcmStream(
	Common::ReadStream *stream,
	unsigned int sampleRate,
	unsigned short channelCount,
	unsigned int totalSamples
) :
	AdpcmStream(stream, sampleRate, channelCount, totalSamples) {
}

int16_t ImaAdpcmStream::decodeSample(byte nibble, int &stepIndex, int16_t last) {
	int32_t E = (2 * (nibble & 0x7) + 1) * kIMAStepTable[stepIndex] / 8;
	int32_t diff = (nibble & 0x08) ? -E : E;
	stepIndex += kIMAIndexTable[nibble];
	stepIndex = std::clamp<int>(stepIndex, 0, 88);
	return std::clamp<int16_t>(last + diff, -32768, 32767);
}

MsImaAdpcmStream::MsImaAdpcmStream(
	Common::ReadStream *stream,
	unsigned int sampleRate,
	unsigned short channelCount,
	unsigned int totalSamples,
	unsigned int numBlockChunks
) :
	ImaAdpcmStream(stream, sampleRate, channelCount, totalSamples),
	_numBlockChunks(numBlockChunks) {
	if (channelCount != 1 && channelCount != 2)
		throw CreateException("Invalid number of channels {}, only mono and stereo supported", channelCount);

	decodeBlock();
}

std::vector<byte> MsImaAdpcmStream::read(size_t numSamples) {
	std::vector<byte> data(numSamples * sizeof(int16_t) * getChannelCount());

	size_t remainingSamples = numSamples;
	while (remainingSamples > 0) {
		const auto samplesToRead = std::min<size_t>(_remainder, remainingSamples);
		const auto bytesToRead = samplesToRead * sizeof(uint16_t) * getChannelCount();
		const auto bytesRead = (numSamples - remainingSamples) * sizeof(uint16_t) * getChannelCount();
		std::memcpy(
			data.data() + bytesRead,
			_buffer.data() + (_numBlockChunks * 8 - _remainder) * sizeof(uint16_t) * getChannelCount(),
			bytesToRead
		);
		_remainder -= samplesToRead;
		remainingSamples -= samplesToRead;
		if (_adpcm->eos()) {
			std::vector<byte> oldData = data;
			data.resize((numSamples - remainingSamples) * sizeof(int16_t) * getChannelCount());
			std::memcpy(data.data(), oldData.data(), (numSamples - remainingSamples) * sizeof(int16_t) * getChannelCount());
			break;
		}
		decodeBlock();
	}

	return data;
}

size_t MsImaAdpcmStream::pos() const {
	const unsigned int bytesPerBlock = (4 + _numBlockChunks * 4) * getChannelCount();
	const unsigned int samplesPerBlock = _numBlockChunks * 8;
	return
		// a full block multplied by the
		(_adpcm->pos() / bytesPerBlock) * samplesPerBlock +
		// The sample position in the current block
		(samplesPerBlock - _remainder) % samplesPerBlock;
}

void MsImaAdpcmStream::seek(ptrdiff_t samples, SeekableAudioStream::SeekType type) {
	Common::ReadStream::SeekOrigin seekOrigin;
	switch (type) {
		case SeekableAudioStream::kBegin:
			seekOrigin = Common::ReadStream::BEGIN;
			_remainder = 0;
			break;

		case SeekableAudioStream::kEnd:
			seekOrigin = Common::ReadStream::END;
			_remainder = 0;
			break;

		case SeekableAudioStream::kCurrent:
			seekOrigin = Common::ReadStream::CURRENT;
			break;
	}

	// Calculate how many bytes to seek pat the full blocks
	const int bytesPerBlock = (4 + static_cast<int>(_numBlockChunks) * 4) * getChannelCount();
	const int samplesPerBlock = static_cast<int>(_numBlockChunks) * 8;
	const int blocksToSkip = ((samplesPerBlock - _remainder) % samplesPerBlock + samples) / samplesPerBlock;
	const int bytesToSkip = blocksToSkip * bytesPerBlock;

	if (samples < 0)
		_adpcm->seek(bytesToSkip - bytesPerBlock, seekOrigin);
	else
		_adpcm->seek(bytesToSkip, seekOrigin);

	decodeBlock();
	if (samples < 0)
		_remainder -= samplesPerBlock - std::abs(samples) % samplesPerBlock;
	else
		_remainder -= samples % samplesPerBlock;
}

void MsImaAdpcmStream::decodeBlock() {
	const auto channelCount = getChannelCount();

	// Read block header
	std::vector<int16_t> last(channelCount);
	std::vector<int> stepIndex(channelCount);
	for (int i = 0; i < channelCount; ++i) {
		last[i] = _adpcm->readSint16LE();
		stepIndex[i] = _adpcm->readByte();
		_adpcm->skip(1);
	}

	_buffer.resize(channelCount * _numBlockChunks * 8);

	_remainder = 0;
	// Decode a number of block chunks
	for (unsigned int k = 0; k < _numBlockChunks; ++k) {
		if (_adpcm->eos())
			break;

		// Decode one 4 byte left and 4 byte chunk for the block
		for (unsigned int i = 0; i < channelCount; ++i) {
			for (unsigned int j = 0; j < 4; ++j) {
				byte date = _adpcm->readByte();
				last[i] = _buffer[k * 16 + (j * channelCount + 0) * 2 + i] = decodeSample(date & 0x0f, stepIndex[i], last[i]);
				last[i] = _buffer[k * 16 + (j * channelCount + 1) * 2 + i] = decodeSample((date >> 4) & 0x0f, stepIndex[i], last[i]);
			}
		}

		// Increase the number of remainders
		_remainder += 8;
	}
}

} // End of namespace Codecs
