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

#include <spdlog/spdlog.h>

#include "common/convexshape.h"
#include "src/common/exception.h"

#include "awe/cidfile.h"
#include "awe/dpfile.h"
#include "awe/foliagedatafile.h"
#include "awe/object.h"

#include "src/graphics/model.h"
#include "src/graphics/meshman.h"
#include "src/graphics/light.h"
#include "src/graphics/skeleton.h"
#include "src/graphics/animation.h"
#include "src/graphics/animationcontroller.h"

#include "src/physics/rigidbody.h"
#include "src/physics/terraincollision.h"
#include "src/physics/charactercontroller.h"

#include "src/sound/audiostreamfactory.h"

#include "objectcollection.h"

#include "src/transform.h"
#include "src/task.h"
#include "src/utils.h"
#include "src/keyframer.h"
#include "src/aiprocess.h"
#include "src/character.h"

ObjectCollection::ObjectCollection(entt::registry &registry, entt::scheduler<double> &scheduler) : _registry(registry), _scheduler(scheduler) {
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
	for (size_t i = 0; i < foliageDataFile.getFoliages().size(); i++) {
		const auto &foliage = foliageDataFile.getFoliages()[i];
		std::vector<glm::vec3> positions;

		entt::entity foliageEntity = _registry.create();
		Graphics::ModelPtr model
			= _registry.emplace<Graphics::ModelPtr>(foliageEntity)
			= std::make_shared<Graphics::Model>(foliage);

		Common::BoundSphere meshBoundSphere = model->getBoundSphere();

		for (const auto &instance: foliageDataFile.getInstances()) {
			if (instance.foliageId != i)
				continue;

			positions.emplace_back(instance.position);
		}
		
		Common::BoundSphere finalBoundSphere = {positions.front() + meshBoundSphere.position, meshBoundSphere.radius};
		for (const auto &position : positions) {
			finalBoundSphere = Common::combine(
				finalBoundSphere,
				{position + meshBoundSphere.position, meshBoundSphere.radius}
			);
		}

		model->setBoundSphere(finalBoundSphere);

		model->addModelUniform(Graphics::Material::Uniform("g_vPositions", positions));

		assert(positions.size() <= 512);

		model->setNumInstances(positions.size());
		model->setLabel("<Foliage>");
	}
}

void ObjectCollection::loadTerrainCollisions(Common::ReadStream *collisions) {
	AWE::COLLISIONSFile collisionsFile(*collisions);
	entt::entity collisionsEntity = _registry.create();
	_registry.emplace<Physics::CollisionObjectPtr>(collisionsEntity) = std::make_shared<Physics::TerrainCollision>(collisionsFile);

	_entities.emplace_back(collisionsEntity);
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
		case kCharacterScript: loadCharacterScript(container); break;
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
		case kKeyframer: loadKeyFramer(container); break;
		case kKeyframeAnimation: loadKeyFrameAnimation(container); break;
		case kKeyframe:	loadKeyFrame(container); break;
		case kAmbientLight:	loadAmbientLightInstance(container); break;
		case kPointLight: loadPointLight(container); break;
		case kWeapon: loadWeapon(container); break;
		case kAttachmentContainer: loadAttachmentContainer(container); break;
		default:
			break; // If the object is currently not addable to the collection, skip it.
	}
}

void ObjectCollection::loadSkeleton(const AWE::Object &container) {
	const auto skeleton = std::any_cast<AWE::Templates::Skeleton>(container);

	auto skeletonEntity = _registry.create();
	_registry.emplace<GID>(skeletonEntity) = skeleton.gid;
	_registry.emplace<Graphics::Skeleton>(skeletonEntity) = skeleton.rid;

	spdlog::debug("Loading skeleton {}", skeleton.name);
}

