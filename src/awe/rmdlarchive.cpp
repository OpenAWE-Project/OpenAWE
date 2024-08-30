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
#include "src/common/readstream.h"
#include "src/common/exception.h"

#include "rmdlarchive.h"
#include "resman.h"

static const uint32_t kRMDL = MKTAG('R', 'M', 'D', 'L');

namespace AWE {

RMDLArchive::RMDLArchive(Common::ReadStream &bin) : _stream(bin) {
	load(_stream);
}

RMDLArchive::RMDLArchive(const std::string &resource) :
		_rmdl(ResMan.getResource(resource)),
		_stream(_rmdl ? *_rmdl : throw Common::Exception("Failed to load resource {}",resource)) {
	load(_stream);
}

size_t RMDLArchive::getNumResources() const {
	return _fileEntries.size();
}

Common::ReadStream *RMDLArchive::getResource(const std::string &rid) const {
	for (const auto &entry : _fileEntries) {
		if (entry.name == rid) {
			byte *data = new byte[entry.size];

			_stream.seek(entry.offset);
			_stream.read(data, entry.size);

			return new Common::MemoryReadStream(data, entry.size);
		}
	}

	return nullptr;
}

void RMDLArchive::load(Common::ReadStream &bin) {
	bin.seek(0);
	const auto magic = bin.readUint32BE();
	if (magic != kRMDL)
		throw Common::Exception("Invalid rmdl archive");

	const size_t indexSize = bin.readUint32LE();
	uint32_t offset = bin.pos();

	// The index is at the end of RMDL files
	if (indexSize) {
		bin.seek(-indexSize, Common::ReadStream::END);

		const uint32_t numFiles = bin.readUint32LE();

		_fileEntries.resize(numFiles);
		for (auto &entry : _fileEntries) {
			entry.size = bin.readUint32LE();

			const uint32_t nameLength = bin.readUint32LE();
			entry.name.resize(nameLength);
			bin.read(entry.name.data(), nameLength);

			entry.offset = offset;
			offset += entry.size;
		}
	}
}

bool RMDLArchive::hasResource(const std::string &rid) const {
	return std::find_if(
			_fileEntries.begin(),
			_fileEntries.end(),
			[&](auto entry) -> bool {
				return entry.name == rid;
			}
	) != _fileEntries.end();
}

bool RMDLArchive::hasDirectory(const std::string &directory) const {
	if (directory.empty())
		return true;
	return false;
}

std::string RMDLArchive::getResourcePath(size_t index) const {
	if (index >= getNumResources())
		throw Common::Exception(
				"Index {} exceeds number of file entries {}",
				index,
				getNumResources()
		);

	return _fileEntries[index].name;
}

std::vector<size_t> RMDLArchive::getDirectoryResources(const std::string &directory) {
	if (!directory.empty())
		return {};

	std::vector<size_t> indices;
	for (size_t i = 0; i < getNumResources(); ++i) {
		indices.emplace_back(i);
	}

	return indices;
}

} // End of namespace AWE
