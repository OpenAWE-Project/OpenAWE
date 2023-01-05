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

#include <optional>
#include <fmt/format.h>
#include <zlib.h>
#include <assert.h>

#include "src/common/strutil.h"
#include "src/common/memreadstream.h"
#include "src/common/exception.h"
#include "src/common/crc32.h"

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

std::optional<RMDPArchive::FolderEntry> RMDPArchive::findDirectory(std::stringstream &path) const{
	std::string item;

	FolderEntry folder = _folderEntries.front();

	uint32_t nameHash = 0;

	while (!path.eof() && std::getline(path, item, '/')) {
		nameHash = Common::crc32(Common::toLower(item));

		if (folder.nextLowerFolder == NO_ENTRY)
			return {};

		folder = _folderEntries[folder.nextLowerFolder];

		while (nameHash != folder.nameHash) {
			if (folder.nextNeighbourFolder == NO_ENTRY)
				return {};
			folder = _folderEntries[folder.nextNeighbourFolder];
		}
	}

	return folder;
}

std::stringstream RMDPArchive::getNormalizedPath(const std::string &path) const{
	std::string lower = Common::toLower(path);
	int64_t pos = std::string::npos;
	while ((pos = lower.find("\\")) != std::string::npos) lower.replace(pos, 1, "/", 1);
	return std::stringstream((_pathPrefix ? "d:/data/" : "") + lower);
}

std::optional<RMDPArchive::FileEntry> RMDPArchive::findFile(const FolderEntry &folder, const uint32_t nameHash) const {
	if (folder.nextFile == NO_ENTRY)
		return {};
	FileEntry file = _fileEntries[folder.nextFile];

	while (file.nameHash != nameHash) {
		if (file.nextFile == NO_ENTRY)
			return {};
		file = _fileEntries[file.nextFile];
	}
	return file;
}

std::vector<size_t> RMDPArchive::getDirectoryResources(const std::string &directory) {
	std::stringstream path = this->getNormalizedPath(directory);
	auto maybeFolder = this->findDirectory(path);
	if (!maybeFolder.has_value()) return {};
	FolderEntry folder = maybeFolder.value();

	if (folder.nextFile == NO_ENTRY)
		return {};

	std::vector<size_t> indices;
	FileEntry file = _fileEntries[folder.nextFile];
	indices.emplace_back(folder.nextFile);

	while (file.nextFile != NO_ENTRY) {
		indices.emplace_back(file.nextFile);
		file = _fileEntries[file.nextFile];
	}

	return indices;
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
	if (fileEntry.prevFolder == NO_ENTRY)
		return path;

	// Get the first folder entry
	auto folderEntry = _folderEntries[fileEntry.prevFolder];
	path = folderEntry.name + "/" + path;

	// Iterate over all preceeding folder entries
	while (folderEntry.prevFolder != NO_ENTRY) {
		folderEntry = _folderEntries[folderEntry.prevFolder];
		if (!folderEntry.name.empty())
			path = folderEntry.name + "/" + path;
	}

	if (path.find("d:/data/") == 0) path.replace(0, 8, "");

	return path;
}

Common::ReadStream *RMDPArchive::getResource(const std::string &rid) const {
	std::stringstream path = this->getNormalizedPath(rid);

	auto maybeFolder = findDirectory(path);
	if (!maybeFolder.has_value()) return nullptr;
	FolderEntry folder = maybeFolder.value();

	if (folder.nextFile == NO_ENTRY)
		return nullptr;

	auto maybeFile = this->findFile(folder, folder.nameHash);
	if (!maybeFile.has_value()) return nullptr;
	FileEntry file = maybeFile.value();

	byte *data = new byte[file.size];
	_rmdp->seek(file.offset);
	_rmdp->read(data, file.size);

	assert(crc32(0L, data, file.size) == file.fileDataHash);

	return new Common::MemoryReadStream(data, file.size, true);
}

bool RMDPArchive::hasResource(const std::string &rid) const {
	std::stringstream path = this->getNormalizedPath(rid);

	auto maybeFolder = this->findDirectory(path);
	if (!maybeFolder.has_value()) return false;
	FolderEntry folder = maybeFolder.value();

	auto maybeFile = this->findFile(folder, folder.nameHash);
	return maybeFile.has_value();
}

bool RMDPArchive::hasDirectory(const std::string &directory) const {
	std::stringstream path = this->getNormalizedPath(directory);
	auto maybeFolder = this->findDirectory(path);
	return maybeFolder.has_value();
}

