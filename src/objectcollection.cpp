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

#include <spdlog/spdlog.h>

#include "common/convexshape.h"

#include "awe/cidfile.h"
#include "awe/dpfile.h"

#include "src/graphics/model.h"

#include "objectcollection.h"

#include <memory>
#include "transform.h"
#include "task.h"

ObjectCollection::ObjectCollection(entt::registry &registry) : _registry(registry) {
}

ObjectCollection::~ObjectCollection() {
	_registry.destroy(_entities.begin(), _entities.end());
}

void ObjectCollection::loadGIDRegistry(Common::ReadStream *stream) {
	std::unique_ptr<Common::ReadStream> gidStream(stream);
	_gid = std::make_unique<AWE::GIDRegistryFile>(*gidStream);
}

void ObjectCollection::loadBytecode(Common::ReadStream *bytecode, Common::ReadStream *bytecodeParameters) {
	_bytecode = std::make_unique<AWE::Script::Collection>(
			bytecode,
			bytecodeParameters
	);
}

void ObjectCollection::load(Common::ReadStream *stream) {
	std::unique_ptr<Common::ReadStream> cidStream(stream);
	AWE::CIDFile cid(*cidStream, nullptr);

	for (const auto &container : cid.getContainers()) {
		load(container);
	}
}

void ObjectCollection::load(Common::ReadStream *stream, DPFile &dp) {
	std::unique_ptr<Common::ReadStream> cidStream(stream);
	AWE::CIDFile cid(*cidStream, &dp);

	for (const auto &container : cid.getContainers()) {
		load(container);
	}
}

void ObjectCollection::load(const AWE::CIDFile::Container &container) {
	switch (container.type) {
		case AWE::CIDFile::kSkeleton: loadSkeleton(container); break;
		case AWE::CIDFile::kAnimation: loadAnimation(container); break;
		case AWE::CIDFile::kNotebookPage: loadNotebookPage(container); break;
		case AWE::CIDFile::kStaticObject: loadStaticObject(container); break;
		case AWE::CIDFile::kDynamicObject: loadDynamicObject(container); break;
		case AWE::CIDFile::kDynamicObjectScript: loadDynamicObjectScript(container); break;
		case AWE::CIDFile::kCharacter: loadCharacter(container); break;
		case AWE::CIDFile::kScriptInstance: loadScriptInstance(container); break;
		case AWE::CIDFile::kScriptInstanceScript: loadObjectScript(container); break;
		case AWE::CIDFile::kAreaTrigger: loadAreaTrigger(container); break;
		case AWE::CIDFile::kFloatingScript: loadFloatingScript(container); break;
		case AWE::CIDFile::kTaskDefinition: loadTaskDefinition(container); break;
		case AWE::CIDFile::kWaypoint: loadWaypoint(container); break;
		case AWE::CIDFile::kSound: loadSound(container); break;
	}
}

void ObjectCollection::loadSkeleton(const AWE::CIDFile::Container &container) {
	const auto skeleton = std::get<AWE::CIDFile::Skeleton>(container.data);

	auto skeletonEntity = _registry.create();
	_registry.emplace<GID>(skeletonEntity) = skeleton.gid;
	// TODO: Load a representation of the skeleton

	spdlog::debug("Loading skeleton {}", skeleton.name);
}

void ObjectCollection::loadAnimation(const AWE::CIDFile::Container &container) {
	const auto animation = std::get<AWE::CIDFile::Animation>(container.data);

	auto animationEntity = _registry.create();
	_registry.emplace<GID>(animationEntity) = animation.gid;
	// TODO: Load a representation of the animation

	spdlog::debug("Loading animation {} for skeleton {}", animation.name, _gid->getString(animation.skeletonGid));
}

