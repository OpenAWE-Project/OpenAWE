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
#include "awe/foliagedatafile.h"
#include "awe/object.h"

#include "src/graphics/model.h"
#include "src/graphics/meshman.h"

#include "src/physics/collisionobject.h"

#include "objectcollection.h"

#include <memory>
#include "transform.h"
#include "task.h"
#include "utils.h"

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

void ObjectCollection::load(Common::ReadStream *stream, ObjectType type) {
	std::unique_ptr<Common::ReadStream> cidStream(stream);
	AWE::CIDFile cid(*cidStream, type, nullptr);

	for (const auto &container : cid.getContainers()) {
		load(container, type);
	}
}

void ObjectCollection::load(Common::ReadStream *stream, ObjectType type, std::shared_ptr<DPFile> dp) {
    if (!stream)
        return;

	std::unique_ptr<Common::ReadStream> cidStream(stream);
	AWE::CIDFile cid(*cidStream, type, dp);

	for (const auto &container : cid.getContainers()) {
		load(container, type);
	}
}

void ObjectCollection::loadFoliageData(Common::ReadStream *foliageData) {
	std::unique_ptr<Common::ReadStream> foliageDataStream(foliageData);
	AWE::FoliageDataFile foliageDataFile(*foliageDataStream);

	std::vector<Graphics::MeshPtr> foliageMeshs;
	for (const auto &foliage : foliageDataFile.getFoliages()) {
		foliageMeshs.emplace_back(MeshMan.getMesh(foliage));
	}

	for (const auto &instance : foliageDataFile.getInstances()) {
		entt::entity foliage = _registry.create();
		Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(foliage) = std::make_shared<Graphics::Model>(foliageMeshs[instance.foliageId]);

		model->getPosition() = instance.position;
		model->getRotation() = glm::identity<glm::mat3>();

		_entities.emplace_back(foliage);
	}
}

void ObjectCollection::load(const AWE::Object &container, ObjectType type) {
	switch (type) {
		case kSkeleton: loadSkeleton(container); break;
		case kAnimation: loadAnimation(container); break;
		case kNotebookPage: loadNotebookPage(container); break;
		case kStaticObject: loadStaticObject(container); break;
		case kDynamicObject: loadDynamicObject(container); break;
		case kDynamicObjectScript: loadDynamicObjectScript(container); break;
		case kCharacter: loadCharacter(container); break;
		case kScriptInstance: loadScriptInstance(container); break;
		case kScript: loadScript(container); break;
		case kAreaTrigger: loadAreaTrigger(container); break;
		case kFloatingScript: loadFloatingScript(container); break;
		case kTaskDefinition: loadTaskDefinition(container); break;
		case kWaypoint: loadWaypoint(container); break;
		case kSound: loadSound(container); break;
		case kTrigger: loadTrigger(container); break;
		case kCharacterClass: loadCharacterClass(container); break;
		case kKeyframedObject: loadKeyFramedObject(container); break;
	}
}

void ObjectCollection::loadSkeleton(const AWE::Object &container) {
	const auto skeleton = std::any_cast<AWE::Templates::Skeleton>(container);

	auto skeletonEntity = _registry.create();
	_registry.emplace<GID>(skeletonEntity) = skeleton.gid;
	// TODO: Load a representation of the skeleton

	spdlog::debug("Loading skeleton {}", skeleton.name);
}

void ObjectCollection::loadAnimation(const AWE::Object &container) {
	const auto animation = std::any_cast<AWE::Templates::Animation>(container);

	auto animationEntity = _registry.create();
	_registry.emplace<GID>(animationEntity) = animation.gid;
	// TODO: Load a representation of the animation

	spdlog::debug("Loading animation {} for skeleton {}", animation.name, _gid->getString(animation.skeletonGid));
}

void ObjectCollection::loadNotebookPage(const AWE::Object &container) {
	const auto notebookPage = std::any_cast<AWE::Templates::NotebookPage>(container);

	auto notebookPageEntity = _registry.create();
	_registry.emplace<GID>(notebookPageEntity) = notebookPage.gid;
	_entities.emplace_back(notebookPageEntity);

	spdlog::debug("Loading notebook page {}", _gid->getString(notebookPage.gid));
}