void ObjectCollection::loadAnimation(const AWE::Object &container) {
	const auto animation = std::any_cast<AWE::Templates::Animation>(container);

	auto animationEntity = _registry.create();
	_registry.emplace<GID>(animationEntity) = animation.gid;
	try {
		_registry.emplace<Graphics::AnimationPtr>(animationEntity) = std::make_shared<Graphics::Animation>(
			animation.rid,
			animation.name
		);
	} catch (Common::Exception &exception) {
		spdlog::error(exception.what());
	}

	_globalObjects[kAnimationID].emplace_back(animationEntity);

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
	auto transform = _registry.emplace<Transform>(staticObjectEntity) = Transform(staticObject.position, staticObject.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(staticObjectEntity) = std::make_shared<Graphics::Model>(staticObject.meshResource);
	model->setLabel("<Static Object>");

	model->setTransform(transform.getTransformation());


	if (staticObject.physicsResource) {
		try {
			Physics::CollisionObjectPtr collisionObject = std::make_shared<Physics::RigidBody>(
				staticObject.physicsResource
			);

			collisionObject->setTransform(staticObject.position, staticObject.rotation);
			collisionObject->setActive(true);

			_registry.emplace<Physics::CollisionObjectPtr>(staticObjectEntity) = collisionObject;
		} catch (Common::Exception &e) {
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
	auto &transform = _registry.emplace<Transform>(dynamicObjectEntity) = Transform(dynamicObject.position, dynamicObject.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(dynamicObjectEntity) = std::make_shared<Graphics::Model>(dynamicObject.meshResource);
	model->setLabel(dynamicObject.identifier);
	// TODO: Physics Resource

	model->setTransform(transform.getTransformation());

	if (!_localObjects[kAttachmentContainerID].empty()) {
		const auto attachmentContainer = _registry.get<AWE::Templates::AttachmentContainer>(
			_localObjects[kAttachmentContainerID][dynamicObject.attachmentContainer.getID()]
		);

		applyAttachmentContainer(
			dynamicObjectEntity,
			attachmentContainer
		);
	}

	_entities.emplace_back(dynamicObjectEntity);

	spdlog::debug("Loading dynamic object {}", _gid->getString(dynamicObject.gid));
}

void ObjectCollection::loadDynamicObjectScript(const AWE::Object &container) {
	const auto dynamicObjectScript = std::any_cast<AWE::Templates::DynamicObjectScript>(container);

	const entt::entity scriptEntity = getEntityByGID(_registry, dynamicObjectScript.gid);
	if (scriptEntity == entt::null) {
		spdlog::error(
			"Couldn't find entity {}:{:x} for script, skipping it",
			dynamicObjectScript.gid.type,
			dynamicObjectScript.gid.id
		);
		return;
	}

	AWE::Script::BytecodePtr bytecode;
	AWE::Script::VariableStorePtr variableStore;
	_bytecode->createScript(
		dynamicObjectScript.script,
		bytecode,
		variableStore
	);

	if (bytecode)
		_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = bytecode;
	if (variableStore)
		_registry.emplace<AWE::Script::VariableStorePtr>(scriptEntity) = variableStore;

	spdlog::debug("Loading script for dynamic object {}", _gid->getString(dynamicObjectScript.gid));
}

void ObjectCollection::loadCharacter(const AWE::Object &container) {
	const auto character = std::any_cast<AWE::Templates::Character>(container);

	auto characterEntity = _registry.create();
	_registry.emplace<GID>(characterEntity) = character.gid;
	auto &transform = _registry.emplace<Transform>(characterEntity) = Transform(character.position, character.rotation);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(characterEntity) = std::make_shared<Graphics::Model>(character.meshResource);
	model->setLabel(character.identifier);
	// TODO: Physics and Cloth Resource

	model->setTransform(transform.getTransformation());

	const auto characterClass = _registry.get<AWE::Templates::CharacterClass>(getEntityByGID(_registry, character.classGid));

	const auto skeletonEntity = getEntityByGID(_registry, characterClass.skeletonGid);
	model->setSkeleton(_registry.get<Graphics::Skeleton>(skeletonEntity));

	Graphics::AnimationControllerPtr animationController
		= _registry.emplace<Graphics::AnimationControllerPtr>(characterEntity)
		= std::make_shared<Graphics::AnimationController>(
			const_cast<Graphics::Skeleton &>(model->getSkeleton()),
			characterClass.animationParameters.animationBlendTime
		);

	auto animationView = _registry.view<Graphics::AnimationPtr>();
	for (const auto &animation: characterClass.animations) {
		const auto animationEntity = _globalObjects[kAnimationID][animation.getID()];
		const auto anim = _registry.try_get<Graphics::AnimationPtr>(animationEntity);
		if (!anim)
			continue;

		animationController->addAnimation(*anim);
		spdlog::debug("Add animation {}", (*anim)->getName());
	}

	Physics::CharacterControllerPtr characterController
		= _registry.emplace<Physics::CharacterControllerPtr>(characterEntity)
		= std::make_shared<Physics::CharacterController>(
		characterClass.capsuleHeight,
		characterClass.capsuleRadius
	);
	characterController->setTransform(character.position, character.rotation);

	_registry.emplace<AI>(characterEntity);
	_scheduler.attach<AIProcess>(_registry, characterEntity);

	_entities.emplace_back(characterEntity);

	spdlog::debug("Loading character {}", _gid->getString(character.gid));
}

void ObjectCollection::loadCharacterScript(const AWE::Object &container) {
	const auto characterScript = std::any_cast<AWE::Templates::CharacterScript>(container);

	entt::entity scriptEntity = getEntityByGID(_registry, characterScript.gid);
	if (scriptEntity == entt::null) {
		spdlog::error(
			"Couldn't find character {}:{:x} for script, skipping it",
			characterScript.gid.type,
			characterScript.gid.id
		);
		return;
	}

	AWE::Script::BytecodePtr bytecode;
	AWE::Script::VariableStorePtr variableStore;
	_bytecode->createScript(
		characterScript.script,
		bytecode,
		variableStore
	);

	if (bytecode)
		_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = bytecode;
	if (variableStore)
		_registry.emplace<AWE::Script::VariableStorePtr>(scriptEntity) = variableStore;

	spdlog::debug("Loading script for object {}", _gid->getString(characterScript.gid));
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
	if (scriptEntity == entt::null) {
		spdlog::error(
			"Couldn't find entity {}:{:x} for script, skipping it",
			scriptInstanceScript.gid.type,
			scriptInstanceScript.gid.id
		);
		return;
	}

	AWE::Script::BytecodePtr bytecode;
	AWE::Script::VariableStorePtr variableStore;
		_bytecode->createScript(
		scriptInstanceScript.script,
		bytecode,
		variableStore
	);

	if (bytecode)
		_registry.emplace<AWE::Script::BytecodePtr>(scriptEntity) = bytecode;
	if (variableStore)
		_registry.emplace<AWE::Script::VariableStorePtr>(scriptEntity) = variableStore;

	spdlog::debug("Loading script for object {}", _gid->getString(scriptInstanceScript.gid));
}

void ObjectCollection::loadFloatingScript(const AWE::Object &container) {
	const auto floatingScript = std::any_cast<AWE::Templates::FloatingScript>(container);

	auto floatingScriptEntity = _registry.create();
	_registry.emplace<GID>(floatingScriptEntity) = floatingScript.gid;
	_registry.emplace<Transform>(floatingScriptEntity) = Transform(floatingScript.position, floatingScript.rotation);

	AWE::Script::BytecodePtr bytecode;
	AWE::Script::VariableStorePtr variableStore;
		_bytecode->createScript(
		floatingScript.script,
		bytecode,
		variableStore
	);

	if (bytecode)
		_registry.emplace<AWE::Script::BytecodePtr>(floatingScriptEntity) = bytecode;
	if (variableStore)
		_registry.emplace<AWE::Script::VariableStorePtr>(floatingScriptEntity) = variableStore;

	spdlog::debug("Loading floating script {}", _gid->getString(floatingScript.gid));
}

void ObjectCollection::loadPointLight(const AWE::Object &container) {
	const auto pointLight = std::any_cast<AWE::Templates::PointLight>(container);

	auto pointLightEntity = _registry.create();
	_registry.emplace<GID>(pointLightEntity) = pointLight.gid;
	const auto &transform = _registry.emplace<Transform>(pointLightEntity) = Transform(pointLight.position, pointLight.rotation);
	auto &light = _registry.emplace<Graphics::Light>(pointLightEntity);

	light.setColor(pointLight.color);
	light.setIntensity(pointLight.intensity);
	light.setDecay(pointLight.decay);
	light.setDirectionalFalloff(pointLight.directionalFalloff);
	light.setTransform(transform.getTransformation());
	if (pointLight.enableRangeClip)
		light.setRangeClip(pointLight.rangeClip);
	light.setLabel(_gid->getString(pointLight.gid));
	light.show();

	if (pointLight.meshRid) {
		const auto pointLightMeshEntity = _registry.create();

		// Make pointlight mesh a child of the pointlight
		_registry.emplace<Relationship>(pointLightMeshEntity) = {pointLightEntity, {}};
		_registry.get_or_emplace<Relationship>(pointLightEntity).children.emplace_back(pointLightMeshEntity);

		// Create pointlight mesh transform
		auto &meshTransform
			= _registry.emplace<Transform>(pointLightMeshEntity)
			= Transform(
			pointLight.meshPosition,
			pointLight.meshRotation
		);
		meshTransform.setParentTransform(transform.getTransformation());

		Graphics::ModelPtr model
			= _registry.emplace<Graphics::ModelPtr>(pointLightMeshEntity)
			= std::make_shared<Graphics::Model>(pointLight.meshRid);

		model->setTransform(meshTransform.getTransformation());
		model->setLabel(fmt::format("{}::mesh", _gid->getString(pointLight.gid)));

		_entities.emplace_back(pointLightMeshEntity);
	}

	_attachmentMappings[pointLight.attachmentGid] = pointLight.gid;

	spdlog::debug("Loading point light {}", _gid->getString(pointLight.gid));
}

void ObjectCollection::loadAmbientLightInstance(const AWE::Object &container) {
	const auto ambientLightInstance = std::any_cast<AWE::Templates::AmbientLightInstance>(container);

	auto ambientLightEntity = _registry.create();
	_registry.emplace<GID>(ambientLightEntity) = ambientLightInstance.gid;
	_registry.emplace<Transform>(ambientLightEntity) = Transform(ambientLightInstance.position, glm::identity<glm::mat3>());

	spdlog::debug("Loading ambient light {}", _gid->getString(ambientLightInstance.gid));
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
	_registry.emplace<Sound::AudioStreamFactory>(soundEntity) = sound.rid;

	spdlog::debug("Loading sound {}", _gid->getString(sound.gid));
}

void ObjectCollection::loadTrigger(const AWE::Object &container) {
	const auto trigger = std::any_cast<AWE::Templates::Trigger>(container);

	auto triggerEntity = _registry.create();
	_registry.emplace<GID>(triggerEntity) = trigger.gid;

	_entities.emplace_back(triggerEntity);

	_attachmentMappings[trigger.attachmentGid] = trigger.gid;

	spdlog::debug("Loading trigger {}", _gid->getString(trigger.gid));
}

void ObjectCollection::loadCharacterClass(const AWE::Object &container) {
	const auto characterClass = std::any_cast<AWE::Templates::CharacterClass>(container);

	auto characterClassEntity = _registry.create();
	_registry.emplace<GID>(characterClassEntity) = characterClass.gid;
	_registry.emplace<AWE::Templates::CharacterClass>(characterClassEntity) = characterClass;

	for (const auto &animation : characterClass.animations) {
		const auto animationEntity = _globalObjects[kAnimationID][animation.getID()];
	}

	_entities.emplace_back(characterClassEntity);

	spdlog::debug("Loading character class {}", _gid->getString(characterClass.gid));
}

void ObjectCollection::loadKeyFramedObject(const AWE::Object &container) {
	const auto keyFramedObject = std::any_cast<AWE::Templates::KeyFramedObject>(container);

	auto keyFramedObjectEntity = _registry.create();
	_registry.emplace<GID>(keyFramedObjectEntity) = keyFramedObject.gid;
	auto &transform = _registry.emplace<Transform>(keyFramedObjectEntity) = Transform(keyFramedObject.position2, keyFramedObject.rotation2);
	Graphics::ModelPtr model = _registry.emplace<Graphics::ModelPtr>(keyFramedObjectEntity) = std::make_shared<Graphics::Model>(keyFramedObject.meshResource);
	// TODO: Physics Resource

	model->setTransform(transform.getTransformation());
    model->setLabel(_gid->getString(keyFramedObject.gid));

	const auto &keyFramer = _registry.get<KeyFramerPtr>(
		_localObjects[kKeyframerID][keyFramedObject.keyFramer.getID()]
	);
	const auto &keyFramerEntity = _localObjects[kKeyframerID][keyFramedObject.keyFramer.getID()];

	transform.setKeyFrameOffset(keyFramedObject.position, keyFramedObject.rotation);

	keyFramer->addAffectedEntity(keyFramedObjectEntity);

	_entities.emplace_back(keyFramedObjectEntity);

	spdlog::debug("Loading key framed object {}", _gid->getString(keyFramedObject.gid));
}

void ObjectCollection::loadKeyFramer(const AWE::Object &container) {
	const auto keyFramer = std::any_cast<AWE::Templates::KeyFramer>(container);

	auto keyFramerEntity = _registry.create();
	_registry.emplace<GID>(keyFramerEntity) = keyFramer.gid;

	std::vector<KeyFrame> keyFrames;
	std::map<GID, KeyFrameAnimation> keyFrameAnimations;
	for (const auto &keyFrame : keyFramer.keyFrames) {
		keyFrames.emplace_back(_registry.get<KeyFrame>(_localObjects[kKeyframeID][keyFrame.getID()]));
	}
	for (const auto &keyFrameAnimation : keyFramer.keyFrameAnimations) {
		const auto keyFrameAnimationEntity = _localObjects[kKeyframeAnimationID][keyFrameAnimation.getID()];
		const auto gid = _registry.get<GID>(keyFrameAnimationEntity);
		keyFrameAnimations[gid] = _registry.get<KeyFrameAnimation>(keyFrameAnimationEntity);
	}

	auto keyframer = _registry.emplace<KeyFramerPtr>(keyFramerEntity) = std::make_shared<KeyFramer>(keyFrames, keyFrameAnimations, keyFramer.initialKeyframe);

	if (keyFramer.parentKeyFramer) {
		const auto parentKeyFramer = _registry.get<KeyFramerPtr>(_localObjects[kKeyframerID][keyFramer.parentKeyFramer.getID()]);
		keyframer->setParentKeyFramer(parentKeyFramer);
	}

	_localObjects[kKeyframerID].emplace_back(keyFramerEntity);

	spdlog::debug("Loading keyframer {}", _gid->getString(keyFramer.gid));
}

void ObjectCollection::loadAttachmentContainer(const AWE::Object &container) {
	const auto attachmentContainer = std::any_cast<AWE::Templates::AttachmentContainer>(container);

	const auto attachmentContainerEntity = _registry.create();
	_registry.emplace<AWE::Templates::AttachmentContainer>(attachmentContainerEntity) = attachmentContainer;

	_localObjects[kAttachmentContainerID].emplace_back(attachmentContainerEntity);

	_entities.emplace_back(attachmentContainerEntity);
}

void ObjectCollection::applyAttachmentContainer(
		const entt::entity &parent,
		const AWE::Templates::AttachmentContainer &attachmentContainer
) {
	const auto transform = _registry.get<Transform>(parent);
	auto &relationship = _registry.get_or_emplace<Relationship>(parent);

	for (const auto &pointLight: attachmentContainer.pointLights) {
		if (_attachmentMappings.find(pointLight) == _attachmentMappings.end())
			throw CreateException("Couldn't find point light {}:{:x}", pointLight.type, pointLight.id);

		const auto pointlightEntity = getEntityByGID(_registry, _attachmentMappings.at(pointLight));
		relationship.children.emplace_back(pointlightEntity);

		_registry.get<Transform>(pointlightEntity).setParentTransform(transform.getTransformation());
		_registry.get<Graphics::Light>(pointlightEntity).setTransform(
				_registry.get<Transform>(pointlightEntity).getTransformation()
		);

		auto childRelationship = _registry.get_or_emplace<Relationship>(parent);
		assert(childRelationship.parent == entt::null);
		childRelationship.parent = parent;
	}

	// TODO: Attach more entity types
}

void ObjectCollection::loadKeyFrameAnimation(const AWE::Object &container) {
	const auto keyFrameAnimation = std::any_cast<AWE::Templates::KeyFrameAnimation>(container);

	auto keyFrameAnimationEntity = _registry.create();
	_registry.emplace<GID>(keyFrameAnimationEntity) = keyFrameAnimation.gid;

	KeyFrameAnimation keyFrameAnimationObject {
		keyFrameAnimation.startKeyFrame,
		keyFrameAnimation.endKeyFrame,
		keyFrameAnimation.length
	};

	try {
		if (keyFrameAnimation.animationResource)
			keyFrameAnimationObject.animation = std::make_shared<Graphics::Animation>(keyFrameAnimation.animationResource);
	} catch (Common::Exception &e) {
		spdlog::error("Failed to load keyframe animation: {}", e.what());
		return;
	}

	if (!keyFrameAnimation.nextAnimation.isNil())
		keyFrameAnimationObject.nextAnimation =	keyFrameAnimation.nextAnimation;

	_registry.emplace<KeyFrameAnimation>(keyFrameAnimationEntity) = keyFrameAnimationObject;

	_localObjects[kKeyframeAnimationID].emplace_back(keyFrameAnimationEntity);

	spdlog::debug("Loading keyframe animation {}", _gid->getString(keyFrameAnimation.gid));
}

void ObjectCollection::loadKeyFrame(const AWE::Object &container) {
	const auto keyFrame = std::any_cast<AWE::Templates::KeyFrame>(container);

	KeyFrame keyFrameObject {
		keyFrame.position,
		keyFrame.rotation
	};

	auto keyFrameEntity = _registry.create();
	_registry.emplace<KeyFrame>(keyFrameEntity) = keyFrameObject;

	_localObjects[kKeyframeID].emplace_back(keyFrameEntity);

	_entities.emplace_back(keyFrameEntity);
}

void ObjectCollection::loadWeapon(const AWE::Object &container) {
	const auto weapon = std::any_cast<AWE::Templates::Weapon>(container);

	const auto weaponEntity = _registry.create();
	_registry.emplace<GID>(weaponEntity) = weapon.gid;
	_registry.emplace<Graphics::MeshPtr>(weaponEntity) = MeshMan.getMesh(weapon.meshResource);

	_entities.emplace_back(weaponEntity);

	spdlog::debug("Loading Weapon {}", _gid->getString(weapon.gid));
}

std::map<ObjectIDType, std::vector<entt::entity>> ObjectCollection::_globalObjects = std::map<ObjectIDType, std::vector<entt::entity>>();