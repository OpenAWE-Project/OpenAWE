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

#include "src/codecs/dumpwav.h"

namespace Codecs {

void dumpWAV(
	Codecs::SeekableAudioStream &audio,
	Common::WriteStream &wav
) {
	const auto data = audio.readAll();
	const auto size = audio.getTotalSamples();
	const auto numChannels = audio.getChannelCount();
	const auto sampleRate = audio.getSampleRate();
	const auto bitsPerSample = audio.getBitsPerSample();
	unsigned int riffSize = 4 + 24 + 8 + size;

	// Write RIFF header
	wav.writeUint32BE(MKTAG('R', 'I', 'F', 'F'));
	wav.writeUint32LE(riffSize);
	wav.writeUint32BE(MKTAG('W', 'A', 'V', 'E'));

	// Write fmt chunk
	wav.writeUint32BE(MKTAG('f', 'm', 't', ' '));
	wav.writeUint32LE(16);
	wav.writeUint16LE(1); // PCM
	wav.writeUint16LE(numChannels); // Channels
	wav.writeUint32LE(sampleRate); // Sample Rate
	wav.writeUint32LE(sampleRate * numChannels * (bitsPerSample / 8)); // Bitrate
	wav.writeUint16LE((bitsPerSample * numChannels) / 8); // Bytes per sample
	wav.writeUint16LE(bitsPerSample); // Bits per sample

	// Write data chunk
	wav.writeUint32BE(MKTAG('d', 'a', 't', 'a'));
	wav.writeUint32LE(size);
	wav.write(data.data(), size);
}

} // End of namespace Codecs