void ObjectCollection::loadStaticObject(const AWE::Object &container) {
	const auto staticObject = std::any_cast<AWE::Templates::StaticObject>(container);

	auto staticObjectEntity = _registry.create();
	_registry.emplace<Transform>(staticObjectEntity) = Transform(staticObject.position, staticObject.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(staticObjectEntity) = std::make_shared<Graphics::Model>(staticObject.meshResource);

	model->getPosition() = staticObject.position;
	model->getRotation() = staticObject.rotation;

	if (staticObject.physicsResource) {
		try {
			Physics::CollisionObjectPtr collisionObject = std::make_shared<Physics::CollisionObject>(
				staticObject.physicsResource
			);

			collisionObject->setTransform(staticObject.position, staticObject.rotation);

			_registry.emplace<Physics::CollisionObjectPtr>(staticObjectEntity) = collisionObject;
		} catch (std::exception &e) {
			/*
			 * Due to the fact, that not all havok primitives are working it might throw exceptions. In this case we
			 * simply skip the creation fo the collision object
			 */
			spdlog::error("Failed to create physics object for static object: {}", e.what());
		}
	}

	_entities.emplace_back(staticObjectEntity);
}

void ObjectCollection::loadDynamicObject(const AWE::Object &container) {
	const auto dynamicObject = std::any_cast<AWE::Templates::DynamicObject>(container);

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

void ObjectCollection::loadDynamicObjectScript(const AWE::Object &container) {
	const auto dynamicObjectScript = std::any_cast<AWE::Templates::DynamicObjectScript>(container);

	const entt::entity scriptEntity = getEntityByGID(_registry, dynamicObjectScript.gid);
	if (scriptEntity == entt::null)
		throw std::runtime_error("Couldn't find script entity");

	_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = AWE::Script::BytecodePtr(_bytecode->createScript(
			dynamicObjectScript.script));

	spdlog::debug("Loading script for dynamic object {}", _gid->getString(dynamicObjectScript.gid));
}

void ObjectCollection::loadCharacter(const AWE::Object &container) {
	const auto character = std::any_cast<AWE::Templates::Character>(container);

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

void ObjectCollection::loadScriptInstance(const AWE::Object &container) {
	const auto scriptInstance = std::any_cast<AWE::Templates::ScriptInstance>(container);

	auto scriptInstanceEntity = _registry.create();
	_registry.emplace<GID>(scriptInstanceEntity) = scriptInstance.gid;
	_registry.emplace<Transform>(scriptInstanceEntity) = Transform(scriptInstance.position,  scriptInstance.rotation);

	spdlog::debug("Loading script instance {}", _gid->getString(scriptInstance.gid));
}

void ObjectCollection::loadScript(const AWE::Object &container) {
	const auto scriptInstanceScript = std::any_cast<AWE::Templates::Script>(container);

	entt::entity scriptEntity = getEntityByGID(_registry, scriptInstanceScript.gid);
	if (scriptEntity == entt::null)
		throw std::runtime_error("Couldn't find script entity");

	_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = AWE::Script::BytecodePtr(_bytecode->createScript(
			scriptInstanceScript.script));

	spdlog::debug("Loading script for object {}", _gid->getString(scriptInstanceScript.gid));
}

void ObjectCollection::loadFloatingScript(const AWE::Object &container) {
	const auto floatingScript = std::any_cast<AWE::Templates::FloatingScript>(container);

	auto floatingScriptEntity = _registry.create();
	_registry.emplace<GID>(floatingScriptEntity) = floatingScript.gid;
	_registry.emplace<Transform>(floatingScriptEntity) = Transform(floatingScript.position, floatingScript.rotation);
	_registry.emplace<AWE::Script::BytecodePtr>(floatingScriptEntity) = AWE::Script::BytecodePtr(
			_bytecode->createScript(floatingScript.script));

	spdlog::debug("Loading floating script {}", _gid->getString(floatingScript.gid));
}

void ObjectCollection::loadPointLight(const AWE::Object &container) {
	const auto pointLight = std::any_cast<AWE::Templates::PointLight>(container);

	auto pointLightEntity = _registry.create();
	_registry.emplace<GID>(pointLightEntity) = pointLight.gid;
	_registry.emplace<Transform>(pointLightEntity) = Transform(pointLight.position, pointLight.rotation);

	spdlog::debug("Loading point light {}", _gid->getString(pointLight.gid));
}

void ObjectCollection::loadAreaTrigger(const AWE::Object &container) {
	const auto areaTrigger = std::any_cast<AWE::Templates::AreaTrigger>(container);

	auto areaTriggerEntity = _registry.create();
	_registry.emplace<GID>(areaTriggerEntity) = areaTrigger.gid;
	_registry.emplace<Common::ConvexShape>(areaTriggerEntity) = areaTrigger.positions;

	spdlog::debug("Loading area trigger {}", areaTrigger.identifier);
}

void ObjectCollection::loadTaskDefinition(const AWE::Object &container) {
	const auto taskDefinition = std::any_cast<AWE::Templates::TaskDefinition>(container);

	auto taskEntity = _registry.create();
	if (taskDefinition.gid.isNil())
		return;

	_registry.emplace<GID>(taskEntity) = taskDefinition.gid;
	_registry.emplace<Transform>(taskEntity) = Transform(taskDefinition.position, taskDefinition.rotation);
	_registry.emplace<Task>(taskEntity) = Task(
		taskDefinition.name,
		taskDefinition.playerCharacter,
		taskDefinition.activateOnStartup,
		taskDefinition.activateOnStartupRound
	);

	spdlog::debug("Loading task {}", _gid->getString(taskDefinition.gid));
}

void ObjectCollection::loadWaypoint(const AWE::Object &container) {
	const auto wayPoint = std::any_cast<AWE::Templates::Waypoint>(container);

	auto wayPointEntity = _registry.create();
	_registry.emplace<GID>(wayPointEntity) = wayPoint.gid;
	_registry.emplace<Transform>(wayPointEntity) = Transform(wayPoint.position, wayPoint.rotation);

	spdlog::debug("Loading way point {}", _gid->getString(wayPoint.gid));
}

void ObjectCollection::loadSound(const AWE::Object &container) {
	const auto sound = std::any_cast<AWE::Templates::Sound>(container);

	auto soundEntity = _registry.create();
	_registry.emplace<GID>(soundEntity) = sound.gid;
	// TODO

	spdlog::debug("Loading sound {}", _gid->getString(sound.gid));
}

void ObjectCollection::loadTrigger(const AWE::Object &container) {
	const auto trigger = std::any_cast<AWE::Templates::Trigger>(container);

	auto triggerEntity = _registry.create();
	_registry.emplace<GID>(triggerEntity) = trigger.gid;

	_entities.emplace_back(triggerEntity);

	spdlog::debug("Loading trigger {}", _gid->getString(trigger.gid));
}

void ObjectCollection::loadCharacterClass(const AWE::Object &container) {
	const auto characterClass = std::any_cast<AWE::Templates::CharacterClass>(container);

	auto characterClassEntity = _registry.create();
	_registry.emplace<GID>(characterClassEntity) = characterClass.gid;
	_registry.emplace<AWE::Templates::CharacterClass>(characterClassEntity) = characterClass;

	_entities.emplace_back(characterClassEntity);

	spdlog::debug("Loading character class {}", _gid->getString(characterClass.gid));
}

void ObjectCollection::loadKeyFramedObject(const AWE::Object &container) {
	const auto keyFramedObject = std::any_cast<AWE::Templates::KeyFramedObject>(container);

	auto keyFramedObjectEntity = _registry.create();
	_registry.emplace<GID>(keyFramedObjectEntity) = keyFramedObject.gid;
	_registry.emplace<Transform>(keyFramedObjectEntity) = Transform(keyFramedObject.position2, keyFramedObject.rotation2);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(keyFramedObjectEntity) = std::make_shared<Graphics::Model>(keyFramedObject.meshResource);
	// TODO: Physics Resource

	model->getPosition() = keyFramedObject.position2;
	model->getRotation() = keyFramedObject.rotation2;


	_entities.emplace_back(keyFramedObjectEntity);

	spdlog::debug("Loading dynamic object {}", _gid->getString(keyFramedObject.gid));
}
