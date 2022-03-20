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

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "src/common/strutil.h"
#include "src/common/exception.h"

#include "src/awe/objectstream.h"

static const uint32_t kDeadBeefV2 = 0xD34DB33F;

namespace AWE {

ObjectReadStream::ObjectReadStream() {
}

ObjectStream::~ObjectStream() {
}

void ObjectStream::resourceID(rid_t &rid) {
	variable("rid", rid, true);
}

void ObjectStream::boundBox(Common::BoundBox &aabb) {
	variable("xmin", aabb.xmin);
	variable("ymin", aabb.ymin);
	variable("zmin", aabb.zmin);
	variable("xmax", aabb.xmax);
	variable("ymax", aabb.ymax);
	variable("zmax", aabb.zmax);
}

void ObjectStream::staticObject(Templates::StaticObject &staticObject) {
	variable("rotation", staticObject.rotation);
	variable("position", staticObject.position);

	object("physicsResource", staticObject.physicsResource, kRID);
	skip(4);
	object("meshResource", staticObject.meshResource, kRID);
	skip(17);
}

void ObjectStream::dynamicObject(Templates::DynamicObject &dynamicObject, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 11
	 * Alan Wakes American Nightmare: 12
	 */
	variable("rotation", dynamicObject.rotation);
	variable("position", dynamicObject.position);

	object("physicsResource", dynamicObject.physicsResource, kRID);
	variable("resourcePath", dynamicObject.resourcePath, true);
	object("meshResource", dynamicObject.meshResource, kRID);
	variable("identifier", dynamicObject.identifier, true);

	uint32_t unknown1, unknown2, unknown3;
	variable("", unknown1);
	variable("", unknown2);
	variable("", unknown3);

	skip(4);
	//std::string name3 = _dp->getString(cid.readUint32LE());

	variable("gid", dynamicObject.gid);

	if (version == 12)
		skip(13);
	else // version == 11
		skip(9);
}

void ObjectStream::dynamicObjectScript(Templates::DynamicObjectScript &dynamicObjectScript, unsigned int version) {
	variable("gid", dynamicObjectScript.gid);
	object("scriptVariables", dynamicObjectScript.script, kScriptVariables);

	uint32_t value;
	variable("", value);
	skip(4);
}

void ObjectStream::cellInfo(Templates::CellInfo &cellInfo) {
	variable("x", cellInfo.x);
	variable("y", cellInfo.y);

	variable("lowDetailFoliageCount", cellInfo.lowDetailFoliageCount);
	variable("highDetailFoliageCount", cellInfo.highDetailFoliageCount);
}

void ObjectStream::animation(Templates::Animation &animation, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 17
	 * Alan Wakes American Nightmare: 19
	 */
	variable("gid", animation.gid);
	variable("skeletonGid", animation.skeletonGid);
	variable("id", animation.id);

	object<rid_t>("animationResource", animation.rid, kRID);

	if (version == 17)
		skip(1);

	variable("name", animation.name, true);

	variable("useFingersLeft", animation.useFingersLeft);
	variable("useFingersRight", animation.useFingersRight);
	variable("useFootIK", animation.useFootIK);
	variable("attachLeftHand", animation.attachLeftHand);
	variable("legSyncLoopCount", animation.legSyncLoopCount);
	variable("scriptedBlendIn", animation.scriptedBlendIn);
	variable("scriptedBlendInTime", animation.scriptedBlendInTime);
	variable("scriptedBlendOut", animation.scriptedBlendOut);
	variable("scriptedMoveCapsule", animation.scriptedMoveCapsule);
}

void ObjectStream::skeleton(Templates::Skeleton &skeleton) {
	variable("gid", skeleton.gid);
	variable("name", skeleton.name, true);

	object("resource", skeleton.rid, kRID);

	variable("id", skeleton.id);
}

void ObjectStream::skeletonSetup(Templates::SkeletonSetup &skeletonSetup) {
	variable("rootBoneGid", skeletonSetup.rootBoneGid);
	variable("identifier", skeletonSetup.identifier, true);

	skip(7);
}

void ObjectStream::notebookPage(Templates::NotebookPage &notebookPage) {
	variable("gid", notebookPage.gid);
	variable("name", notebookPage.name, true);

	// Probably GID?
	skip(8);

	variable("episodeNumber", notebookPage.episodeNumber);
	variable("id", notebookPage.id);
	variable("onlyInNightmare", notebookPage.onlyInNightmare);
}

void ObjectStream::sound(Templates::Sound &sound) {
	variable("gid", sound.gid);
	variable("threed", sound.threed);
	variable("streamed", sound.streamed);
	variable("looping", sound.looping);
	variable("volume", sound.volume);
	variable("hotspot", sound.hotspot);
	variable("falloff", sound.falloff);
	variable("volumeVariation", sound.volumeVariation);
	variable("frequencyVariation", sound.frequencyVariation);

	skip(0x26);

	object("resource", sound.rid, kRID);

	skip(7);
}

void ObjectStream::character(Templates::Character &character, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 13
	 * Alan Wakes American Nightmare: 17
	 */
	variable("gid", character.gid);
	variable("classGid", character.classGid);

	if (version == 13)
		skip(1);

	// Mesh
	object("meshResource", character.meshResource, kRID);

	variable("rotation", character.rotation);
	variable("position", character.position);

	objects("resources", character.resources, kRID);

	if (version == 17) {
		skip(4);

		variable("identifier", character.identifier, false);

		object("clothResource", character.clothResource, kRID);

		// TODO: Cloth Parameters
		skip(48);

		// FaceFX
		object("fxaResource", character.fxaResource, kRID);

		skip(1);

		// Animgraphs
		object("animgraphResource", character.animgraphResource, kRID);

		skip(9);

		// Additional resources
		rid_t resource1, resource2, resource3, resource4;
		object("", resource1, kRID);
		object("", resource2, kRID);
		object("", resource3, kRID);
		object("", resource4, kRID);
	} else { // Version 13
		skip(0x3A);
	}
}

void ObjectStream::characterScript(Templates::CharacterScript &characterScript) {
	variable("gid", characterScript.gid);
	object("script", characterScript.script, kScriptVariables);

	skip(8); // Always 0?
}

void ObjectStream::characterClass(Templates::CharacterClass &characterClass, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 38
	 * Alan Wakes American Nightmare: 42
	 */
	variable("gid", characterClass.gid);
	variable("name", characterClass.name, true);

	if (version == 42)
		variable("baseClasses", characterClass.baseClasses);
	else // version == 38
		variable("baseClasses", characterClass.baseClasses, 4);

	variable("skeletonGid", characterClass.skeletonGid);
	if (version == 42) {
		variable("parentName", characterClass.parentName, true);
		variable("capsuleHeight", characterClass.capsuleHeight);
		variable("capsuleRadius", characterClass.capsuleRadius);
		variable("lethalDoseOfHitEnergy", characterClass.lethalDoseOfHitEnergy);
		variable("healthRecoveryStartDelay", characterClass.healthRecoveryStartDelay);
		variable("healthRecoveryTime", characterClass.healthRecoveryTime);
		variable("shadowShieldStrength", characterClass.shadowShieldStrength);
	}

	variable("strongShield", characterClass.strongShield);
	variable("kickbackMultiplier", characterClass.kickbackMultiplier);
	variable("timeBetweenDazzles", characterClass.timeBetweenDazzles);

	if (version == 42) {
		variable("animations", characterClass.animations);
		object("animationParameters", characterClass.animationParameters, kAnimationParameters);
		variable("type", characterClass.type, true);
		skip(8); // Arcade Score, Arcade Multiplier
	} else {
		skip(0x49);
	}
}

void ObjectStream::taskDefinition(Templates::TaskDefinition &taskDefinition, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 11
	 * Alan Wakes American Nightmare: 15
	 */
	variable("name", taskDefinition.name, true);

	std::vector<uint32_t> vals;
	variable("", vals);

	skip(8); // Another offset and count into the dp file

	variable("rootTask", taskDefinition.rootTask);
	variable("topLevelTask", taskDefinition.topLevelTask);

	variable("rotation", taskDefinition.rotation);
	variable("position", taskDefinition.position);

	variable("activateOnStartup", taskDefinition.activateOnStartup);
	if (version == 15) {
		variable("activateOnStartupRound", taskDefinition.activateOnStartupRound, 3);
	}
	bool gidlessTask = false;
	variable("", gidlessTask);

	variable("gid", taskDefinition.gid);

	bool b2 = false; // If it is a non zero position?
	variable("", b2);

	if (version == 15) {
		variable("rotationPlayer", taskDefinition.rotationPlayer);
		variable("positionPlayer", taskDefinition.positionPlayer);

		taskDefinition.playerCharacter.resize(3);
		variable("playerCharacter1", taskDefinition.playerCharacter[0]);
		skip(8);
		variable("cinematic", taskDefinition.cinematic, true);
		variable("playerCharacter2", taskDefinition.playerCharacter[1]);
		variable("playerCharacter3", taskDefinition.playerCharacter[2]);
	} else {
		skip(0x44);
	}
}

void ObjectStream::scriptVariables(Templates::ScriptVariables &script, unsigned int version) {
	variable("codeSize", script.codeSize);
	variable("offsetCode", script.offsetCode);
	variable("numHandlers", script.numHandlers);
	variable("offsetHandlers", script.offsetHandlers);
	variable("numVariables", script.numVariables);
	variable("offsetVariables", script.offsetVariables);
	variable("numSignals", script.numSignals);
	variable("offsetSignals", script.offsetSignals);

	if (version >= 2) {
		variable("numDebugEntries", script.numDebugEntries);
		variable("offsetDebugEntries", script.offsetDebugEntries);
	}
}

void ObjectStream::script(Templates::Script &script) {
	variable("gid", script.gid);
	object("script", script.script, kScriptVariables);
}

void ObjectStream::scriptInstance(Templates::ScriptInstance &scriptInstance) {
	variable("gid2", scriptInstance.gid2);
	variable("gid", scriptInstance.gid);

	variable("rotation", scriptInstance.rotation);
	variable("position", scriptInstance.position);
}

void ObjectStream::pointLight(Templates::PointLight &pointLight, unsigned int version) {
	/*
     * Versions:
     * Alan Wake: 11
     * Alan Wakes American Nightmare: 13
     */
	variable("gid", pointLight.gid);

	if (version == 13) {
		variable("gid2", pointLight.gid);

		variable("rotation", pointLight.rotation);
		variable("position", pointLight.position);
		variable("color", pointLight.color);
		variable("decay", pointLight.decay);
		variable("directionalFalloff", pointLight.directionalFalloff);
		variable("autoStart", pointLight.autoStart);
		variable("castShadows", pointLight.castShadows);
		variable("intensity", pointLight.intensity);

		object("meshResource", pointLight.meshRid, kRID);
		object("staticShadowMapRsource", pointLight.staticShadowMapRid, kRID);

		variable("meshRotation", pointLight.meshRotation);
		variable("meshPosition", pointLight.meshPosition);

		variable("drainMultiplier", pointLight.drainMultiplier);
		variable("enableSpecular", pointLight.enableSpecular);
		variable("shadowMapRange", pointLight.shadowMapRange);
		variable("enableRangeClip", pointLight.enableRangeClip);
		variable("rangeClip", pointLight.rangeClip);

		skip(12);
	} else { // Version 11
		skip(0x94);
	}
}

void ObjectStream::ambientLightInstance(Templates::AmbientLightInstance &ambientLightInstance) {
	variable("scriptGid", ambientLightInstance.scriptGid);
	variable("gid", ambientLightInstance.gid);
	variable("position", ambientLightInstance.position);

	variable("color", ambientLightInstance.color);

	variable("decay", ambientLightInstance.decay);
	variable("autoStart", ambientLightInstance.autoStart);
	variable("intensity", ambientLightInstance.intensity);
}

void ObjectStream::floatingScript(Templates::FloatingScript &floatingScript) {
	variable("gid", floatingScript.gid);
	object("script", floatingScript.script, kScriptVariables);
	variable("rotation", floatingScript.rotation);
	variable("position", floatingScript.position);
}

void ObjectStream::trigger(Templates::Trigger &trigger, unsigned int version) {
	/*
     * Versions:
     * Alan Wake: 18
     * Alan Wakes American Nightmare: 20
     */
	variable("gid2", trigger.gid2);
	variable("gid", trigger.gid);

	skip(4); // Priority?

	variable("identifier", trigger.identifier, true);

	skip(4);

	variable("localeString", trigger.localeString, true);

	skip(12);
	std::vector<int32_t> values;
	variable("", values);

	if (version == 20)
		skip(7);
	else
		skip(3);
}

void ObjectStream::areaTrigger(Templates::AreaTrigger &areaTrigger) {
	variable("gid", areaTrigger.gid);

	uint32_t value1;
	variable("", value1);

	variable("identifier", areaTrigger.identifier, true);
	variable("positions", areaTrigger.positions);

	skip(32);
}

void ObjectStream::taskContent(Templates::TaskContent &taskContent) {
	skip(12);

	// List of rids
	variable("resources", taskContent.rids);

	// List of gids + 8byte padding
	skip(8);

	// Unknown container
	Templates::AttachmentResource attachmentResource;
	object("", attachmentResource, kAttachmentResources);

	std::vector<uint32_t> values;
	variable("", values, false);
}

void ObjectStream::attachmentResources(Templates::AttachmentResource &attachmentResource) {
	// TODO
	skip(24);
}

void ObjectStream::waypoint(Templates::Waypoint &waypoint) {
	variable("gid", waypoint.gid);
	variable("rotation", waypoint.rotation);
	variable("position", waypoint.position);
}

void ObjectStream::animationParameters(Templates::AnimationParameters &animationParameters) {
	variable("animationBlendTime", animationParameters.animationBlendTime);
	variable("halfRotationTime", animationParameters.halfRotationTime);
	variable("tiltGain", animationParameters.tiltGain);
	variable("tiltRegression", animationParameters.tiltRegression);
	variable("tiltAngleRadians", animationParameters.tiltAngleRadians);
	variable("tiltAgility", animationParameters.tiltAgility);
	variable("tiltScaleForward", animationParameters.tiltScaleForward);
	variable("tiltScaleBackwards", animationParameters.tiltScaleBackwards);
	variable("animationProfile", animationParameters.animationProfile);
}

void ObjectStream::keyFramedObject(Templates::KeyFramedObject &keyFramedObject, unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 4
	 * Alan Wakes American Nightmare: 5
	 */
	variable("rotation", keyFramedObject.rotation);
	variable("position", keyFramedObject.position);

	object("physicsResource", keyFramedObject.physicsResource, kRID);
	variable("source", keyFramedObject.source, true);
	object("meshResource", keyFramedObject.meshResource, kRID);
	variable("name", keyFramedObject.name, true);
	skip(8);
	variable("resources", keyFramedObject.resources);
	variable("gid", keyFramedObject.gid);

	variable("keyFramer", keyFramedObject.keyFramer);
	skip(5);

	if (version >= 5) {
		variable("rotation2", keyFramedObject.rotation2);
		variable("position2", keyFramedObject.position2);
	} else {
		keyFramedObject.rotation2 = glm::identity<glm::mat3>();
		keyFramedObject.position2 = glm::zero<glm::vec3>();
	}
}

void ObjectStream::keyFramer(Templates::KeyFramer &keyFramer) {
	variable("gid", keyFramer.gid);
	variable("keyFrames", keyFramer.keyFrames);
	variable("keyFrameAnimations", keyFramer.keyFrameAnimations);

	variable("parentKeyFramer", keyFramer.parentKeyFramer);
	variable("initialKeyframe", keyFramer.initialKeyframe);

	ObjectID oid;
	variable("", oid);

	variable("resources", keyFramer.resources);
	bool val1 = false;
	variable("", val1);
}

void ObjectStream::keyFrameAnimation(Templates::KeyFrameAnimation &keyFrameAnimation) {
	variable("gid", keyFrameAnimation.gid);
	variable("startKeyFrame", keyFrameAnimation.startKeyFrame);
	variable("endKeyFrame", keyFrameAnimation.endKeyFrame);
	variable("length", keyFrameAnimation.length);

	// Unknown values
	std::vector<uint32_t> vals1;
	std::vector<float> vals2, vals3;

	variable("", vals1);
	variable("", vals2);
	variable("", vals3);

	object("animationResource", keyFrameAnimation.animationResource, kRID);
	skip(4);
	variable("nextAnimation", keyFrameAnimation.nextAnimation);
}

void ObjectStream::keyFrame(Templates::KeyFrame &keyFrame) {
	variable("position", keyFrame.position);
	variable("rotation", keyFrame.rotation);
}

void ObjectStream::gameEvent(Templates::GameEvent &gameEvent) {
	object("script", gameEvent.scriptVariables, kScriptVariables);

	variable("gid", gameEvent.gid);
	variable("name", gameEvent.name, true);
	skip(8);
}

void ObjectStream::readFileInfoMetadata(Templates::FileInfoMetadata &fileInfoMetadata) {
	variable("fileSize", fileInfoMetadata.fileSize);
	variable("fileDataCRC", fileInfoMetadata.fileDataCRC);
	variable("flags", fileInfoMetadata.flags);
}

void ObjectStream::foliageMeshMetadata(Templates::FoliageMeshMetadata foliageMeshMetadata) {
	variable("vertexBufferBytes", foliageMeshMetadata.vertexBufferBytes);
	variable("indexCount", foliageMeshMetadata.indexCount);

	object("boundBox", foliageMeshMetadata.boundBox, kAABB);

	// Does this work?
	objects("textureResources", foliageMeshMetadata.textureRids, kRID);
}

void ObjectStream::havokAnimationMetadata(Templates::HavokAnimationMetadata &havokAnimationMetadata) {
	variable("animationEventPath", havokAnimationMetadata.animationEventPath, false);
}

void ObjectStream::readMeshMetadata(Templates::MeshMetadata &meshMetadata) {
	variable("vertexBufferBytes", meshMetadata.vertexBufferBytes);
	variable("indexCount", meshMetadata.indexCount);
	object("boundBox", meshMetadata.boundBox, kAABB);
	variable("hasBones", meshMetadata.hasBones);
	objects("textureResources", meshMetadata.textureRids, kRID);
}

void ObjectStream::readParticleSystemMetadata(Templates::ParticleSystemMetadata particleSystemMetadata) {
	objects("textureResources", particleSystemMetadata.textureRids, kRID);
}

void ObjectStream::textureMetadata(Templates::TextureMetadata &textureMetadata) {
	variable("type", textureMetadata.type);
	variable("format", textureMetadata.format);
	variable("filter", textureMetadata.format);
	variable("width", textureMetadata.width);
	variable("height", textureMetadata.height);
	variable("depth", textureMetadata.depth);

	skip(4);

	variable("mipmapOffsets", textureMetadata.mipmapOffsets, 8lu);

	variable("highDetailStreamDistance", textureMetadata.highDetailStreamDistance);
	variable("useTextureLOD", textureMetadata.useTextureLOD);
}

void ObjectStream::object(Object &value, ObjectType type, unsigned int version) {
	switch (type) {
		case kRID: resourceID(as<rid_t>(value)); break;
		case kAABB: boundBox(as<Common::BoundBox>(value)); break;
		case kStaticObject: staticObject(as<Templates::StaticObject>(value)); break;
		case kDynamicObject: dynamicObject(as<Templates::DynamicObject>(value), version); break;
		case kDynamicObjectScript: dynamicObjectScript(as<Templates::DynamicObjectScript>(value), version); break;
		case kCellInfo: cellInfo(as<Templates::CellInfo>(value)); break;
		case kAnimation: animation(as<Templates::Animation>(value), version); break;
		case kSkeleton: skeleton(as<Templates::Skeleton>(value)); break;
		case kSkeletonSetup: skeletonSetup(as<Templates::SkeletonSetup>(value)); break;
		case kNotebookPage: notebookPage(as<Templates::NotebookPage>(value)); break;
		case kSound: sound(as<Templates::Sound>(value)); break;
		case kCharacter: character(as<Templates::Character>(value), version); break;
		case kCharacterClass: characterClass(as<Templates::CharacterClass>(value), version); break;
		case kCharacterScript: characterScript(as<Templates::CharacterScript>(value)); break;
		case kTaskDefinition: taskDefinition(as<Templates::TaskDefinition>(value), version); break;
		case kTaskContent: taskContent(as<Templates::TaskContent>(value)); break;
		case kScriptVariables: scriptVariables(as<Templates::ScriptVariables>(value), version); break;
		case kScript: script(as<Templates::Script>(value)); break;
		case kScriptInstance: scriptInstance(as<Templates::ScriptInstance>(value)); break;
		case kPointLight: pointLight(as<Templates::PointLight>(value), version); break;
		case kAmbientLight: ambientLightInstance(as<Templates::AmbientLightInstance>(value)); break;
		case kFloatingScript: floatingScript(as<Templates::FloatingScript>(value)); break;
		case kTrigger: trigger(as<Templates::Trigger>(value), version); break;
		case kAreaTrigger: areaTrigger(as<Templates::AreaTrigger>(value)); break;
		case kAttachmentResources: attachmentResources(as<Templates::AttachmentResource>(value)); break;
		case kWaypoint: waypoint(as<Templates::Waypoint>(value)); break;
		case kAnimationParameters: animationParameters(as<Templates::AnimationParameters>(value)); break;
		case kKeyframedObject: keyFramedObject(as<Templates::KeyFramedObject>(value), version); break;
		case kKeyframe: keyFrame(as<Templates::KeyFrame>(value)); break;
		case kKeyframeAnimation: keyFrameAnimation(as<Templates::KeyFrameAnimation>(value)); break;
		case kKeyframer: keyFramer(as<Templates::KeyFramer>(value)); break;
		case kGameEvent: gameEvent(as<Templates::GameEvent>(value)); break;

		case kFileInfoMetadata: readFileInfoMetadata(as<Templates::FileInfoMetadata>(value)); break;
		case kFoliageMeshMetadata: foliageMeshMetadata(as<Templates::FoliageMeshMetadata>(value)); break;
		case kHavokAnimationMetadata: havokAnimationMetadata(as<Templates::HavokAnimationMetadata>(value)); break;
		case kTextureMetadata: textureMetadata(as<Templates::TextureMetadata>(value)); break;
		case kParticleSystemMetadata: readParticleSystemMetadata(as<Templates::ParticleSystemMetadata>(value)); break;
		case kMeshMetadata: readMeshMetadata(as<Templates::MeshMetadata>(value)); break;

		default: throw Common::Exception("Unsupported content type");
	}
}

}
