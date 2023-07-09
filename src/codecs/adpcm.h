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

#ifndef OPENAWE_ADPCM_H
#define OPENAWE_ADPCM_H

#include <memory>

#include "src/common/readstream.h"

#include "src/codecs/audiostream.h"

namespace Codecs {

/*!
 * \brief Base class for adpcm sound decoding
 *
 * This class serves as base class for different adpcm schemes implemented for decoding.
 */
class AdpcmStream : public SeekableAudioStream {
public:
	bool eos() const override;

protected:
	AdpcmStream(Common::ReadStream *stream, unsigned int sampleRate, unsigned short channelCount, unsigned int totalSamples);

	std::unique_ptr<Common::ReadStream> _adpcm;
	unsigned int _remainder;
	std::vector<int16_t> _buffer;
};

/*!
 * \brief Variant of adpcm used by the fmod engine
 *
 * This adpcm variant class implements a scheme used by the fmod engine for audio contained in their fsb container
 * files.
 * The scheme builds up on blocks of 140 bytes which contain 256 samples. In the case of stereo streams the audio is
 * interleaved on the layer of the blocks, maning the.
 */
class FmodAdpcmStream : public AdpcmStream {
public:
	FmodAdpcmStream(
		Common::ReadStream *stream,
		unsigned int sampleRate,
		unsigned short channelCount,
		unsigned int totalSamples
	);

	size_t pos() const override;

	std::vector<byte> read(size_t numSamples) override;

	void seek(ptrdiff_t samples, SeekType type) override;

private:
	void decodeBlock();
};

/*!
 * \brief Base class for ima adpcm variant decoding
 *
 * This class is a base for decoding ima adpcm schemes. It offers basic methods for decoding ima adpcm samples
 */
class ImaAdpcmStream : public AdpcmStream {
public:
	ImaAdpcmStream(
		Common::ReadStream *stream,
		unsigned int sampleRate,
		unsigned short channelCount,
		unsigned int totalSamples
	);

protected:
	int16_t decodeSample(byte nibble, int &stepIndex, int16_t last);
};

/*!
 * \brief Implementing Microsofts IMA ADPCM stream
 *
 * This class implements Microsofts ima adpcm stream scheme, in which the data is split into blocks with a maximum
 * number of chunks. They have starting values for the IMA ADPCM scheme. Every one of these chunks switches between
 * left and right audio in stereo mode every 4 bytes.
 */
class MsImaAdpcmStream : public ImaAdpcmStream {
public:
	MsImaAdpcmStream(
		Common::ReadStream *stream,
		unsigned int sampleRate,
		unsigned short channelCount,
		unsigned int totalSamples,
		unsigned int numBlockChunks
	);

	void seek(ptrdiff_t samples, SeekType type) override;

	std::vector<byte> read(size_t numSamples) override;

	size_t pos() const override;

private:
	const unsigned int _numBlockChunks;

	void decodeBlock();
};

} // End of namespace Codecs

#endif //OPENAWE_ADPCM_H
