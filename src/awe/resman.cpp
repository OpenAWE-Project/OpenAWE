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

#include <memory>
#include <iostream>
#include <filesystem>

#include "src/common/readfile.h"

#include "resman.h"
#include "src/awe/rmdparchive.h"
#include "src/awe/streamedresourcefile.h"

namespace AWE {

void RessourceManager::indexPackmeta(const std::string &packmetaFile) {
	std::unique_ptr<Common::ReadStream> packmeta;
	packmeta.reset(getResource(packmetaFile));

	if (!packmeta)
		throw std::runtime_error("Invalid packmeta file");

	_meta.emplace_back();
	_meta.back() = std::make_unique<PACKMETAFile>(*packmeta);
}

void RessourceManager::indexStreamedResource(const std::string &resourcedbFile) {
	std::unique_ptr<Common::ReadStream> resourcedb;
	resourcedb.reset(getResource(resourcedbFile));

	_meta.emplace_back();
	_meta.back() = std::make_unique<StreamedResourceFile>(*resourcedb);
}

void RessourceManager::indexArchive(const std::string &binFile, const std::string &rmdpFile) {
	Common::ReadFile *bin, *rmdp;
	bin = new Common::ReadFile(binFile);
	rmdp = new Common::ReadFile(rmdpFile);
	_archives.emplace_back();
	_archives.back() = std::make_unique<RMDPArchive>(bin, rmdp);
}

bool RessourceManager::hasResource(const std::string &path) {
	if (std::filesystem::is_regular_file(_rootPath + "/" + path))
		return true;

	for (const auto &archive : _archives) {
		if (archive->hasResource(path))
			return true;
	}

	return false;
}

bool RessourceManager::hasDirectory(const std::string &path) {
	if (std::filesystem::is_directory(_rootPath + "/" + path))
		return true;

	for (const auto &archive : _archives) {
		if (archive->hasDirectory(path))
			return true;
	}

	return false;
}

std::vector<std::string> RessourceManager::getDirectoryResources(const std::string &path) {
	std::vector<std::string> paths;
	for (auto &archive : _archives) {
		const auto indices = archive->getDirectoryResources(path);
		for (const auto &index: indices) {
			paths.emplace_back(archive->getResourcePath(index));
		}
	}

	return paths;
}

Common::ReadStream *RessourceManager::getResource(const std::string &path) {
	if (std::filesystem::is_regular_file(_rootPath + "/" + path))
		return new Common::ReadFile(_rootPath + "/" + path);

	for (auto &archive : _archives) {
		Common::ReadStream *stream = archive->getResource(path);
		if (stream != nullptr)
			return stream;
	}

	return nullptr;
}

Common::ReadStream *RessourceManager::getResource(rid_t rid) {
	for (const auto &meta : _meta) {
		const std::string path = meta->getNameByRid(rid);

		if (path.empty())
			continue;

		return getResource(path);
	}
	return nullptr;
}

void RessourceManager::setRootPath(const std::string &rootPath) {
	_rootPath = rootPath;
}

} // End of namespace AWE
