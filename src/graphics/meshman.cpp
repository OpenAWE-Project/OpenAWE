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

#include <stdexcept>
#include <regex>

#include <fmt/format.h>

#include "meshman.h"
#include "src/awe/resman.h"

#include "src/graphics/mesh_binmsh.h"
#include "src/graphics/mesh_binfol.h"


namespace Graphics {

MeshManager::MeshManager() {
	if (ResMan.hasResource("objects/hardcoded/helpers_missingmesh.binmsh"))
		_missingMeshPath = "objects/hardcoded/helpers_missingmesh.binmsh";
	if (ResMan.hasResource("objects/hardcoded/helpers_brokenmesh.binmsh"))
		_brokenMeshPath = "objects/hardcoded/helpers_brokenmesh.binmsh";

	if (_missingMeshPath.empty())
		throw std::runtime_error("No missing mesh mesh found!");
	if (_brokenMeshPath.empty())
		throw std::runtime_error("No broken mesh mesh found!");
}

MeshPtr MeshManager::getMesh(rid_t rid) {
	auto iter = _meshRegistry.find(rid);
	if (iter == _meshRegistry.end()) {
		Common::ReadStream *meshResource = ResMan.getResource(rid);
		if (!meshResource)
			return getMissingMesh();

		try {
			_meshRegistry.insert(std::make_pair(rid, std::make_shared<BINMSHMesh>(meshResource)));
		} catch (std::exception &e) {
			return getBrokenMesh();
		}
		return _meshRegistry[rid];
	} else {
		return iter->second;
	}
}

MeshPtr MeshManager::getMesh(const std::string &path) {
	auto iter = _meshRegistry.find(path);
	if (iter == _meshRegistry.end()) {
		Common::ReadStream *meshResource = ResMan.getResource(path);
		if (!meshResource)
			return getMissingMesh();

		try {
			if (std::regex_match(path, std::regex(".*\\.(binmsh|binfbx)$")))
				_meshRegistry.insert(std::make_pair(path, std::make_shared<BINMSHMesh>(meshResource)));
			else if (std::regex_match(path, std::regex(".*\\.binfol$")))
				_meshRegistry.insert(std::make_pair(path, std::make_shared<BINFOLMesh>(meshResource)));
			else
				throw std::runtime_error(fmt::format("Invalid or unsupported mesh file {}", path));
		} catch (std::exception &e) {
			return getBrokenMesh();
		}
		return _meshRegistry[path];
	} else {
		return iter->second;
	}
}

MeshPtr MeshManager::getMissingMesh() {
	return getMesh(_missingMeshPath);
}

MeshPtr MeshManager::getBrokenMesh() {
	return getMesh(_brokenMeshPath);
}

}
