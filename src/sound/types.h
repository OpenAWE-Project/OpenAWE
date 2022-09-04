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

#ifndef SOUND_TYPES_H
#define SOUND_TYPES_H

#include "src/common/exception.h"

#include "src/sound/openal.h"

namespace Sound {

enum AudioFormat {
	kFormatMono8    = AL_FORMAT_MONO8,
	kFormatMono16   = AL_FORMAT_MONO16,
	kFormatStereo8  = AL_FORMAT_STEREO8,
	kFormatStereo16 = AL_FORMAT_STEREO16
};

inline AudioFormat getAudioFormat(unsigned short numChannels, unsigned short bitsPerSample) {
	switch (numChannels) {
		case 1:
			switch (bitsPerSample) {
				case 8:  return kFormatMono8;
				case 16: return kFormatMono16;
				default: break;
			}
			break;
		case 2:
			switch (bitsPerSample) {
				case 8:  return kFormatStereo8;
				case 16: return kFormatStereo16;
				default: break;
			}
			break;
		default:
			break;
	}

	throw CreateException("Invalid audio format with {} channels and {} bits per sample", numChannels, bitsPerSample);
}

} // End of namespace Sound

#endif // SOUND_TYPES_H
