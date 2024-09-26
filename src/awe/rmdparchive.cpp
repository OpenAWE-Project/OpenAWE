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

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <optional>
#include <regex>
#include <vector>
#include <mutex>

#include <fmt/format.h>

#include "src/common/endianreadstream.h"
#include "src/common/strutil.h"
#include "src/common/memreadstream.h"
#include "src/common/exception.h"
#include "src/common/crc32.h"

#include "src/awe/path.h"
#include "src/awe/rmdparchive.h"

enum kArchiveVersion {
	kVersionAlanWake = 2,
	kVersionNightmare = 7,
	kVersionQuantumBreak = 8,
	kVersionControl = 9
};

namespace AWE {

RMDPArchive::RMDPArchive(Common::ReadStream *bin, Common::ReadStream *rmdp) : _rmdp(rmdp) {
	_littleEndian = bin->readByte() == 0;
	Common::EndianReadStream end = Common::EndianReadStream(bin, !_littleEndian);

	_version = end.readUint32();

	switch (_version) {
		case kVersionAlanWake:
			loadHeaderV2(bin, end);
			break;
		case kVersionNightmare:
			loadHeaderV7(bin, end);
			break;
		case kVersionQuantumBreak:
			loadHeaderV8(bin, end);
			break;
		case kVersionControl:
			loadHeaderV8(bin, end);
			break;
		default:
			throw CreateException("Unknown RMDP Archive version {}", _version);
	}

	delete bin;
}

size_t RMDPArchive::getNumResources() const {
	return _fileEntries.size();
}

std::vector<uint32_t> RMDPArchive::getPathHashes(const std::string &path) const {
	std::vector<std::string> pathNames = Common::split(path, std::regex("/"));
	std::vector<uint32_t> pathHashes;
	pathHashes.resize(pathNames.size());

	for (uint64_t i = 0; i < pathNames.size(); ++i)
		pathHashes[i] = Common::crc32(pathNames[i]);

	return pathHashes;
}

std::optional<RMDPArchive::FolderEntry> RMDPArchive::findDirectory(const std::vector<uint32_t> &pathHashes) const {
	FolderEntry folder = _folderEntries.front();
	if (pathHashes.empty()) return folder;

	for (uint32_t nameHash : pathHashes) {
		if (folder.nextLowerFolder == -1)
			return {};

		folder = _folderEntries[folder.nextLowerFolder];

		while (nameHash != folder.nameHash) {
			if (folder.nextNeighbourFolder == -1)
				return {};
			folder = _folderEntries[folder.nextNeighbourFolder];
		}
	}

	return folder;
}

std::optional<RMDPArchive::FileEntry> RMDPArchive::findFile(const FolderEntry &folder, const uint32_t nameHash) const {
	if (folder.nextFile == -1)
		return {};
	FileEntry file = _fileEntries[folder.nextFile];

	while (file.nameHash != nameHash) {
		if (file.nextFile == -1)
			return {};
		file = _fileEntries[file.nextFile];
	}
	return file;
}

std::vector<size_t> RMDPArchive::getDirectoryResources(const std::string &directory) {
	auto pathHashes = getPathHashes(directory);

	auto maybeFolder = findDirectory(pathHashes);
	if (!maybeFolder) return {};
	FolderEntry folder = *maybeFolder;

	if (folder.nextFile == -1)
		return {};

	std::vector<size_t> indices;
	FileEntry file = _fileEntries[folder.nextFile];
	indices.emplace_back(folder.nextFile);

	while (file.nextFile != -1) {
		indices.emplace_back(file.nextFile);
		file = _fileEntries[file.nextFile];
	}

	return indices;
}

std::string RMDPArchive::getResourcePath(size_t index) const {
	// Check if the index is less than the number of resources
	if (index >= getNumResources())
		throw CreateException(
				"Index {} exceeds number of file entries {}",
				index,
				getNumResources()
		);


	const auto &fileEntry = _fileEntries[index];
	std::string path = fileEntry.name;

	// Check if the file is in the root directory
	if (fileEntry.prevFolder == 0)
		return path;

	// Get the first folder entry
	auto folderEntry = _folderEntries[fileEntry.prevFolder];
	path = folderEntry.name + "/" + path;

	// Iterate over all preceeding folder entries
	while (folderEntry.prevFolder != -1) {
		folderEntry = _folderEntries[folderEntry.prevFolder];
		if (!folderEntry.name.empty())
			path = folderEntry.name + "/" + path;
	}

	return path;
}

Common::ReadStream *RMDPArchive::getResource(const std::string &rid) const {
	std::lock_guard<std::mutex> g(_readMutex);

	// Extract and separate file name from the rest of the path
	auto pathHashes = getPathHashes(rid);
	uint32_t fileHash = pathHashes.back();
	pathHashes.pop_back();

	const auto maybeFolder = findDirectory(pathHashes);
	if (!maybeFolder) return nullptr;
	FolderEntry folder = *maybeFolder;

	if (folder.nextFile == -1)
		return nullptr;

	const auto maybeFile = findFile(folder, fileHash);
	if (!maybeFile) return nullptr;
	const FileEntry &file = *maybeFile;

	byte *data = new byte[file.size];
	_rmdp->seek(file.offset);
	_rmdp->read(data, file.size);

	assert(Common::crc32(data, file.size) == file.fileDataHash);

	return new Common::MemoryReadStream(data, file.size, true);
}

bool RMDPArchive::hasResource(const std::string &rid) const {
	// Extract and separate file name from the rest of the path
	auto pathHashes = getPathHashes(rid);
	uint32_t fileHash = pathHashes.back();
	pathHashes.pop_back();

	auto maybeFolder = findDirectory(pathHashes);
	if (!maybeFolder) return false;
	FolderEntry folder = *maybeFolder;

	auto maybeFile = findFile(folder, fileHash);
	return maybeFile.has_value();
}

bool RMDPArchive::hasDirectory(const std::string &directory) const {
	const auto pathHashes = getPathHashes(directory);
	const auto maybeFolder = findDirectory(pathHashes);
	return maybeFolder.has_value();
}

std::string RMDPArchive::readEntryName(Common::ReadStream *bin, int64_t offset, uint32_t nameSize) {
	std::string result;
	if (offset != -1) {
		size_t lastPos = bin->pos();
		bin->seek(-static_cast<int>(nameSize) + static_cast<int>(offset), Common::ReadStream::END);
		result = bin->readNullTerminatedString();
		bin->seek(lastPos);
	} else {
		result = "";
	}
	return result;
}

void RMDPArchive::loadHeaderV2(Common::ReadStream *bin, Common::EndianReadStream &end) {
	// Load header version 2, used by both Alan Wake and Alan Wake Remastered

	const uint32_t numFolders = end.readUint32();
	const uint32_t numFiles = end.readUint32();

	const uint32_t nameSize = end.readUint32();

	const std::string pathPrefix = bin->readNullTerminatedString();

	bin->skip(120);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	// Now let's figure out whether we're looking at a remaster or not
	const size_t AWFileSizeBytes = 40;
	const size_t AWRFileSizeBytes = 64;
	const size_t AWFolderSizeBytes = 28;
	const size_t AWRFolderSizeBytes = 56;
	const size_t AWStructsSize = AWFileSizeBytes * numFiles + AWFolderSizeBytes * numFolders;
	const size_t AWRStructsSize = AWRFileSizeBytes * numFiles + AWRFolderSizeBytes * numFolders;

	// Move to the start of name space to find file + folder size
	const size_t structStartAddress = bin->pos();
	bin->seek(-static_cast<int>(nameSize), Common::ReadStream::END);
	const size_t fileAndFolderSize = bin->pos() - structStartAddress;

	const bool isAlanWake = AWStructsSize == fileAndFolderSize;
	const bool isAlanWakeRemastered = AWRStructsSize <= fileAndFolderSize;

	bin->seek(structStartAddress);

	if (isAlanWake) {
		for (auto &entry : _folderEntries) {
			entry.nameHash = end.readUint32();
			entry.nextNeighbourFolder = end.readSint32();
			entry.prevFolder = end.readSint32();

			bin->skip(4); // Unknown value

			const int32_t nameOffset = end.readSint32();
			entry.name = readEntryName(bin, nameOffset, nameSize);

			const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
			if (entry.nameHash != testHash)
				throw CreateException("Invalid name hash: expected {}, but found {}",
										entry.nameHash,
										testHash);

			entry.nextLowerFolder = end.readSint32();
			entry.nextFile = end.readSint32();
		}

		for (auto &entry : _fileEntries) {
			entry.nameHash = end.readUint32();
			entry.nextFile = end.readSint32();
			entry.prevFolder = end.readSint32();
			entry.flags = end.readUint32();

			const int32_t nameOffset = end.readSint32();
			entry.name = readEntryName(bin, nameOffset, nameSize);

			const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
			if (entry.nameHash != testHash)
				throw CreateException("Invalid name hash: expected {}, but found {}",
										entry.nameHash,
										testHash);

			entry.offset = end.readUint64();
			entry.size = end.readUint64();

			// For some reason, Alan Wakes file data hashes are saved as little endian
			entry.fileDataHash = bin->readUint32LE();
		}
	} else if (isAlanWakeRemastered) {
		// Since name space may not be the end of a file,
		// We need to readjust the value
		const uint32_t adjustedNameSize = nameSize + (fileAndFolderSize - AWRStructsSize);

		for (auto &entry : _folderEntries) {
			entry.nameHash = end.readUint32();
			bin->skip(4); // Always 0
			entry.nextNeighbourFolder = end.readSint64();
			entry.prevFolder = end.readSint64();

			bin->skip(8); // Unknown value

			const int64_t nameOffset = end.readSint64();
			entry.name = readEntryName(bin, nameOffset, adjustedNameSize);

			entry.nextLowerFolder = end.readSint64();
			entry.nextFile = end.readSint64();
		}

		for (auto &entry : _fileEntries) {
			entry.nameHash = end.readUint32();
			bin->skip(4); // Always 0
			entry.nextFile = end.readSint64();
			entry.prevFolder = end.readSint64();
			entry.flags = end.readUint64();

			const int64_t nameOffset = end.readSint64();
			entry.name = readEntryName(bin, nameOffset, adjustedNameSize);

			const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
			if (entry.nameHash != testHash)
				throw CreateException("Invalid name hash: expected {}, but found {}",
										entry.nameHash,
										testHash);

			entry.offset = end.readUint64();
			entry.size = end.readUint64();

			entry.fileDataHash = bin->readUint32LE();
			bin->skip(4); // Always 0
		}
	} else 
		CreateException("Abnormal v2 header: found structures with size {}, while expected either {} or {}",
						fileAndFolderSize, AWStructsSize, AWRStructsSize);
}

void RMDPArchive::loadHeaderV7(Common::ReadStream *bin, Common::EndianReadStream &end) {
	// Load header version 7, used by Nightmare

	const uint32_t numFolders = end.readUint32();
	const uint32_t numFiles = end.readUint32();

	bin->skip(8); // Always 64 bit 1?

	const uint32_t nameSize = end.readUint32();

	bin->skip(128);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) {
		entry.nameHash = end.readUint32();
		entry.nextNeighbourFolder = end.readSint32();
		entry.prevFolder = end.readSint32();

		bin->skip(4); // Unknown value

		const int32_t nameOffset = end.readSint32();
		entry.name = readEntryName(bin, nameOffset, nameSize);

		entry.nextLowerFolder = end.readSint32();
		entry.nextFile = end.readSint32();
	}

