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

#include "fsbfile.h"

static const uint32_t kFSB4 = MKTAG('F', 'S', 'B', '4');

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

		if (fileEntryLength != 80)
			throw std::runtime_error("Invalid FSB entry length");

		std::string fileName = _fsb->readFixedSizeString(30, true);

		uint32_t samplesLength = _fsb->readUint32LE();
		entry.size = _fsb->readUint32LE();
		entry.offset = localOffset;
		localOffset += entry.size;
		uint32_t loopStart = _fsb->readUint32LE();
		uint32_t loopEnd = _fsb->readUint32LE();
		uint32_t mode = _fsb->readUint32LE();
		uint32_t sampleRate = _fsb->readUint32LE();
		uint16_t volume = _fsb->readUint32LE();
		uint16_t pan = _fsb->readUint16LE();
		uint16_t pri = _fsb->readUint16LE();
		uint16_t numChannels = _fsb->readUint16LE();
		uint32_t minimumDistance = _fsb->readUint32LE();
		uint32_t maximumDistance = _fsb->readUint32LE();
		uint32_t variableFrequency = _fsb->readUint32LE();
		uint16_t variableVolume = _fsb->readUint32LE();
		uint16_t variablePan = _fsb->readUint32LE();
	}

	_dataOffset = 48 + directoryLength;
}

size_t FSBFile::getNumEntries() {
	return _entries.size();
}

Common::ReadStream *FSBFile::getStream(size_t index) {
	const auto entry = _entries[index];

	_fsb->seek(_dataOffset + entry.offset);

	return _fsb->readStream(entry.size);
}

} // End of namespace Sound
