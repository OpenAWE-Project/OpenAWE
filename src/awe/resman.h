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

#ifndef AWE_RESMAN_H
#define AWE_RESMAN_H

#include <string>
#include <vector>
#include <memory>

#include "src/common/singleton.h"
#include "src/common/readstream.h"

#include "src/awe/archive.h"
#include "src/awe/packmetafile.h"

namespace AWE {

/*!
 * \brief Manager for laoding resources and mapping rids
 *
 * This class can be used to manage resources and index archives. It
 * is also used to read rid to path mappings from the packmeta and
 * streamed resource files
 */
class RessourceManager : public Common::Singleton<RessourceManager> {
public:
	void setRootPath(const std::string &rootPath);

	void indexPackmeta(const std::string &packmetaFile);

	void indexStreamedResource(const std::string &resourcedbFile);

	void indexArchive(const std::string &binFile, const std::string &rmdpFile);

	bool hasResource(const std::string &path);
	bool hasDirectory(const std::string &path);

	std::vector<std::string> getDirectoryResources(const std::string &path);

	Common::ReadStream *getResource(const std::string &path);

	Common::ReadStream *getResource(rid_t rid);

private:
	std::string _rootPath;
	std::vector<std::unique_ptr<RIDProvider>> _meta;
	std::vector<std::unique_ptr<Archive>> _archives;
};

} // End of namespace AWE

#define ResMan AWE::RessourceManager::instance()

#endif //AWE_RESMAN_H
