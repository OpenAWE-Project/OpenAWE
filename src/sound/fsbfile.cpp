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

#include "src/common/exception.h"

#include "src/codecs/adpcm.h"

#include "src/sound/fsbfile.h"

static const uint32_t kFSB4 = MKTAG('F', 'S', 'B', '4');

enum SampleFlags {
	kFormatMono     = 0x00000020,
	kFormatStereo   = 0x00000040,
	kFormatSigned   = 0x00000100,
	kFormatMP3      = 0x00000200,
	kFormatImaAdpcm = 0x00400000
};

namespace Sound {

FSBFile::FSBFile(Common::ReadStream *fsb) : _fsb(fsb) {
	if (_fsb->readUint32BE() != kFSB4)
		throw std::runtime_error("Invalid magic FSB4 id");

	uint32_t numFiles        = _fsb->readUint32LE();
	uint32_t directoryLength = _fsb->readUint32LE();
	uint32_t dataLength      = _fsb->readUint32LE();
	uint32_t extendedVersion = _fsb->readUint32LE();
	uint32_t flags           = _fsb->readUint32LE();

	_fsb->skip(24);

	_entries.resize(numFiles);

	size_t localOffset = 0;
	for (auto &entry : _entries) {
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
	}

	_dataOffset = 48 + directoryLength;
}

size_t FSBFile::getNumEntries() {
	return _entries.size();
}

Codecs::SeekableAudioStream *FSBFile::getStream(size_t index) {
	const auto entry = _entries[index];

	_fsb->seek(_dataOffset + entry.offset);

	if (!(entry.flags & kFormatImaAdpcm))
		throw CreateException("Unknown and unimplemented audio codecs found");

	return new Codecs::MsImaAdpcmStream(
		_fsb->readStream(entry.size),
		entry.sampleRate,
		entry.numChannels,
		entry.totalSamples,
		8
	);
}

Codecs::SeekableAudioStream *FSBFile::getStream(size_t index, ExtraData &streamData) {
	const auto entry = _entries[index];

	_fsb->seek(_dataOffset + entry.offset);

	if (!(entry.flags & kFormatImaAdpcm))
		throw CreateException("Unknown and unimplemented audio codecs found");

	streamData = entry.streamData;

	return new Codecs::MsImaAdpcmStream(
		_fsb->readStream(entry.size),
		entry.sampleRate,
		entry.numChannels,
		entry.totalSamples,
		8
	);
}

std::string FSBFile::getFileName(size_t index) {
	const auto entry = _entries[index];
	return entry.fileName;
}

} // End of namespace Sound
