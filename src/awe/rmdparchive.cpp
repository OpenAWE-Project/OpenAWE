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

#include <regex>
#include <fmt/format.h>
#include <zlib.h>
#include <assert.h>

#include "src/common/strutil.h"
#include "src/common/memreadstream.h"
#include "src/common/exception.h"

#include "rmdparchive.h"

namespace AWE {

RMDPArchive::RMDPArchive(Common::ReadStream *bin, Common::ReadStream *rmdp) : _rmdp(rmdp) {
	_littleEndian = bin->readByte() == 0;

	uint32_t version;
	if (_littleEndian)
		version = bin->readUint32LE();
	else
		version = bin->readUint32BE();

	switch (version) {
		case 2: // Alan Wake
			loadHeaderV2(bin);
			break;
		case 7: // Alan Wakes American Nightmare
			loadHeaderV7(bin);
			break;
		case 8: // Quantum Break
			loadHeaderV8(bin);
			break;
		default:
			throw std::runtime_error(fmt::format("Unknown RMDP Archive version {}", version));
	}

	delete bin;
}

size_t RMDPArchive::getNumResources() const {
	return _fileEntries.size();
}

std::string RMDPArchive::getResourcePath(size_t index) const {
	// Check if the index is less than the number of resources
	if (index >= getNumResources())
		throw Common::Exception(
				"Index {} exceeds number of file entries {}",
				index,
				getNumResources()
		);


	const auto &fileEntry = _fileEntries[index];
	std::string path = fileEntry.name;

	// Check if the file is in the root directory
	if (fileEntry.prevFolder == 0xFFFFFFFF)
		return path;

	// Get the first folder entry
	auto folderEntry = _folderEntries[fileEntry.prevFolder];
	path = folderEntry.name + "/" + path;

	// Iterate over all preceeding folder entries
	while (folderEntry.prevFolder != 0xFFFFFFFF) {
		folderEntry = _folderEntries[folderEntry.prevFolder];
		if (!folderEntry.name.empty())
			path = folderEntry.name + "/" + path;
	}

	path = std::regex_replace(path, std::regex("d:/data/"), "");

	return path;
}

Common::ReadStream *RMDPArchive::getResource(const std::string &rid) const {
	std::stringstream path(
			std::regex_replace(
					(_pathPrefix ? "d:/data/" : "") + rid,
					std::regex("\\\\"),
					"/"
			)
	);
	std::string item;

	FolderEntry folder = _folderEntries.front();

	uint32_t nameHash = 0;

	while (std::getline(path, item, '/')) {
		nameHash = Common::crc32(Common::toLower(item));

		if (path.eof())
			break;

		if (folder.nextLowerFolder == 0xFFFFFFFF)
			return nullptr;

		folder = _folderEntries[folder.nextLowerFolder];

		while (nameHash != folder.nameHash) {
			if (folder.nextNeighbourFolder == 0xFFFFFFFF)
				return nullptr;
			folder = _folderEntries[folder.nextNeighbourFolder];
		}
	}

	if (folder.nextFile == 0xFFFFFFFF)
		return nullptr;

	FileEntry file = _fileEntries[folder.nextFile];

	while (file.nameHash != nameHash) {
		if (file.nextFile == 0xFFFFFFFF)
			return nullptr;
		file = _fileEntries[file.nextFile];
	}

	byte *data = new byte[file.size];
	_rmdp->seek(file.offset);
	_rmdp->read(data, file.size);

	assert(crc32(0L, data, file.size) == file.fileDataHash);

	return new Common::MemoryReadStream(data, file.size, true);
}

bool RMDPArchive::hasResource(const std::string &rid) const {
	std::stringstream path(
			std::regex_replace(
					(_pathPrefix ? "d:/data/" : "") + rid,
					std::regex("\\\\"),
					"/"
			)
	);
	std::string item;

	FolderEntry folder = _folderEntries.front();

	uint32_t nameHash;

	while (std::getline(path, item, '/')) {
		nameHash = Common::crc32(Common::toLower(item));

		if (path.eof())
			break;

		if (folder.nextLowerFolder == 0xFFFFFFFF)
			return false;

		folder = _folderEntries[folder.nextLowerFolder];

		while (nameHash != folder.nameHash) {
			if (folder.nextNeighbourFolder == 0xFFFFFFFF)
				return false;
			folder = _folderEntries[folder.nextNeighbourFolder];
		}
	}

	if (folder.nextFile == 0xFFFFFFFF)
		return false;

	FileEntry file = _fileEntries[folder.nextFile];

	while (file.nameHash != nameHash) {
		if (file.nextFile == 0xFFFFFFFF)
			return false;
		file = _fileEntries[file.nextFile];
	}

	return true;
}

bool RMDPArchive::hasDirectory(const std::string &directory) const {
	std::stringstream path(
		std::regex_replace(
			(_pathPrefix ? "d:/data/" : "") + directory,
			std::regex("\\\\"),
			"/"
		)
	);
	std::string item;

	FolderEntry folder = _folderEntries.front();

	uint32_t nameHash;

	while (std::getline(path, item, '/')) {
		nameHash = Common::crc32(Common::toLower(item));

		if (folder.nextLowerFolder == 0xFFFFFFFF)
			return false;

		folder = _folderEntries[folder.nextLowerFolder];

		while (nameHash != folder.nameHash) {
			if (folder.nextNeighbourFolder == 0xFFFFFFFF)
				return false;
			folder = _folderEntries[folder.nextNeighbourFolder];
		}
	}

	return true;
}

void RMDPArchive::loadHeaderV2(Common::ReadStream *bin) {
	// Load header version 2, used by Alan Wake
	_pathPrefix = false;

	uint32_t numFolders = bin->readUint32BE();
	uint32_t numFiles = bin->readUint32BE();

	uint32_t nameSize = bin->readUint32BE();

	std::string pathPrefix = bin->readNullTerminatedString();

	bin->skip(120);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) {
		entry.nameHash = bin->readUint32BE();
		entry.nextNeighbourFolder = bin->readUint32BE();
		entry.prevFolder = bin->readUint32BE();

		bin->skip(4); // Unknown value

		uint32_t nameOffset = bin->readUint32BE();
		if (nameOffset != 0xFFFFFFFF) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(nameOffset), Common::ReadStream::END);
			entry.name = bin->readNullTerminatedString();
			bin->seek(lastPos);
		}

		entry.nextLowerFolder = bin->readUint32BE();
		entry.nextFile = bin->readUint32BE();
	}

	for (auto &entry : _fileEntries) {
		entry.nameHash = bin->readUint32BE();
		entry.nextFile = bin->readUint32BE();
		entry.prevFolder = bin->readUint32BE();
		entry.flags = bin->readUint32BE();

		uint32_t nameOffset = bin->readUint32BE();
		if (nameOffset != 0xFFFFFFFF) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(nameOffset), Common::ReadStream::END);
			entry.name = bin->readNullTerminatedString();
			bin->seek(lastPos);
		}

		entry.offset = bin->readUint64BE();
		entry.size = bin->readUint64BE();

		entry.fileDataHash = bin->readUint32LE();
	}
}

