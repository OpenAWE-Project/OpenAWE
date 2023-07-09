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

#include <filesystem>

#include "src/common/bitstream.h"
#include "src/common/exception.h"

#include "src/codecs/adpcm.h"

#include "src/sound/fsbfile.h"

static const uint32_t kFSB4 = MKTAG('F', 'S', 'B', '4');
static const uint32_t kFSB5 = MKTAG('F', 'S', 'B', '5');

enum SampleFlags {
	k8Bits    = 0x00000008,
	k16Bits   = 0x00000010,
	kMono     = 0x00000020,
	kStereo   = 0x00000040,
	kSigned   = 0x00000100,
	kMP3      = 0x00000200,
	kImaAdpcm = 0x00400000
};

enum SoundFormatFSB5 {
	kFormatUnknown    =  0,
	kFormatPCM8       =  1,
	kFormatPCM16      =  2,
	kFormatPCM24      =  3,
	kFormatPCM32      =  4,
	kFormatPCMFloat   =  5,
	kFormatGCADPCM    =  6,
	kFormatIMAADPCM   =  7,
	kFormatVAG        =  8,
	kFormatHEVAG      =  9,
	kFormatXMA        = 10,
	kFormatMPEG       = 11,
	kFormatCELT       = 12,
	kFormatAT9        = 13,
	kFormatXWMA       = 14,
	kFormatVorbis     = 15,
	kFormatFmodADPCM  = 16,
	kFormatOpus       = 17
};

