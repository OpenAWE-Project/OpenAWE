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

#ifndef AWE_BINARCHIVE_H
#define AWE_BINARCHIVE_H

#include <vector>
#include <memory>

#include "archive.h"

#include "src/common/readstream.h"

namespace AWE {

/*!
 * \brief Loader for bin archives
 *
 * This class is responsible for loading bin archives. These
 * archives have little information, every entry only
 * specifies the size of the file without an offset. The data
 * is stored inside a following chunk which is compressed
 * through zlibs deflate algorithm.
 */
class BINArchive : public Archive {
public:
	/*!
	 * Load a bin archive from the specified stream
	 *
	 * \param bin the stream to load from
	 */
	BINArchive(Common::ReadStream &bin);

	BINArchive(const std::string &resource);

	Common::ReadStream *getResource(const std::string &rid) const override;

	bool hasResource(const std::string &rid) const override;

	size_t getNumResources() const override;

	std::string getResourcePath(size_t index) const override;

	bool hasDirectory(const std::string &directory) const override;

private:
	void load(Common::ReadStream &bin);

	struct FileEntry {
		std::string name;
		uint32_t size, offset;
	};

	std::vector<FileEntry> _fileEntries;

	std::unique_ptr<Common::ReadStream> _data;
};

} // End of namespace AWE


#endif //AWE_BINARCHIVE_H
