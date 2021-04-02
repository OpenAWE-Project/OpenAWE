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

#ifndef OPENAWE_OBJECTCOLLECTION_H
#define OPENAWE_OBJECTCOLLECTION_H

#include <vector>

#include <entt/entt.hpp>

#include "src/common/readstream.h"

#include "src/awe/script/collection.h"
#include "src/awe/gidregistryfile.h"
#include "src/awe/cidfile.h"

class ObjectCollection {
public:
	virtual ~ObjectCollection();

protected:
	ObjectCollection(entt::registry &registry);

	void loadGIDRegistry(Common::ReadStream *stream);
	void loadBytecode(Common::ReadStream *bytecode, Common::ReadStream *bytecodeParameters);

	void load(Common::ReadStream * stream);
	void load(Common::ReadStream * stream, DPFile &dp);

	entt::registry &_registry;

private:
	void load(const AWE::CIDFile::Container &container);

	void loadSkeleton(const AWE::CIDFile::Container &container);
	void loadAnimation(const AWE::CIDFile::Container &container);
	void loadNotebookPage(const AWE::CIDFile::Container &container);
	void loadStaticObject(const AWE::CIDFile::Container &container);
	void loadDynamicObject(const AWE::CIDFile::Container &container);
	void loadDynamicObjectScript(const AWE::CIDFile::Container &container);
	void loadCharacter(const AWE::CIDFile::Container &container);
	void loadScriptInstance(const AWE::CIDFile::Container &container);
	void loadObjectScript(const AWE::CIDFile::Container &container);
	void loadFloatingScript(const AWE::CIDFile::Container &container);
	void loadPointLight(const AWE::CIDFile::Container &container);
	void loadAreaTrigger(const AWE::CIDFile::Container &container);
	void loadTaskDefinition(const AWE::CIDFile::Container &container);
	void loadWaypoint(const AWE::CIDFile::Container &container);
	void loadSound(const AWE::CIDFile::Container &container);

	std::vector<entt::entity> _entities;
	std::unique_ptr<AWE::GIDRegistryFile> _gid;
	std::unique_ptr<AWE::Script::Collection> _bytecode;
};


#endif //OPENAWE_OBJECTCOLLECTION_H
