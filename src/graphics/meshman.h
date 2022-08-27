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
#include "src/graphics/mesh_binfol.h"

#include "src/awe/types.h"

namespace Graphics {

class MeshManager : public Common::Singleton<MeshManager> {
public:
	MeshManager();

	MeshPtr getMesh(rid_t rid);
	MeshPtr getMesh(const std::string &path);

	/*!
	 * Get the specific mesh as foliage mesh
	 * \param path The path to the mesh as string
	 * \return A pointer to the specific foliage mesh
	 */
	BINFOLMeshPtr getFoliageMesh(const std::string &path);

private:
	MeshPtr getMissingMesh();
	MeshPtr getBrokenMesh();

	std::string _missingMeshPath, _brokenMeshPath;

	std::map<std::variant<rid_t, std::string>, MeshPtr> _meshRegistry;
};

} // End of namespace Graphics

#define MeshMan Graphics::MeshManager::instance()

#endif //AWE_MESHMANAGER_H
