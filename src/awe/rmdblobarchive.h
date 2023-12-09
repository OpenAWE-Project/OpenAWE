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

#ifndef OPENAWE_RMDBLOBARCHIVE_H
#define OPENAWE_RMDBLOBARCHIVE_H

#include <optional>

#include "src/awe/archive.h"

namespace AWE {

/*!
 * \brief Class for unpacking a rmdblob/rmdtoc archive structure
 *
 * This class helps to unpack the rmdblob/rmdtoc rachive structures whic
 * are built up from one rmdtoc file containing the tables of contents of
 * an archive and one or more rmdblob files containing the raw or compressed
 * data. It utilizes lz4 as compression, so it should be taken into account
 * that if this class is to be sued, it should be compiled with support for
 * lz4.
 *
 * This archive format is currently utilized in the following games from
 * remedy:
 * - Alan Wake 2
 *
 * \remark It is not intended to implement the games utilizing thifs format
 * anytime soon, this class was written for research purposes.
 */
class RMDBlobArchive : Archive {
public:
	/*!
	 * Create a new RMDBlob archive instance and load the data from the
	 * rmdtoc file to fill the appropriate tables
	 * \param rmdtoc A stream from the rmdtoc file
	 * \param path The path of the rmdtoc file to help find the corresponding blob files
	 */
	RMDBlobArchive(Common::ReadStream &rmdtoc, const std::string &path);

	/*!
	 * Get the number of resources contained in this file
	 * \return The number of resources in this file
	 */
	size_t getNumResources() const override;

	/*!
	 * Get the indices of all resources contained in a certain directory
	 * \param directory The directory to get the resources for
	 * \return A list of indices of the files ocntianed in this directory
	 */
	std::vector<size_t> getDirectoryResources(const std::string &directory) override;

	/*!
	 * Get the path of a certain resource given by an index
	 * \param index The index of the resource to get the path for
	 * \return A string containing the path of the resource
	 */
	std::string getResourcePath(size_t index) const override;

	/*!
	 * Get a stream to the resource given by a path. Returns a null pointer
	 * if the resource is not found
	 * \param rid The path of the resource to get
	 * \return A pointer to a valid readstream of the resource or a null pointer
	 * if the reosurce is not found
	 */
	Common::ReadStream *getResource(const std::string &rid) const override;

	/*!
	 * Return if a certain directory exists in the archive
	 * \param rid The path to the files
	 * \return true if the file exists, false otherwise
	 */
	bool hasResource(const std::string &rid) const override;

	/*!
	 * Return if a certain directory exists in the archive
	 * \param rid The path to the directory
	 * \return true if the directory exists, false otherwise
	 */
	bool hasDirectory(const std::string &directory) const override;

private:
	struct CompressionInfo {
		byte blobIndex;
		byte hint;
		unsigned long offset;
		unsigned int compressedSize;
		unsigned int uncompressedSize;
	};

	struct DirectoryEntry {
		int directory{-1};
		unsigned int lowerDirectoriesOffset;
		unsigned int lowerDirectoriesCount;
		unsigned int filesOffset;
		unsigned int filesCount;
		std::string name;
	};

	struct FileEntry {
		std::vector<CompressionInfo> compressionInfo;
		unsigned int decompressedSize;
		unsigned int directory;
		std::string name;
	};

	/*!
	 * Find the directory entry for a certain path which might not exist
	 * \param path The path to find
	 * \return An optional containing the entry if it exists
	 */
	std::optional<DirectoryEntry> findDirectoryEntry(const std::string &path) const;

	/*!
	 * Find the file entry in a directory which might not exist
	 * \param fileName The file to find
	 * \param dir The directory to search the file for
	 * \return An optional containing the entry if it exists
	 */
	std::optional<FileEntry> findFileEntry(
		const std::string &fileName,
		const DirectoryEntry& dir
	) const;

	std::vector<DirectoryEntry> _directories;
	std::vector<FileEntry> _files;
	std::vector<std::unique_ptr<Common::ReadStream>> _blobStreams;

	std::vector<std::unique_ptr<Common::ReadStream>> _streams;
};

} // End of namespace AWE

#endif //OPENAWE_RMDBLOBARCHIVE_H
