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

#ifndef AWE_FSBFILE_H
#define AWE_FSBFILE_H

#include <memory>
#include <string>
#include <vector>

#include "src/common/readstream.h"

namespace Sound {

/*!
 * \brief Reader class for fmod sound bank files
 *
 * This reader is responsible for loading streams from an fmod sound bank file in version 4. These files have the ending
 * fsb and the magic 4 bytes "FSB4" in ascii.
 */
class FSBFile {
public:
	FSBFile(Common::ReadStream *fsb);

	/*!
	 * Get number of entries in the fsb file
	 * \return The number of entries in this FSB file
	 */
	size_t getNumEntries();

	/*!
	 * Get a specific stream on the given index
	 * \param index the stream to get for this index
	 * \return The newly created stream
	 */
	Common::ReadStream *getStream(size_t index);

	std::string getFileName(size_t index);

private:
	struct Entry {
		std::string fileName;
		uint32_t offset, size;
	};

	uint32_t _dataOffset;

	std::vector<Entry> _entries;

	std::unique_ptr<Common::ReadStream> _fsb;
};

} // End of namespace Soun

#endif //AWE_FSBFILE_H
