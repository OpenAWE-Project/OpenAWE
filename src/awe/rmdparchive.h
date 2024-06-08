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

#ifndef AWE_RMDPARCHIVE_H
#define AWE_RMDPARCHIVE_H

#include <vector>
#include <memory>
#include <optional>
#include <mutex>

#include "src/common/endianreadstream.h"

#include "src/awe/archive.h"

namespace AWE {

/*!
 * \brief Loader for bin/rmdp archive structures
 *
 * This class serves as a loader for bin/rmdp archive
 * structures, which contain the main data of both Alan
 * Wake games. The bin file contains the metadata tables
 * with a file table, a folder table and a name table. The
 * rmdp file contains the raw data without any compression
 * used. The first byte is common to all files and denotes
 * the endianness used by the containing tables, and the
 * second to fifth byte denotes a version of the header
 * information.
 */
class RMDPArchive : public Archive {
public:
	/*!
	 * Loads a new bin/rmdp archive structure from the streams given
	 * in bin and rmdp. The rmdp pointer will later be stored in the
	 * class to allow seamless access to the resources inside the archive.
	 *
	 * \param bin the bin file data containing the metadata
	 * \param rmdp the rmdp file containing the raw data
	 */
	RMDPArchive(Common::ReadStream *bin, Common::ReadStream *rmdp);

	/*!
	 * Get the number of resources contained inside this archive by simply
	 * returning the size of the _fileEntries vector
	 *
	 * \return the number of resources contained inside this archive
	 */
	size_t getNumResources() const override;

	/*!
	 * Get a list of indices to all files in the specific directory. If the list is empty or the directory does not
	 * exist, it will return an empty list
	 *
	 * \param directory The directory from which to get the files
	 * \return A vector containing all indices
	 */
	std::vector<size_t> getDirectoryResources(const std::string &directory) override;

	/*!
	 * Get the path for a resource including its filename at a specific index. Throws
	 * an error if the index is equal or exceeds the number of resources in this archive
	 *
	 * \param index The index of a resource to get the path for
	 * \return The path of the resource at the specified index
	 */
	std::string getResourcePath(size_t index) const override;

	/*!
	 * Loads a file from the bin/rmdp archive and get the metadata by
	 * following the indexes from folder and file entries and create
	 * a memory stream from it.
	 *
	 * \param rid the virtual path to the resource
	 * \return the newly created stream for the specified resource
	 */
	[[nodiscard]] Common::ReadStream *getResource(const std::string &rid) const override;

	/*!
	 * Check if the file specified by rid exists inside this archive
	 * by iterating through the tree
	 *
	 * \param rid the file to check
	 * \return if the file given by rid exists inside this archive
	 */
	[[nodiscard]] bool hasResource(const std::string &rid) const override;

	/*!
	 * Check if a certain directory exists
	 *
	 * \param directory The directory to test for
	 * \return If this directory exists
	 */
	bool hasDirectory(const std::string &directory) const override;

private:
	/*!
	 * Load header version 2 used by Alan Wake
	 *
	 * \param bin the stream from which to load the header
	 * \param end stream wrapper with fixed endianness
	 */
	void loadHeaderV2(Common::ReadStream *bin, Common::EndianReadStream &end);

	/*!
	 * Load header version 7 used by Alan Wakes American Nightmare
	 *
	 * \param bin the stream from which to load the header
	 * \param end stream wrapper with fixed endianness
	 */
	void loadHeaderV7(Common::ReadStream *bin, Common::EndianReadStream &end);

	/*!
	 * Load header version 8 used by Quantum Break
	 *
	 * \param bin the stream from which to load the header
	 * \param end stream wrapper with fixed endianness
	 */
	void loadHeaderV8(Common::ReadStream *bin, Common::EndianReadStream &end);

	/*!
	 * An utility function to read a file/folder name from an offset
	 * with known name size.
	 *
	 * \param bin the stream from which to load the name
	 */
	std::string readEntryName(Common::ReadStream *bin, int64_t offset, uint32_t nameSize);

	/*!
	 * Break down a given path into an array of CRC32 hashes
	 * for later processing.
	 */
	std::vector<uint32_t> getPathHashes(const std::string &path) const;
	
	/*!
	 * Structure describing a folder entry of the loaded bin/rmdp archive
	 * structure. Has name hash for faster comparison and indices to the next
	 * lower folder, the next neighbour folder and the next folder inside.
	 */
	struct FolderEntry {
		std::string name;
		uint32_t nameHash;
		int64_t nextLowerFolder;
		int64_t nextNeighbourFolder;
		int64_t nextFile;
		int64_t prevFolder;
	};

	/*!
	 * Structure describing a file entry of the loaded bin/rmdp archive
	 * structure. Has a name hash for faster comparison and an index
	 * to the next file in the current directory. It contains also
	 * offset and size to determine the files inside the rmdp file.
	 */
	struct FileEntry {
		std::string name;
		uint32_t nameHash;
		uint32_t fileDataHash;
		int64_t nextFile;
		int64_t prevFolder;
		uint64_t flags;
		uint64_t offset, size;
	};

	/*!
	 * A helper function that navigates through _folderEntries under
	 * path given as an array of hashes.
	 *
	 * \return folder entry, if it exists under giver path
	 */
	std::optional<FolderEntry> findDirectory(const std::vector<uint32_t> &pathHashes) const;

	/*!
	 * A helper functon that find a file in a given folder by
	 * its name hash value.
	 */
	std::optional<FileEntry> findFile(const FolderEntry &folder, const uint32_t nameHash) const;

	bool _littleEndian;
	uint32_t _version;

	std::vector<FolderEntry> _folderEntries;
	std::vector<FileEntry> _fileEntries;

	std::unique_ptr<Common::ReadStream> _rmdp;

	/*
	 * TODO: Currently the reading is critical sectioned allowing only one parallel read at a time in the archive. This
	 * could be improved by creating and utilizing a fixed set of streams allowing parallel access to the archive
	 */
	mutable std::mutex _readMutex;
};

} // End of namespace AWE

#endif //AWE_RMDPARCHIVE_H
