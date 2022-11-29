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

#include <iostream>
#include <algorithm>
#include "dpfile.h"

DPFile::DPFile(Common::ReadStream *dp) : _dp(dp) {
	uint32_t numValues, numStrings;

	testHeader();

	switch (_headerType) {
		case kHeaderV2:
			numValues = _dp->readUint32LE() + _dp->readUint32LE();
			numStrings = _dp->readUint32LE();
			_dataSize = _dp->readUint32LE();

			_dp->skip(12); // Always 0?
			break;

		case kHeaderV1:
			numValues = _dp->readUint32LE();
			numStrings = _dp->readUint32LE();
			_dataSize = _dp->readUint32LE();

			_dp->skip(8);
			break;

		default:
			throw std::runtime_error("Not supported header");
	}

	_valueOffsets.resize(numValues);
	for (auto &offset : _valueOffsets) {
		offset = _dp->readUint32LE();
	}

	_stringOffsets.resize(numStrings);
	for (auto &offset : _stringOffsets) {
		offset = _dp->readUint32LE();
	}

	for (const auto &item : _stringOffsets) {
		bool overlap = (item & 0x80u) != 0;

		int32_t offset = item >> 8u;

		_dp->seek(-static_cast<int>(_dataSize) + offset * 8, Common::ReadStream::END);
		if (overlap)
			_dp->skip(4);

		std::string s = _dp->readNullTerminatedString();
	}
}

bool DPFile::hasString(uint32_t offset) {
	return std::find(_stringOffsets.begin(), _stringOffsets.end(), offset) != _stringOffsets.end();
}

std::string DPFile::getString(uint32_t offset) {
	if ((offset & 0x000000FFu) == 0)
		return "";

	if (std::find(std::begin(_stringOffsets), std::end(_stringOffsets), offset) == _stringOffsets.end())
		return "";

	bool overlap = (offset & 0x80u) != 0;

	int32_t relativeOffset = offset >> 8u;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset * 8, Common::ReadStream::END);
	if (overlap)
		_dp->skip(4);

	std::string string = _dp->readNullTerminatedString();

	return string;
}

std::vector<uint32_t> DPFile::getValues(uint32_t offset, unsigned int count) {
	std::vector<uint32_t> values(count);

	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	/*if (std::find(std::begin(_valueOffsets), std::end(_valueOffsets), relativeOffset) == _valueOffsets.end())
		return values;*/

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);
	for (auto &value : values) {
		value = _dp->readUint32LE();
	}

	return values;
}

std::vector<float> DPFile::getFloats(uint32_t offset, unsigned int count) {
	std::vector<float> values(count);

	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	if (std::find(std::begin(_valueOffsets), std::end(_valueOffsets), relativeOffset) == _valueOffsets.end())
		return values;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);
	for (auto &value : values) {
		value = _dp->readIEEEFloatLE();
	}

	return values;
}

std::vector<glm::vec2> DPFile::getPositions2D(uint32_t offset, unsigned int count) {
	std::vector<glm::vec2> positions(count);

	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);
	for (auto &position : positions) {
		position.x = _dp->readIEEEFloatLE();
		position.y = _dp->readIEEEFloatLE();
	}

	return positions;
}

std::vector<GID> DPFile::getGIDs(uint32_t offset, unsigned int count) {
	std::vector<GID> gids(count);

	bool overlap = (offset & 0x80u) != 0;
	uint32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + static_cast<int>(relativeOffset), Common::ReadStream::END);
	for (auto &gid: gids) {
		gid.type = _dp->readUint32LE();
		gid.id = _dp->readUint32BE();
		_dp->skip(8); // Always 0? Maybe aligning?
	}

	return gids;
}

std::vector<DPFile::ScriptMetadata> DPFile::getScriptMetadata(uint32_t offset, unsigned int count) {
	std::vector<ScriptMetadata> metadata(count);

	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);

	for (auto &item : metadata) {
		item.offset = _dp->readUint32LE();
		item.name = _dp->readUint32LE();
	}

	return metadata;
}

std::vector<DPFile::ScriptSignal> DPFile::getScriptSignals(uint32_t offset, unsigned int count) {
	std::vector<ScriptSignal> scriptSignal(count);

	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);

	for (auto &gidDatum : scriptSignal) {
		gidDatum.gid.type = _dp->readUint32LE();
		gidDatum.gid.id = _dp->readUint32LE();
		gidDatum.nameOffset = _dp->readUint32LE();
		_dp->skip(4);
	}

	return scriptSignal;
}

std::vector<DPFile::ScriptDebugEntry> DPFile::getScriptDebugEntries(uint32_t offset, unsigned int count) {
	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);

	std::vector<ScriptDebugEntry> debugEntries(count);
	for (auto &debugEntry : debugEntries) {
		debugEntry.id = _dp->readUint32LE();
		debugEntry.type = _dp->readUint32LE();
		debugEntry.nameOffset = _dp->readUint32LE();
	}

	return debugEntries;
}

Common::ReadStream * DPFile::getStream(uint32_t offset, unsigned int length) {
	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);
	return _dp->readStream(length * 4);
}

void DPFile::readTaskData1(uint32_t offset, unsigned int count) {
	bool overlap = (offset & 0x80u) != 0;
	int32_t relativeOffset = (offset >> 8u) * 8;
	if (overlap)
		relativeOffset += 4;

	_dp->seek(-static_cast<int>(_dataSize) + relativeOffset, Common::ReadStream::END);

	TaskData1 taskData1;
	for (unsigned int i = 0; i < count; ++i) {
		taskData1.count = _dp->readUint32LE();
		taskData1.hash = _dp->readUint32LE();
		_dp->skip(8); // Always zero?
	}
}

void DPFile::testHeader() {
	uint32_t numValues, numReferences, numStrings, dataSize;

	_dp->seek(0, Common::ReadStream::END);
	uint32_t fileSize = _dp->pos();
	_dp->seek(0);

	// Test if it is a V1 header
	numValues = _dp->readUint32LE();
	numStrings = _dp->readUint32LE();
	dataSize = _dp->readUint32LE();

	_dp->seek(0);

	if (20 + numValues * 4 + numStrings * 4 + dataSize == fileSize) {
		_headerType = kHeaderV1;
		return;
	}

	// Test if it is a V2 header
	numValues = _dp->readUint32LE();
	numReferences = _dp->readUint32LE();
	numStrings = _dp->readUint32LE();
	dataSize = _dp->readUint32LE();

	_dp->seek(0);

	if (28 + numValues * 4 + numReferences * 4 + numStrings * 4 + dataSize == fileSize) {
		_headerType = kHeaderV2;
		return;
	}

	throw std::runtime_error("Invalid dp file, could determine header type");
}
