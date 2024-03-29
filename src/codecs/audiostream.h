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

#ifndef OPENAWE_AUDIOSTREAM_H
#define OPENAWE_AUDIOSTREAM_H

#include <vector>

#include "src/common/types.h"

namespace Codecs {

/*!
 * \brief Base class for audio streams
 *
 * This class offers all methods to use an audio stream including metadata of sample rate, bits ber sample, channel
 * count and if the samples are signed. It also offers two ways of reading the audio stream, either by reading a certain
 * number of samples for big streamed media like music or by reading the complete media for example for short sounds
 */
class AudioStream {
public:
	virtual ~AudioStream() = default;

	/*!
	 * Get the sample rate of the audio stream
	 * \return The sample rate of the audio stream
	 */
	unsigned int getSampleRate() const;

	/*!
	 * Get the number of bits per sample and per channel for this audio stream
	 * \return The number ob bits per sample of the audio stream
	 */
	unsigned int getBitsPerSample() const;

	/*!
	 * Get the number of channels for this audio stream
	 * \return The number of channels for this audio stream
	 */
	unsigned short getChannelCount() const;

	/*!
	 * Get if the samples of this audio stream are signed
	 * \return If the samples of this audio stream are signed
	 */
	bool isSigned() const;

	/*!
	 * Get if the stream is at its end
	 * \return If the stream reached its end
	 */
	virtual bool eos() const = 0;

	/*!
	 * Get the current position in the audio stream in samples
	 * \return The current audio streams position in samples
	 */
	virtual size_t pos() const = 0;

	/*!
	 * Read a specified number of samples from the audio stream
	 * \param numSamples The number of samples to be read
	 * \return A buffer of byte containing the samples
	 */
	virtual std::vector<byte> read(size_t numSamples) = 0;

protected:
	/*!
	 * Create a new audio stream by giving the general parameters of it
	 * \param sampleRate The sample rate of the audio stream
	 * \param bitsPerSample The bits per sample for the audio stream
	 * \param channelCount The number of channels for the audio stream
	 * \param isSigned If the audio stream is signed
	 */
	AudioStream(unsigned int sampleRate, unsigned int bitsPerSample, unsigned short channelCount, bool isSigned);

private:
	const unsigned int _sampleRate;
	const unsigned int _bitsPerSample;
	const unsigned short _channelCount;
	const bool _signed;
};

class SeekableAudioStream : public AudioStream {
public:
	enum SeekType {
		kBegin,
		kCurrent,
		kEnd
	};

	/*!
	 * Get the total number of samples in this audio stream
	 * \return The total number of samples in this audio stream
	 */
	unsigned int getTotalSamples() const;

	/*!
	 * Seek a number of samples in a direction in the stream
	 * \param samples The number of samples to seek
	 * \param type If the seeking should start from the begin, end or the current position
	 */
	virtual void seek(ptrdiff_t samples, SeekType type = kBegin) = 0;

	/*!
	 * Read the complete audio stream into a buffer with the total number of samples
	 * \return A buffer of bytes containing the complete samples of the stream
	 */
	std::vector<byte> readAll();

protected:
	/*!
	 * Create a new seekable audio stream by giving the general parameters of it
	 * \param sampleRate The sample rate of the audio stream
	 * \param bitsPerSample The bits per sample for the audio stream
	 * \param totalSamples The total number of samples in this file
	 * \param channelCount The number of channels for the audio stream
	 * \param isSigned If the audio stream is signed
	 */
	SeekableAudioStream(
		unsigned int sampleRate,
		unsigned int bitsPerSample,
		unsigned int totalSamples,
		unsigned short channelCount,
		bool isSigned
	);

private:
	const unsigned int _totalSamples;
};

} // End of namespace Codecs

#endif //OPENAWE_AUDIOSTREAM_H
