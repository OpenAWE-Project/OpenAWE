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

#ifndef AWE_MESHMANAGER_H
#define AWE_MESHMANAGER_H

#include <map>
#include <variant>

#include "src/common/singleton.h"

#include "src/graphics/mesh.h"

#include "src/awe/types.h"

namespace Graphics {

/*!
 * \brief Interface for implementing a mesh loader
 *
 * This interface describes the loading for a mesh from a file given as
 * read stream and returns a usable mesh pointer
 */
class MeshLoader {
public:
	MeshLoader(const std::initializer_list<std::string> fileEndings) : _fileEndings(fileEndings) {}

	bool isExtensionSupported(const std::string &extension) const {
		return std::find(_fileEndings.cbegin(), _fileEndings.cend(), extension) != _fileEndings.end();
	}

	virtual MeshPtr load(Common::ReadStream &) const = 0;

private:
	const std::vector<std::string> _fileEndings;
};

class MeshManager : public Common::Singleton<MeshManager> {
public:
	MeshManager();

	/*!
	 * Append new loader to the end of the loaders list. The priority is the order of the added loaders e.g. from the
	 * front of the loaders list to the back.
	 * @tparam T The type of the loader to be appended
	 * @tparam Params The parameters of the loader creation
	 * @param args The arguments for the creation of the loader
	 */
	template<typename T, typename... Params> void appendLoader(Params... args) {
		_loaders.emplace_back(std::make_unique<T>(std::forward<Params>(args)...));
	}

	MeshPtr getMesh(rid_t rid);
	MeshPtr getMesh(const std::string &path);

private:
	MeshPtr getMissingMesh();
	MeshPtr getBrokenMesh();

	const MeshLoader &getMeshLoader(const std::string &extension);

	std::vector<std::unique_ptr<MeshLoader>> _loaders;

	std::string _missingMeshPath, _brokenMeshPath;

	std::map<std::variant<rid_t, std::string>, MeshPtr> _meshRegistry;
};

} // End of namespace Graphics

#define MeshMan Graphics::MeshManager::instance()

#endif //AWE_MESHMANAGER_H
