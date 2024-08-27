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

#ifndef AWE_RMDLARCHIVE_H
#define AWE_RMDLARCHIVE_H

#include <vector>
#include <memory>

#include "archive.h"

#include "src/common/readstream.h"

namespace AWE {

/*!
 * \brief Loader for rmdl archives
 *
 * This class handles the loading of rmdl archives. These archives include
 * a compact header with an identifier and the index size. The header
 * is followed by the uncompressed raw data and the index is stored at
 * the end of the file. The index defines the size and name of each file
 * without an offset.
 *
 * The rmdl archive format is currently utilized in the following games from
 * remedy:
 * - Control
 */
class RMDLArchive : public Archive {
public:
	/*!
	 * Load a rmdl archive from the specified stream
	 *
	 * \param bin the stream to load from
	 */
	RMDLArchive(Common::ReadStream &bin);

	RMDLArchive(const std::string &resource);

	Common::ReadStream *getResource(const std::string &rid) const override;

	bool hasResource(const std::string &rid) const override;

	size_t getNumResources() const override;

	std::vector<size_t> getDirectoryResources(const std::string &directory) override;

	std::string getResourcePath(size_t index) const override;

	bool hasDirectory(const std::string &directory) const override;

private:
	void load(Common::ReadStream &bin);

	struct FileEntry {
		std::string name;
		uint32_t size, offset;
	};

	std::vector<FileEntry> _fileEntries;

	std::unique_ptr<Common::ReadStream> _rmdl;

	Common::ReadStream& _stream;
};

} // End of namespace AWE


#endif //AWE_RMDLARCHIVE_H