	for (auto &entry : _fileEntries) {
		entry.nameHash = end.readUint32();
		entry.nextFile = end.readSint32();
		entry.prevFolder = end.readSint32();
		entry.flags = end.readUint32();

		const int32_t nameOffset = end.readSint32();
		entry.name = readEntryName(bin, nameOffset, nameSize);

		const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
		if (entry.nameHash != testHash)
			throw CreateException("Invalid name hash: expected {}, but found {}",
									entry.nameHash,
									testHash);

		entry.offset = end.readUint64();
		entry.size = end.readUint64();

		entry.fileDataHash = end.readUint32();
		bin->skip(8); // Write Time
	}
}

void RMDPArchive::loadHeaderV8(Common::ReadStream *bin, Common::EndianReadStream &end) {
	// Load header version 8, used by Quantum Break

	const uint32_t numFolders = end.readUint32();
	const uint32_t numFiles = end.readUint32();

	bin->skip(8);

	const uint32_t nameSize = end.readUint32();
	const std::string pathPrefix = bin->readFixedSizeString(8, true);

	bin->skip(120);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) {
		entry.nameHash = end.readUint32();
		entry.nextNeighbourFolder = end.readSint64();
		entry.prevFolder = end.readSint64();

		bin->skip(4); // Unknown value

		const int64_t nameOffset = end.readSint64();
		entry.name = readEntryName(bin, nameOffset, nameSize);

		const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
		if (entry.nameHash != testHash)
			throw CreateException("Invalid name hash: expected {}, but found {}",
								  entry.nameHash,
								  testHash);

		entry.nextLowerFolder = end.readSint64();
		entry.nextFile = end.readSint64();
	}

	for (auto &entry: _fileEntries) {
		entry.nameHash = end.readUint32();
		entry.nextFile = end.readSint64();
		entry.prevFolder = end.readSint64();
		entry.flags = end.readUint32();

		const int64_t nameOffset = end.readSint64();
		entry.name = readEntryName(bin, nameOffset, nameSize);

		const uint32_t testHash = Common::crc32(Common::toLower(entry.name));
		if (entry.nameHash != testHash)
			throw CreateException("Invalid name hash: expected {}, but found {}",
								  entry.nameHash,
								  testHash);

		entry.offset = end.readUint64();
		entry.size = end.readUint64();

		entry.fileDataHash = end.readUint32();
		bin->skip(8); // Write Time
	}
}

} // End of namespace AWE
