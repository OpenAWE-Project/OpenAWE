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

#include "archive.h"

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
	size_t getNumResources() override;

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
	 */
	void loadHeaderV2(Common::ReadStream *bin);

	/*!
	 * Load header version 7 used by Alan Wakes American Nightmare
	 *
	 * \param bin the stream from which to load the header
	 */
	void loadHeaderV7(Common::ReadStream *bin);

	/*!
	 * Load header version 8 used by Quantum Break
	 *
	 * \param bin the stream from which to load the header
	 */
	void loadHeaderV8(Common::ReadStream *bin);

	/*!
	 * Structure describing a folder entry of the loaded bin/rmdp archive
	 * structure. Has name hash for faster comparison and indices to the next
	 * lower folder, the next neighbour folder and the next folder inside.
	 */
	struct FolderEntry {
		std::string name;
		uint32_t nameHash;
		uint32_t nextLowerFolder;
		uint32_t nextNeighbourFolder;
		uint32_t nextFile;
		uint32_t prevFolder;
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
		uint32_t nextFile;
		uint32_t prevFolder;
		uint32_t flags;
		uint64_t offset, size;
	};

	bool _pathPrefix;
	bool _littleEndian;

	std::vector<FolderEntry> _folderEntries;
	std::vector<FileEntry> _fileEntries;

	std::unique_ptr<Common::ReadStream> _rmdp;
};

} // End of namespace AWE

#endif //AWE_RMDPARCHIVE_H