void ObjectCollection::loadNotebookPage(const AWE::CIDFile::Container &container) {
	const auto notebookPage = std::get<AWE::CIDFile::NotebookPage>(container.data);

	auto notebookPageEntity = _registry.create();
	_registry.emplace<GID>(notebookPageEntity) = notebookPage.gid;
	_entities.emplace_back(notebookPageEntity);

	spdlog::debug("Loading notebook page {}", _gid->getString(notebookPage.gid));
}

void ObjectCollection::loadStaticObject(const AWE::CIDFile::Container &container) {
	const auto staticObject = std::get<AWE::CIDFile::StaticObject>(container.data);

	auto staticObjectEntity = _registry.create();
	_registry.emplace<Transform>(staticObjectEntity) = Transform(staticObject.position, staticObject.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(staticObjectEntity) = std::make_shared<Graphics::Model>(staticObject.meshResource);
	// TODO: Physics Resource

	model->getPosition() = staticObject.position;
	model->getRotation() = staticObject.rotation;

	_entities.emplace_back(staticObjectEntity);
}

void ObjectCollection::loadDynamicObject(const AWE::CIDFile::Container &container) {
	const auto dynamicObject = std::get<AWE::CIDFile::DynamicObject>(container.data);

	auto dynamicObjectEntity = _registry.create();
	_registry.emplace<GID>(dynamicObjectEntity) = dynamicObject.gid;
	_registry.emplace<Transform>(dynamicObjectEntity) = Transform(dynamicObject.position, dynamicObject.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(dynamicObjectEntity) = std::make_shared<Graphics::Model>(dynamicObject.meshResource);
	// TODO: Physics Resource

	model->getPosition() = dynamicObject.position;
	model->getRotation() = dynamicObject.rotation;

	_entities.emplace_back(dynamicObjectEntity);

	spdlog::debug("Loading dynamic object {}", _gid->getString(dynamicObject.gid));
}

void ObjectCollection::loadDynamicObjectScript(const AWE::CIDFile::Container &container) {
	const auto dynamicObjectScript = std::get<AWE::CIDFile::DynamicObjectScript>(container.data);

	auto gidView = _registry.view<GID>();
	entt::entity scriptEntity = entt::null;
	for (const auto &entity : gidView) {
		GID gid = _registry.get<GID>(entity);
		if (gid == dynamicObjectScript.gid) {
			scriptEntity = entity;
			break;
		}
	}

	if (scriptEntity == entt::null)
		throw std::runtime_error("Couldn't find script entity");

	_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = AWE::Script::BytecodePtr(_bytecode->createScript(
			dynamicObjectScript.script));

	spdlog::debug("Loading script for dynamic object {}", _gid->getString(dynamicObjectScript.gid));
}

void ObjectCollection::loadCharacter(const AWE::CIDFile::Container &container) {
	const auto character = std::get<AWE::CIDFile::Character>(container.data);

	auto characterEntity = _registry.create();
	_registry.emplace<GID>(characterEntity) = character.gid;
	_registry.emplace<Transform>(characterEntity) = Transform(character.position, character.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(characterEntity) = std::make_shared<Graphics::Model>(character.meshResource);
	// TODO: Physics and Cloth Resource

	model->getPosition() = character.position;
	model->getRotation() = character.rotation;

	_entities.emplace_back(characterEntity);

	spdlog::debug("Loading character {}", _gid->getString(character.gid));
}

void ObjectCollection::loadScriptInstance(const AWE::CIDFile::Container &container) {
	const auto scriptInstance = std::get<AWE::CIDFile::ScriptInstance>(container.data);

	auto scriptInstanceEntity = _registry.create();
	_registry.emplace<GID>(scriptInstanceEntity) = scriptInstance.gid;
	_registry.emplace<Transform>(scriptInstanceEntity) = Transform(scriptInstance.position,  scriptInstance.rotation);

	spdlog::debug("Loading script instance {}", _gid->getString(scriptInstance.gid));
}

void ObjectCollection::loadObjectScript(const AWE::CIDFile::Container &container) {
	const auto scriptInstanceScript = std::get<AWE::CIDFile::ObjectScript>(container.data);

	auto gidView = _registry.view<GID>();
	entt::entity scriptEntity = entt::null;
	for (const auto &entity : gidView) {
		GID gid = _registry.get<GID>(entity);
		if (gid == scriptInstanceScript.gid) {
			scriptEntity = entity;
			break;
		}
	}

	if (scriptEntity == entt::null)
		throw std::runtime_error("Couldn't find script entity");

	_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = AWE::Script::BytecodePtr(_bytecode->createScript(
			scriptInstanceScript.script));

	spdlog::debug("Loading script for object {}", _gid->getString(scriptInstanceScript.gid));
}

void ObjectCollection::loadFloatingScript(const AWE::CIDFile::Container &container) {
	const auto floatingScript = std::get<AWE::CIDFile::FloatingScript>(container.data);

	auto floatingScriptEntity = _registry.create();
	_registry.emplace<GID>(floatingScriptEntity) = floatingScript.gid;
	_registry.emplace<Transform>(floatingScriptEntity) = Transform(floatingScript.position, floatingScript.rotation);
	_registry.emplace<AWE::Script::BytecodePtr>(floatingScriptEntity) = AWE::Script::BytecodePtr(
			_bytecode->createScript(floatingScript.script));

	spdlog::debug("Loading floating script {}", _gid->getString(floatingScript.gid));
}

void ObjectCollection::loadPointLight(const AWE::CIDFile::Container &container) {
	const auto pointLight = std::get<AWE::CIDFile::PointLight>(container.data);

	auto pointLightEntity = _registry.create();
	_registry.emplace<GID>(pointLightEntity) = pointLight.gid;
	_registry.emplace<Transform>(pointLightEntity) = Transform(pointLight.position, pointLight.rotation);

	spdlog::debug("Loading point light {}", _gid->getString(pointLight.gid));
}

void ObjectCollection::loadAreaTrigger(const AWE::CIDFile::Container &container) {
	const auto areaTrigger = std::get<AWE::CIDFile::AreaTrigger>(container.data);

	auto areaTriggerEntity = _registry.create();
	_registry.emplace<GID>(areaTriggerEntity) = areaTrigger.gid;
	_registry.emplace<Common::ConvexShape>(areaTriggerEntity) = areaTrigger.positions;

	spdlog::debug("Loading area trigger {}", areaTrigger.identifier);
}

void ObjectCollection::loadTaskDefinition(const AWE::CIDFile::Container &container) {
	const auto taskDefinition = std::get<AWE::CIDFile::TaskDefinition>(container.data);

	auto taskEntity = _registry.create();
	if (taskDefinition.gid.isNil())
		return;

	_registry.emplace<GID>(taskEntity) = taskDefinition.gid;
	_registry.emplace<Transform>(taskEntity) = Transform(taskDefinition.position, taskDefinition.rotation);
	_registry.emplace<Task>(taskEntity) = Task(taskDefinition.activateOnStartup);

	spdlog::debug("Loading task {}", _gid->getString(taskDefinition.gid));
}

void ObjectCollection::loadWaypoint(const AWE::CIDFile::Container &container) {
	const auto wayPoint = std::get<AWE::CIDFile::Waypoint>(container.data);

	auto wayPointEntity = _registry.create();
	_registry.emplace<GID>(wayPointEntity) = wayPoint.gid;
	_registry.emplace<Transform>(wayPointEntity) = Transform(wayPoint.position, wayPoint.rotation);

	spdlog::debug("Loading way point {}", _gid->getString(wayPoint.gid));
}

void ObjectCollection::loadSound(const AWE::CIDFile::Container &container) {
	const auto sound = std::get<AWE::CIDFile::Sound>(container.data);

	auto soundEntity = _registry.create();
	_registry.emplace<GID>(soundEntity) = sound.gid;
	// TODO

	spdlog::debug("Loading sound {}", _gid->getString(sound.gid));
}