namespace Sound {

FSBFile::FSBFile(Common::ReadStream *fsb) : _fsb(fsb) {
	const auto magic = _fsb->readUint32BE();

	uint32_t numFiles, directoryLength, dataLength, extendedVersion, flags, nameTableSize = 0;

	switch (magic) {
		case kFSB4:
			numFiles        = _fsb->readUint32LE();
			directoryLength = _fsb->readUint32LE();
			dataLength      = _fsb->readUint32LE();
			extendedVersion = _fsb->readUint32LE();
			flags           = _fsb->readUint32LE();

			_fsb->skip(24);
			break;

		case kFSB5:
			extendedVersion = _fsb->readUint32LE();
			numFiles        = _fsb->readUint32LE();
			directoryLength = _fsb->readUint32LE();
			nameTableSize   = _fsb->readUint32LE();
			dataLength      = _fsb->readUint32LE();
			flags           = _fsb->readUint32LE();

			_fsb->skip(32);
			break;

		default:
			throw CreateException("Invalid magic FSB id");
	}

	const auto headerSize = _fsb->pos();

	_entries.resize(numFiles);

	size_t localOffset = 0;
	for (auto &entry : _entries) {
		switch (magic) {
			case kFSB4: {
				uint16_t fileEntryLength = _fsb->readUint16LE();

				entry.fileName = _fsb->readFixedSizeString(30, true);
				entry.fileName = std::filesystem::path(entry.fileName).stem().string() + ".wav";

				entry.totalSamples = _fsb->readUint32LE();
				entry.size = _fsb->readUint32LE();
				entry.offset = localOffset;
				localOffset += entry.size;
				entry.streamData.loopStart = _fsb->readUint32LE();
				entry.streamData.loopEnd = _fsb->readUint32LE();
				entry.flags = _fsb->readUint32LE();
				entry.sampleRate = _fsb->readUint32LE();
				uint16_t volume = _fsb->readUint16LE();
				uint16_t pan = _fsb->readUint16LE();
				uint16_t pri = _fsb->readUint16LE();
				entry.numChannels = _fsb->readUint16LE();
				entry.streamData.minimumDistance = _fsb->readIEEEFloatLE();
				entry.streamData.maximumDistance = _fsb->readIEEEFloatLE();
				uint32_t variableFrequency = _fsb->readUint32LE();
				uint16_t variableVolume = _fsb->readUint16LE();
				uint16_t variablePan = _fsb->readUint16LE();

				break;
			}

			case kFSB5: {
				auto bits = Common::BitStream32LELSB(*fsb);

				const auto nextChunk = bits.read();
				const auto sampleRateType = bits.read(4);

				switch (sampleRateType) {
					case 0:  entry.sampleRate = 4000; break;
					case 1:	 entry.sampleRate = 8000; break;
					case 2:	 entry.sampleRate = 11000; break;
					case 3:	 entry.sampleRate = 12000; break;
					case 4:	 entry.sampleRate = 16000; break;
					case 5:	 entry.sampleRate = 22050; break;
					case 6:	 entry.sampleRate = 24000; break;
					case 7:	 entry.sampleRate = 32000; break;
					case 8:	 entry.sampleRate = 44100; break;
					case 9:	 entry.sampleRate = 48000; break;
					case 10: entry.sampleRate = 96000; break;
					default:
						throw CreateException("Invalid sample rate type {}", sampleRateType);
				}

				switch (flags) {
					case kFormatUnknown:
						throw CreateException("Unknown audio coding is not supported");

					case kFormatIMAADPCM:  entry.format = kIMAADPCM; break;
					case kFormatFmodADPCM: entry.format = kFmodADPCM; break;

					case kFormatPCM8:
					case kFormatPCM16:
					case kFormatPCM24:
					case kFormatPCM32:
					case kFormatPCMFloat:
					case kFormatGCADPCM:
					case kFormatVAG:
					case kFormatHEVAG:
					case kFormatXMA:
					case kFormatMPEG:
					case kFormatCELT:
					case kFormatAT9:
					case kFormatXWMA:
					case kFormatVorbis:
					case kFormatOpus:
						throw CreateException("Unimplemented FMOD FSB5 codec {}", flags);

					default:
						throw CreateException("Unknown FMOD FSB5 codec {}", flags);
				}

				entry.numChannels = 1 << bits.read(2);
				entry.offset = bits.read(25) * 32;
				entry.totalSamples = bits.read(30) / (sizeof(int16_t) * entry.numChannels);
				entry.size = dataLength;

				if (nextChunk)
					throw CreateException("Extended entries arenot yet implemented");

				break;
			}
		}
	}

	if (magic == kFSB5) {
		_fsb->seek(headerSize + directoryLength);

		std::vector<uint32_t> offsets(numFiles);
		for (auto &offset: offsets)
			offset = _fsb->readUint32LE();

		for (size_t i = 0; i < _entries.size(); ++i) {
			_fsb->seek(headerSize + directoryLength + offsets[i]);
			_entries[i].fileName = std::filesystem::path(_fsb->readNullTerminatedString()).stem().string() + ".wav";
		}
	}

	_dataOffset = headerSize + directoryLength + nameTableSize;
}

size_t FSBFile::getNumEntries() {
	return _entries.size();
}

Codecs::SeekableAudioStream *FSBFile::getStream(size_t index) {
	const auto entry = _entries[index];

	_fsb->seek(_dataOffset + entry.offset);

	switch (entry.format) {
		case kFmodADPCM:
			return new Codecs::FmodAdpcmStream(
					_fsb->readStream(entry.size),
					entry.sampleRate,
					entry.numChannels,
					entry.totalSamples
			);
		case kIMAADPCM:
			return new Codecs::MsImaAdpcmStream(
					_fsb->readStream(entry.size),
					entry.sampleRate,
					entry.numChannels,
					entry.totalSamples,
					8
			);
		default:
			throw CreateException("Unimplemented codec {}", entry.format);
	}
}

Codecs::SeekableAudioStream *FSBFile::getStream(size_t index, ExtraData &streamData) {
	const auto entry = _entries[index];

	_fsb->seek(_dataOffset + entry.offset);

	streamData = entry.streamData;

	switch (entry.format) {
		case kFmodADPCM:
			return new Codecs::FmodAdpcmStream(
					_fsb->readStream(entry.size),
					entry.sampleRate,
					entry.numChannels,
					entry.totalSamples
			);
		case kIMAADPCM:
			return new Codecs::MsImaAdpcmStream(
					_fsb->readStream(entry.size),
					entry.sampleRate,
					entry.numChannels,
					entry.totalSamples,
					8
			);
		default:
			throw CreateException("Unimplemented codec {}", entry.format);
	}
}

std::string FSBFile::getFileName(size_t index) {
	const auto entry = _entries[index];
	return entry.fileName;
}

} // End of namespace Sound