std::string RMDPArchive::readEntryName(Common::ReadStream *bin, uint32_t offset, uint32_t nameSize) {
	std::string result;
	if (offset != NO_ENTRY) {
			size_t lastPos = bin->pos();
			bin->seek(-static_cast<int>(nameSize) + static_cast<int>(offset), Common::ReadStream::END);
			result = bin->readNullTerminatedString();
			bin->seek(lastPos);
	} else {
		result = "";
	}
	return result;
}

RMDPArchive::FolderEntry RMDPArchive::readFolder(Common::ReadStream *bin, uint32_t (Common::ReadStream::*readUint32) (), uint32_t nameSize) {
	// Since folder struct remains the same through v2/7/8 headers
	FolderEntry entry;
	entry.nameHash = (bin->*readUint32)();
	entry.nextNeighbourFolder = (bin->*readUint32)();
	entry.prevFolder = (bin->*readUint32)();

	bin->skip(4); // Unknown value

	uint32_t nameOffset = (bin->*readUint32)();
	entry.name = this->readEntryName(bin, nameOffset, nameSize);

	entry.nextLowerFolder = (bin->*readUint32)();
	entry.nextFile = (bin->*readUint32)();
	return entry;
}

RMDPArchive::FileEntry RMDPArchive::readFile(Common::ReadStream *bin, uint32_t (Common::ReadStream::*readUint32) (), uint64_t (Common::ReadStream::*readUint64) (), uint32_t nameSize) {
	FileEntry entry;
	entry.nameHash = (bin->*readUint32)();
	entry.nextFile = (bin->*readUint32)();
	entry.prevFolder = (bin->*readUint32)();
	entry.flags = (bin->*readUint32)();

	uint32_t nameOffset = (bin->*readUint32)();
	entry.name = this->readEntryName(bin, nameOffset, nameSize);

	if (entry.nameHash != Common::crc32(Common::toLower(entry.name)))
			throw std::runtime_error("Invalid name hash");

	entry.offset = (bin->*readUint64)();
	entry.size = (bin->*readUint64)();

	entry.fileDataHash = (bin->*readUint32)();
	return entry;
}

void RMDPArchive::loadHeaderV2(Common::ReadStream *bin) {
	// Load header version 2, used by Alan Wake
	_pathPrefix = false;
	// Define endianness once to avoid typos
	uint32_t (Common::ReadStream::*readUint32) () = &Common::ReadStream::readUint32BE;
	uint64_t (Common::ReadStream::*readUint64) () = &Common::ReadStream::readUint64BE;

	uint32_t numFolders = (bin->*readUint32)();
	uint32_t numFiles = (bin->*readUint32)();

	uint32_t nameSize = (bin->*readUint32)();

	std::string pathPrefix = bin->readNullTerminatedString();

	bin->skip(120);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries)
		entry = this->readFolder(bin, readUint32, nameSize);

	for (auto &entry : _fileEntries) 
		entry = this->readFile(bin, readUint32, readUint64, nameSize);
}

void RMDPArchive::loadHeaderV7(Common::ReadStream *bin) {
	// Load header version 7, used by Nightmare
	_pathPrefix = true;
	// Define endianness once to avoid typos
	uint32_t (Common::ReadStream::*readUint32) () = &Common::ReadStream::readUint32LE;
	uint64_t (Common::ReadStream::*readUint64) () = &Common::ReadStream::readUint64LE;

	uint32_t numFolders = (bin->*readUint32)();
	uint32_t numFiles = (bin->*readUint32)();

	bin->skip(8); // Always 64 bit 1?

	uint32_t nameSize = (bin->*readUint32)();

	bin->skip(128);

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries) 
		entry = this->readFolder(bin, readUint32, nameSize);

	for (auto &entry : _fileEntries) {
		entry = this->readFile(bin, readUint32, readUint64, nameSize);
		bin->skip(8); // Write Time
	}
}

void RMDPArchive::loadHeaderV8(Common::ReadStream *bin) {
	// Load header version 8, used by Quantum Break
	_pathPrefix = true;
	// Define endianness once to avoid typos
	uint32_t (Common::ReadStream::*readUint32) () = &Common::ReadStream::readUint32LE;
	uint64_t (Common::ReadStream::*readUint64) () = &Common::ReadStream::readUint64LE;

	uint32_t numFolders = (bin->*readUint32)();
	uint32_t numFiles = (bin->*readUint32)();

	bin->skip(8);

	uint32_t nameSize = (bin->*readUint32)();

	std::string pathPrefix = bin->readNullTerminatedString();

	_folderEntries.resize(numFolders);
	_fileEntries.resize(numFiles);

	for (auto &entry : _folderEntries)
		entry = this->readFolder(bin, readUint32, nameSize);
	
	// TO-DO: Fill in _fileEntries array
}

} // End of namespace AWE
