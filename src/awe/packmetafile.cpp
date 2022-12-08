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

#include <vector>
#include <regex>
#include "packmetafile.h"
#include "types.h"
#include "cidfile.h"

struct FileEntry {
	uint32_t offset;
	std::string name;
};

struct RIDEntry {
	uint32_t offset;
	rid_t rid;
};

namespace AWE {

PACKMETAFile::PACKMETAFile(Common::ReadStream &packmeta) {
	uint32_t numElements = packmeta.readUint32LE();
	packmeta.skip(8);
	uint32_t nameSize = packmeta.readUint32LE();

	std::vector<FileEntry> fileEntries;
	fileEntries.resize(numElements);

	for (auto &item: fileEntries) {
		item.name = packmeta.readNullTerminatedString();
		item.name = std::regex_replace(item.name, std::regex("\\\\"), "/");
		item.name = std::regex_replace(item.name, std::regex("d:/data/"), "");
	}

	for (auto &item: fileEntries) {
		item.offset = packmeta.readUint32LE();
	}

	uint32_t ridCount = packmeta.readUint32LE();

	std::vector<RIDEntry> ridEntries;
	ridEntries.resize(ridCount);

	for (auto &entry: ridEntries) {
		entry.rid = packmeta.readUint32BE();
	}

	for (auto &entry: ridEntries) {
		entry.offset = packmeta.readUint32LE();
	}

	for (const auto &entry: fileEntries) {
		for (const auto &ridEntry: ridEntries) {
			if (entry.offset == ridEntry.offset)
				_resources[ridEntry.rid] = entry.name;
		}
	}

	uint32_t count = packmeta.readUint32LE();

	packmeta.skip(count * 4);

	// TODO: Read additional CID Information appended to the file
	//AWE::CIDFile cidFile(packmeta, ridCount);
}

} // End of namespace AWE
