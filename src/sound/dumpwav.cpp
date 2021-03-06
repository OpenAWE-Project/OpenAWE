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

#include "src/sound/dumpwav.h"

namespace Sound {

void dumpWAV(Common::WriteStream &wav, void *data, size_t size) {
	unsigned int riffSize = 4 + 24 + 8 + size;

	// Write RIFF header
	wav.writeUint32BE(MKTAG('R', 'I', 'F', 'F'));
	wav.writeUint32LE(riffSize);
	wav.writeUint32BE(MKTAG('W', 'A', 'V', 'E'));

	// Write fmt chunk
	wav.writeUint32BE(MKTAG('f', 'm', 't', ' '));
	wav.writeUint32LE(16);
	wav.writeUint16LE(1); // PCM
	wav.writeUint16LE(1); // Channels
	wav.writeUint32LE(48000); // Sample Rate
	wav.writeUint32LE(96000); // Bitrate
	wav.writeUint16LE(2); // Bytes per sample
	wav.writeUint16LE(16); // Bits per sample

	// Write data chunk
	wav.writeUint32BE(MKTAG('d', 'a', 't', 'a'));
	wav.writeUint32LE(size);
	wav.write(data, size);
}

}
