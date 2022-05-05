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

#include <algorithm>

#include <src/common/memreadstream.h>
#include "src/common/zlib.h"
#include "src/common/readstream.h"
#include "src/common/exception.h"

#include "binarchive.h"
#include "resman.h"

namespace AWE {

BINArchive::BINArchive(Common::ReadStream &bin) {
	load(bin);
}

BINArchive::BINArchive(const std::string &resource) {
	std::unique_ptr<Common::ReadStream> bin(ResMan.getResource(resource));

	load(*bin);
}

size_t BINArchive::getNumResources() const {
	return _fileEntries.size();
}

Common::ReadStream *BINArchive::getResource(const std::string &rid) const {
	for (const auto &entry : _fileEntries) {
		if (entry.name == rid) {
			byte *data = new byte[entry.size];

			_data->seek(entry.offset);
			_data->read(data, entry.size);

			return new Common::MemoryReadStream(data, entry.size);
		}
	}

	return nullptr;
}

void BINArchive::load(Common::ReadStream &bin) {
	bin.seek(0, Common::ReadStream::END);
	unsigned int fileSize = bin.pos();
	bin.seek(0);

	const uint32_t numFiles = bin.readUint32LE();

	_fileEntries.resize(numFiles);
	uint32_t offset = 0;
	for (auto &entry : _fileEntries) {
		const uint32_t nameLength = bin.readUint32LE();
		entry.name.resize(nameLength);
		bin.read(entry.name.data(), nameLength);

		entry.size = bin.readUint32LE();
		entry.offset = offset;

		offset += entry.size;
	}

	size_t compressedSize = fileSize - bin.pos();
	size_t decompressedSize = offset;

	byte *data = new byte[compressedSize];
	bin.read(data, compressedSize);

	_data.reset(Common::decompressZLIB(
			data,
			compressedSize,
			decompressedSize
	));

	delete[] data;
}

bool BINArchive::hasResource(const std::string &rid) const {
	return std::find_if(
			_fileEntries.begin(),
			_fileEntries.end(),
			[&](auto entry) -> bool {
				return entry.name == rid;
			}
	) != _fileEntries.end();
}

bool BINArchive::hasDirectory(const std::string &directory) const {
	if (directory.empty())
		return true;
	return false;
}

std::string BINArchive::getResourcePath(size_t index) const {
	if (index >= getNumResources())
		throw Common::Exception(
				"Index {} exceeds number of file entries {}",
				index,
				getNumResources()
		);

	return _fileEntries[index].name;
}

} // End of namespace AWE