void RMDPArchive::loadHeaderV7(Common::ReadStream *bin) {
	// Load header version 7, used by Nightmare
	_pathPrefix = true;

	uint32_t numFolders = bin->readUint32LE();
	uint32_t numFiles = bin->readUint32LE();

	bin->skip(8); // Always 64 bit 1?

	uint32_t nameSize = bin->readUint32LE();

	bin->skip(8);

	bin->skip(120);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) {
		entry.nameHash = bin->readUint32LE();
		entry.nextNeighbourFolder = bin->readUint32LE();
		entry.prevFolder = bin->readUint32LE();

		bin->skip(4); // Unknown value

		uint32_t nameOffset = bin->readUint32LE();
		if (nameOffset != 0xFFFFFFFF) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(nameOffset), Common::ReadStream::END);
			entry.name = bin->readNullTerminatedString();
			bin->seek(lastPos);
		}

		entry.nextLowerFolder = bin->readUint32LE();
		entry.nextFile = bin->readUint32LE();
	}

	for (auto &entry : _fileEntries) {
		entry.nameHash = bin->readUint32LE();
		entry.nextFile = bin->readUint32LE();
		entry.prevFolder = bin->readUint32LE();
		entry.flags = bin->readUint32LE();

		uint32_t nameOffset = bin->readUint32LE();
		if (nameOffset != 0xFFFFFFFF) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(nameOffset), Common::ReadStream::END);
			entry.name = bin->readNullTerminatedString();
			bin->seek(lastPos);
		}

		if (entry.nameHash != Common::crc32(Common::toLower(entry.name)))
			throw std::runtime_error("Invalid name hash");

		entry.offset = bin->readUint64LE();
		entry.size = bin->readUint64LE();

		entry.fileDataHash = bin->readUint32LE();

		bin->skip(8); // Write Time
	}
}

void RMDPArchive::loadHeaderV8(Common::ReadStream *bin) {
	// Load header version 8, used by Quantum Break
	_pathPrefix = true;

	uint32_t numFolders = bin->readUint32LE();
	uint32_t numFiles = bin->readUint32LE();

	bin->skip(8);

	uint32_t nameSize = bin->readUint32LE();

	std::string pathPrefix = bin->readNullTerminatedString();

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) {
		entry.nameHash = bin->readUint32LE();
		entry.nextNeighbourFolder = bin->readUint32LE();
		entry.prevFolder = bin->readUint32LE();

		bin->skip(4); // Unknown value

		uint32_t nameOffset = bin->readUint32LE();
		if (nameOffset != 0xFFFFFFFF) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(nameOffset), Common::ReadStream::END);
			entry.name = bin->readNullTerminatedString();
			bin->seek(lastPos);
		}

		entry.nextLowerFolder = bin->readUint32LE();
		entry.nextFile = bin->readUint32LE();
	}
}

} // End of namespace AWE
