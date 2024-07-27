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
#include <filesystem>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/exception.h"

#include "src/awe/resman.h"

#include "src/graphics/meshman.h"

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
		std::unique_ptr<Common::ReadStream> meshResource(ResMan.getResource(rid));
		if (!meshResource) {
			spdlog::debug("Mesh {} is missing. Falling back to MissingMesh mesh.", rid);
			return getMissingMesh();
		}

		try {
			const auto &loader = getMeshLoader(std::filesystem::path(ResMan.getResourcePath(rid)).extension().string());
			return _meshRegistry[rid] = loader.load(*meshResource, {"depth", "material"});
		} catch (std::exception &e) {
			spdlog::error("Error while loading mesh {:x}: {}", rid, e.what());
			return getBrokenMesh();
		}
	} else {
		return iter->second;
	}
}

MeshPtr MeshManager::getMesh(const std::string &path) {
	auto iter = _meshRegistry.find(path);
	if (iter == _meshRegistry.end()) {
		std::unique_ptr<Common::ReadStream> meshResource(ResMan.getResource(path));
		if (!meshResource) {
			spdlog::debug("Mesh {} is missing. Falling back to MissingMesh mesh.", path);
			return getMissingMesh();
		}

		try {
			const auto &loader = getMeshLoader(std::filesystem::path(path).extension().string());
			return _meshRegistry[path] = loader.load(*meshResource, {"depth", "material"});
		} catch (std::exception &e) {
			spdlog::error("Error while loading mesh \"{}\": {}", path, e.what());
			if (path != _brokenMeshPath)
				return getBrokenMesh();
			return nullptr;
		}
	} else {
		return iter->second;
	}
}

MeshPtr MeshManager::getMesh(const std::string &path, std::initializer_list<std::string> stages) {
	auto iter = _meshRegistry.find(path);
	if (iter == _meshRegistry.end()) {
		std::unique_ptr<Common::ReadStream> meshResource(ResMan.getResource(path));
		if (!meshResource) {
			spdlog::debug("Mesh {} is missing. Falling back to MissingMesh mesh.", path);
			return getMissingMesh();
		}

		try {
			const auto &loader = getMeshLoader(std::filesystem::path(path).extension().string());
			return _meshRegistry[path] = loader.load(*meshResource, stages);
		} catch (std::exception &e) {
			spdlog::error("Error while loading mesh \"{}\": {}", path, e.what());
			if (path != _brokenMeshPath)
				return getBrokenMesh();
			return nullptr;
		}
	} else {
		return iter->second;
	}
}

void MeshManager::clear() {
	_meshRegistry.clear();
}

MeshPtr MeshManager::getMissingMesh() {
	return getMesh(_missingMeshPath);
}

MeshPtr MeshManager::getBrokenMesh() {
	return getMesh(_brokenMeshPath);
}

const MeshLoader &MeshManager::getMeshLoader(const std::string &extension) {
	const auto loaderIter = std::find_if(
		_loaders.begin(),
		_loaders.end(),
		[&](const auto &loader){ return loader->isExtensionSupported(extension); }
	);

	if (loaderIter == _loaders.end())
		throw CreateException("No loader available for type {}", extension);

	return **loaderIter;
}

}
