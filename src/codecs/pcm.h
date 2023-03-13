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

#ifndef OPENAWE_PCM_H
#define OPENAWE_PCM_H

#include <memory>

#include "src/common/readstream.h"

#include "src/codecs/audiostream.h"

namespace Codecs {

/*!
 * \brief Implementation of raw uncompressed pcm data
 *
 * This class implements the reading of raw uncompressed pulse code modulation (pcm) data as an audio stream. The size
 * of a sample is defined by the template parameter.
 *
 * \tparam SampleType The type of one sample read by this stream
 */
template<typename SampleType>
class PcmStream : public SeekableAudioStream {
public:
	/*!
	 * Create a new audio stream from an existing read stream. The pcm stream takes ownership of the read stream.
	 * \param pcm The stream of pcm data to read from
	 * \param sampleRate The samplerate of the stream
	 * \param channelCount The number of channels in this stream
	 */
	PcmStream(Common::ReadStream *pcm, unsigned int sampleRate, unsigned short channelCount) :
		SeekableAudioStream(
			sampleRate,
			sizeof(SampleType) * 8,
			pcm->size() / (sizeof(SampleType) * channelCount),
			channelCount,
			std::numeric_limits<SampleType>::is_signed
		),
		_pcm(pcm) {
	}

	/*!
	 * Check if the audio stream has reached its end
	 * \return If the stream reached its end
	 */
	bool eos() const override {
		return _pcm->eos();
	}

	/*!
	 * Return the current position inside the stream
	 * \return The current position in the stream in samples
	 */
	size_t pos() const override {
		return _pcm->pos() / (sizeof(SampleType) * getChannelCount());
	}

	/*!
	 * Read a number of samples from this stream
	 * \param numSamples The number of samples
	 * \return A buffer of the read sample data
	 */
	std::vector<byte> read(size_t numSamples) override {
		const auto sampleBlockSize = numSamples * sizeof(SampleType) * getChannelCount();
		std::vector<byte> sampleData(sampleBlockSize);
		_pcm->read(sampleData.data(), sampleBlockSize);
		return sampleData;
	}

	/*!
	 * Seek a num samples from a given origin
	 * \param samples The number of samples to seek from the origin left or right
	 * \param type The origin from which to seek
	 */
	void seek(ptrdiff_t samples, SeekType type) override {
		Common::ReadStream::SeekOrigin origin = Common::ReadStream::BEGIN;
		switch (type) {
			case SeekableAudioStream::kBegin:
				origin = Common::ReadStream::BEGIN;
				break;
			case SeekableAudioStream::kCurrent:
				origin = Common::ReadStream::CURRENT;
				break;
			case SeekableAudioStream::kEnd:
				origin = Common::ReadStream::END;
				break;
		}
		_pcm->seek(samples * sizeof(SampleType) * getChannelCount(), origin);
	}

private:
	std::unique_ptr<Common::ReadStream> _pcm;
};

// Implements specific PCM types
typedef PcmStream<uint8_t>  PcmStreamU8;
typedef PcmStream<int8_t>   PcmStreamS8;
typedef PcmStream<uint16_t> PcmStreamU16;
typedef PcmStream<int16_t>  PcmStreamS16;

} // End of namespace Codecs

#endif //OPENAWE_PCM_H
