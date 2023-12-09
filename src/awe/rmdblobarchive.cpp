//
// Created by patrick on 03.11.23.
//

#include <filesystem>
#include <numeric>

#include "src/common/exception.h"
#include "src/common/memwritestream.h"
#include "src/common/memreadstream.h"
#include "src/common/readfile.h"
#include "src/common/writefile.h"
#include "src/common/lz4.h"
#include "src/common/strutil.h"

#include "src/awe/rmdblobarchive.h"

static const uint32_t kRTOC = MKTAG('R', 'T', 'O', 'C');

namespace AWE {

RMDBlobArchive::RMDBlobArchive(Common::ReadStream &rmdtoc, const std::string &path) {
	const auto magic = rmdtoc.readUint32LE();
	if (magic != kRTOC)
		throw CreateException("Invalid RMDToc magic id, expected 0x{:X}, found 0x{:X}", kRTOC, magic);

	const auto version = rmdtoc.readUint32LE();
	if (version != 2)
		throw CreateException("Invalid version, only 2 is supported, found {}", version);

	const auto blobTableOffset = rmdtoc.readUint32LE();
	const auto blobTableSize   = rmdtoc.readUint32LE();

	const auto blobFilesOffset = rmdtoc.readUint32LE();
	const auto blobFilesCount  = rmdtoc.readUint32LE();

	const auto folderTableOffset = rmdtoc.readUint32LE();
	const auto folderTableCount  = rmdtoc.readUint32LE();

	const auto fileTableOffset = rmdtoc.readUint32LE();
	const auto fileTableCount   = rmdtoc.readUint32LE();

	const auto nameTableOffset = rmdtoc.readUint32LE();
	const auto nameTableSize   = rmdtoc.readUint32LE();

	rmdtoc.skip(32);

	const auto compressionTableOffset = rmdtoc.readUint32LE();
	const auto compressionTableSize   = rmdtoc.readUint32LE();

	Common::DynamicMemoryWriteStream tocData(false);

	rmdtoc.seek(blobTableOffset);
	for (unsigned int i = 0; i < blobTableSize / 16; ++i) {
		const auto compressionHint = rmdtoc.readByte();
		rmdtoc.skip(2);
		const auto dataOffset      = rmdtoc.readUint32LE();
		rmdtoc.skip(1);
		const auto uncompressedSize = rmdtoc.readUint32LE();
		const auto compressedSize   = rmdtoc.readUint32LE();

		assert(compressionHint == 16);

		const auto compressedData = new byte[compressedSize];

		const auto lastPos = rmdtoc.pos();
		rmdtoc.seek(dataOffset);
		rmdtoc.read(compressedData, compressedSize);

		std::unique_ptr<Common::ReadStream> chunk(Common::decompressLZ4(
				compressedData,
				compressedSize,
				uncompressedSize
		));
		tocData.writeStream(chunk.get());

		rmdtoc.seek(lastPos);
	}

	Common::MemoryReadStream toc(tocData.getData(), tocData.getLength(), true);

	toc.seek(nameTableOffset);
	std::unique_ptr<Common::ReadStream> nameTable(toc.readStream(nameTableSize));

	toc.seek(compressionTableOffset);
	std::unique_ptr<Common::ReadStream> compressionTable(toc.readStream(compressionTableSize));

	toc.seek(blobFilesOffset);
	for (unsigned int i = 0; i < blobFilesCount; ++i) {
		const auto nameOffset = toc.readUint32LE();
		const auto nameSize   = toc.readUint32LE();
		toc.skip(8);

		nameTable->seek(nameOffset);
		const auto blobPath = nameTable->readFixedSizeString(nameSize);

		_blobStreams.emplace_back(
			std::make_unique<Common::ReadFile>(
				fmt::format("{}/{}", path, std::filesystem::path(blobPath).filename().string())
			)
		);
	}

	_directories.resize(folderTableCount);
	_files.resize(fileTableCount);

	toc.seek(folderTableOffset);
	for (unsigned int i = 0; i < folderTableCount; ++i) {
		const auto treeLevel   = toc.readUint32LE();
		const auto folderIndex = toc.readUint32LE();
		const auto folderCount = toc.readUint32LE();
		const auto fileIndex   = toc.readUint32LE();
		const auto fileCount   = toc.readUint32LE();
		const auto nameOffset  = toc.readUint32LE();
		const auto nameSize    = toc.readUint32LE();

		nameTable->seek(nameOffset);
		const auto name = nameTable->readFixedSizeString(nameSize);

		auto &directory = _directories[i];

		if (i != 0)
			directory.name = name;

		directory.lowerDirectoriesOffset = folderIndex;
		directory.lowerDirectoriesCount = folderCount;
		directory.filesOffset = fileIndex;
		directory.filesCount = fileCount;

		for (size_t j = 0; j < folderCount; ++j) {
			_directories[j + folderIndex].directory = i;
		}
		for (size_t j = 0; j < fileCount; ++j) {
			_files[j + fileIndex].directory = i;
		}

		fmt::print("{}\n", name);
	}

	for (unsigned int i = 0; i < fileTableCount; ++i) {
		const auto compressionInfoOffset  = toc.readUint32LE();
		const auto compressionInfoSize    = toc.readUint32LE();
		const auto fileId                 = toc.readUint32LE();
		const auto nameOffset             = toc.readUint32LE();
		const auto nameSize               = toc.readUint32LE();
		const auto decompressedSize       = toc.readUint32LE();
		const auto dmkpTableOffset        = toc.readUint32LE();
		const auto dmkpTableSize          = toc.readUint32LE();

		auto &file = _files[i];

		nameTable->seek(nameOffset);
		file.name = nameTable->readFixedSizeString(nameSize);
		file.decompressedSize = decompressedSize;

		const auto lastPos = toc.pos();
		compressionTable->seek(compressionInfoOffset);

		file.compressionInfo.resize(compressionInfoSize / 16);
		for (auto &compressionInfo : file.compressionInfo) {
			compressionInfo.hint = compressionTable->readByte();
			compressionInfo.blobIndex= compressionTable->readByte();
			compressionTable->skip(1);

			compressionInfo.offset = compressionTable->readUint32LE();
			const auto extendedOffset = compressionTable->readByte();
			compressionInfo.offset += extendedOffset * 0x100000000;

			compressionInfo.uncompressedSize = compressionTable->readUint32LE();
			compressionInfo.compressedSize = compressionTable->readUint32LE();

			//fmt::print("{} {} {} {}\n", compressionInfo.blobIndex, compressionInfo.hint, compressionInfo.offset, compressionInfo.compressedSize);

			assert(compressionInfo.blobIndex<blobFilesCount);
		}

		toc.seek(lastPos);
 	}
}

size_t RMDBlobArchive::getNumResources() const {
	return _files.size();
}

std::vector<size_t> RMDBlobArchive::getDirectoryResources(const std::string &directory) {
	const auto directoryEntry = findDirectoryEntry(directory);
	if (!directoryEntry)
		return {};

	std::vector<size_t> fileIndices(directoryEntry->filesCount);
	std::iota(fileIndices.begin(), fileIndices.end(), directoryEntry->filesOffset);
	return fileIndices;
}

std::string RMDBlobArchive::getResourcePath(size_t index) const {
	FileEntry file = _files.at(index);

	std::string path = file.name;

	DirectoryEntry currentDir = _directories[file.directory];
	while (currentDir.directory != -1) {
		path = fmt::format("{}/{}", currentDir.name, path);
		currentDir = _directories[currentDir.directory];
	}

	return path;
}

Common::ReadStream *RMDBlobArchive::getResource(const std::string &rid) const {
	const auto directoryEntry = findDirectoryEntry(std::filesystem::path(rid).parent_path());
	if (!directoryEntry)
		return nullptr;

	const auto entry = findFileEntry(std::filesystem::path(rid).filename(), *directoryEntry);
	if (!entry)
		return nullptr;

	auto writeStream = Common::DynamicMemoryWriteStream(false);
	for (const auto &info: entry->compressionInfo) {
		auto &blobStream = _blobStreams.at(info.blobIndex);

		blobStream->seek(info.offset);

		switch (info.hint) {
			case 0: {
				std::vector<byte> data(info.uncompressedSize);
				blobStream->read(data.data(), data.size());

				writeStream.write(data.data(), data.size());
				break;
			}

			case 16: {
				std::vector<byte> data(info.compressedSize);
				blobStream->read(data.data(), data.size());

				std::unique_ptr<Common::ReadStream> decompressed(
						Common::decompressLZ4(
								data.data(),
								data.size(),
								info.uncompressedSize
						)
				);
				writeStream.writeStream(decompressed.get());

				break;
			}

			default:
				throw CreateException("Invalid compression hint {}", info.hint);
		}
	}

	return new Common::MemoryReadStream(writeStream.getData(), writeStream.getLength());
}

bool RMDBlobArchive::hasResource(const std::string &rid) const {
	const auto directoryEntry = findDirectoryEntry(std::filesystem::path(rid).parent_path());
	if (!directoryEntry)
		return false;

	const auto entry = findFileEntry(std::filesystem::path(rid).filename(), *directoryEntry);
	return entry.has_value();
}

bool RMDBlobArchive::hasDirectory(const std::string &directory) const {
	const auto directoryEntry = findDirectoryEntry(directory);
	return directoryEntry.has_value();
}

std::optional<RMDBlobArchive::DirectoryEntry> RMDBlobArchive::findDirectoryEntry(const std::string &path) const {
	const auto filePath = Common::split(path, std::regex("/"));
	const auto splitPath = std::vector<std::string>(filePath.begin(), filePath.end());

	DirectoryEntry currentDir = _directories.front();
	for (const auto &part: splitPath) {
		bool found = false;
		for (unsigned int i = 0; i < currentDir.lowerDirectoriesCount; ++i) {
			DirectoryEntry potentialDir = _directories[i + currentDir.lowerDirectoriesOffset];

			if (potentialDir.name == part) {
				currentDir = potentialDir;
				found = true;
				break;
			}
		}

		if (!found)
			return {};
	}

	return currentDir;
}

std::optional<RMDBlobArchive::FileEntry> RMDBlobArchive::findFileEntry(
		const std::string &fileName,
		const RMDBlobArchive::DirectoryEntry& dir
) const {
	for (unsigned int i = 0; i < dir.filesCount; ++i) {
		FileEntry currentFile = _files[i + dir.filesOffset];

		if (currentFile.name == fileName)
			return currentFile;
	}

	return {};
}

} // End of namespace AWE