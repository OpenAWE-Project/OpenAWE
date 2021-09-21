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

#include "src/common/strutil.h"

#include "src/awe/objectstream.h"

static const uint32_t kDeadBeefV2 = 0xD34DB33F;

namespace AWE {

ObjectReadStream::ObjectReadStream() {
}

ObjectBinaryReadStream::ObjectBinaryReadStream(Common::ReadStream &stream) : _stream(stream) {
}

ObjectBinaryReadStream::ObjectBinaryReadStream(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) : _stream(stream), _dp(dp) {
}

rid_t ObjectBinaryReadStream::readRID() {
	return _stream.readUint32BE();
}

Common::BoundBox ObjectBinaryReadStream::readAABB() {
	Common::BoundBox aabb{};

	aabb.xmin = _stream.readIEEEFloatLE();
	aabb.ymin = _stream.readIEEEFloatLE();
	aabb.zmin = _stream.readIEEEFloatLE();
	aabb.xmax = _stream.readIEEEFloatLE();
	aabb.ymax = _stream.readIEEEFloatLE();
	aabb.zmax = _stream.readIEEEFloatLE();

	return aabb;
}

Templates::StaticObject ObjectBinaryReadStream::readStaticObject() {
	Templates::StaticObject staticObject{};

	staticObject.rotation = readRotation();
	staticObject.position = readPosition();

	staticObject.physicsResource = std::any_cast<rid_t>(readObject(kRID));
	_stream.skip(4);
	staticObject.meshResource = std::any_cast<rid_t>(readObject(kRID));
	_stream.skip(17);

	return staticObject;
}

Templates::DynamicObject ObjectBinaryReadStream::readDynamicObject(unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 11
	 * Alan Wakes American Nightmare: 12
	 */
	Templates::DynamicObject dynamicObject{};

	dynamicObject.rotation = readRotation();
	dynamicObject.position = readPosition();

	dynamicObject.physicsResource = std::any_cast<rid_t>(readObject(kRID));
	dynamicObject.resourcePath = _dp->getString(_stream.readUint32LE());
	dynamicObject.meshResource = std::any_cast<rid_t>(readObject(kRID));
	dynamicObject.identifier = _dp->getString(_stream.readUint32LE());

	unsigned int unknown1 = _stream.readUint32LE();
	unsigned int unknown2 = _stream.readUint32LE();
	unsigned int unknown3 = _stream.readUint32LE();

	_stream.skip(4);
	//std::string name3 = _dp->getString(cid.readUint32LE());

	dynamicObject.gid = readGID();

	if (version == 12)
		_stream.skip(13);
	else // version == 11
		_stream.skip(9);

	return dynamicObject;
}

Templates::DynamicObjectScript ObjectBinaryReadStream::readDynamicObjectScript(unsigned int version) {
	Templates::DynamicObjectScript dynamicObject{};

	dynamicObject.gid = readGID();
	dynamicObject.script = std::any_cast<Templates::ScriptVariables>(readObject(kScriptVariables));

	uint32_t value = _stream.readUint32LE();
	_stream.skip(4);

	return dynamicObject;
}

Templates::CellInfo ObjectBinaryReadStream::readCellInfo() {
	Templates::CellInfo cellInfo{};

	cellInfo.x = _stream.readUint32LE();
	cellInfo.y = _stream.readUint32LE();

	cellInfo.lowDetailFoliageCount = _stream.readUint32LE();
	cellInfo.highDetailFoliageCount = _stream.readUint32LE();

	return cellInfo;
}

Templates::Animation ObjectBinaryReadStream::readAnimation(unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 17
	 * Alan Wakes American Nightmare: 19
	 */
	Templates::Animation animation{};

	animation.gid = readGID();
	animation.skeletonGid = readGID();
	animation.id = _stream.readUint32LE();

	animation.rid = std::any_cast<rid_t>(readObject(kRID));

	if (version == 17)
		_stream.skip(1);

	animation.name = _dp->getString(_stream.readUint32LE());

	_stream.skip(15);

	return animation;
}

Templates::Skeleton ObjectBinaryReadStream::readSkeleton() {
	Templates::Skeleton skeleton{};

	skeleton.gid = readGID();
	skeleton.name = _dp->getString(_stream.readUint32LE());

	skeleton.rid = std::any_cast<rid_t>(readObject(kRID));

	skeleton.id = _stream.readUint32LE();

	return skeleton;
}

Templates::SkeletonSetup ObjectBinaryReadStream::readSkeletonSetup() {
	Templates::SkeletonSetup skeletonSetup{};

	skeletonSetup.rootBoneGid = readGID();
	skeletonSetup.identifier = _dp->getString(_stream.readUint32LE());

	_stream.skip(7);

	return Templates::SkeletonSetup();
}

Templates::NotebookPage ObjectBinaryReadStream::readNotebookPage() {
	Templates::NotebookPage notebookPage{};

	notebookPage.gid = readGID();

	notebookPage.name = _dp->getString(_stream.readUint32LE());

	// Probably GID?
	_stream.skip(8);

	notebookPage.episodeNumber = _stream.readUint32LE();
	notebookPage.id = _stream.readUint32LE();

	notebookPage.onlyInNightmare = _stream.readByte() == 1;

	return notebookPage;
}

Templates::Sound ObjectBinaryReadStream::readSound() {
	Templates::Sound sound{};

	sound.gid = readGID();
	sound.threed = _stream.readByte() == 1;
	sound.streamed = _stream.readByte() == 1;
	sound.looping = _stream.readUint32LE();
	sound.volume = _stream.readIEEEFloatLE();
	sound.hotspot = _stream.readIEEEFloatLE();
	sound.falloff = _stream.readIEEEFloatLE();
	sound.volumeVariation = _stream.readIEEEFloatLE();
	sound.frequencyVariation = _stream.readIEEEFloatLE();

	_stream.skip(0x26);

	sound.rid = std::any_cast<rid_t>(readObject(kRID));

	_stream.skip(7);

	return sound;
}

Templates::Character ObjectBinaryReadStream::readCharacter(unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 13
	 * Alan Wakes American Nightmare: 17
	 */
	Templates::Character character{};

	character.gid = readGID();
	character.classGid = readGID();

	if (version == 13)
		_stream.skip(1);

	// Mesh
	character.meshResource = std::any_cast<rid_t>(readObject(kRID));

	character.rotation = readRotation();
	character.position = readPosition();

	uint32_t numResources = _stream.readUint32LE();
	std::vector<rid_t> rids(numResources);
	for (auto &rid : rids) {
		rid = std::any_cast<rid_t>(readObject(kRID));
	}

	if (version == 17) {
		_stream.skip(4);

		uint32_t identifierLength = _stream.readUint32LE();
		character.identifier = _stream.readFixedSizeString(identifierLength);

		// Cloth
		character.clothResource = std::any_cast<rid_t>(readObject(kRID));

		// TODO: Cloth Parameters
		_stream.skip(48);

		// FaceFX
		character.fxaResource = std::any_cast<rid_t>(readObject(kRID));

		_stream.skip(1);

		// Animgraphs
		character.animgraphResource = std::any_cast<rid_t>(readObject(kRID));

		_stream.skip(9);

		// Additional resources
		const auto resource1 = std::any_cast<rid_t>(readObject(kRID));
		const auto resource2 = std::any_cast<rid_t>(readObject(kRID));
		const auto resource3 = std::any_cast<rid_t>(readObject(kRID));
		const auto resource4 = std::any_cast<rid_t>(readObject(kRID));
	} else { // Version 13
		_stream.skip(0x3A);
	}

	return character;
}

Templates::CharacterScript ObjectBinaryReadStream::readCharacterScript() {
	Templates::CharacterScript characterScript{};

	characterScript.gid = readGID();
	characterScript.script = std::any_cast<Templates::ScriptVariables>(readObject(kScriptVariables));

	_stream.skip(8); // Always 0?

	return characterScript;
}

Templates::CharacterClass ObjectBinaryReadStream::readCharacterClass(unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 38
	 * Alan Wakes American Nightmare: 42
	 */
	Templates::CharacterClass characterClass{};

	characterClass.gid = readGID();
	characterClass.name = _dp->getString(_stream.readUint32LE());

	uint32_t numBaseClasses = 4;
	if (version == 42)
		numBaseClasses = _stream.readUint32LE();

	for (int i = 0; i < numBaseClasses; ++i) {
		const auto baseClass = _dp->getString(_stream.readUint32LE());
		if (!baseClass.empty())
			characterClass.baseClasses.emplace_back(baseClass);
	}

	characterClass.skeletonGid = readGID();
	if (version == 42) {
		characterClass.parentName = _dp->getString(_stream.readUint32LE());
		characterClass.capsuleHeight = _stream.readIEEEFloatLE();
		characterClass.capsuleRadius = _stream.readIEEEFloatLE();
		characterClass.lethalDoseOfHitEnergy = _stream.readIEEEFloatLE();
		characterClass.healthRecoveryStartDelay = _stream.readIEEEFloatLE();
		characterClass.healthRecoveryTime = _stream.readIEEEFloatLE();
		characterClass.shadowShieldStrength = _stream.readIEEEFloatLE();
	}

	characterClass.strongShield = _stream.readByte() != 0;
	characterClass.kickbackMultiplier = _stream.readIEEEFloatLE();
	characterClass.timeBetweenDazzles = _stream.readIEEEFloatLE();

	if (version == 42) {
		unsigned int numAnimations = _stream.readUint32LE();
		std::vector<uint32_t> values = _dp->getValues(_stream.readUint32LE(), numAnimations);
		for (const auto &value : values) {
			characterClass.animations.emplace_back(ObjectID(value));
		}

		characterClass.animationParameters = std::any_cast<Templates::AnimationParameters>(readObject(kAnimationParameters));

		characterClass.type = _dp->getString(_stream.readUint32LE());
		_stream.skip(8);
	} else {
		_stream.skip(0x49);
	}

	return characterClass;
}

Templates::TaskDefinition ObjectBinaryReadStream::readTaskDefinition(unsigned int version) {
	/*
	 * Versions:
	 * Alan Wake: 11
	 * Alan Wakes American Nightmare: 15
	 */
	Templates::TaskDefinition taskDefinition{};

	taskDefinition.name = _dp->getString(_stream.readUint32LE());

	unsigned int count = _stream.readUint32LE();
	uint32_t offset = _stream.readUint32LE();
	auto values = _dp->getValues(offset, count);

	_stream.skip(8); // Another offset and count into the dp file

	bool rootTask = _stream.readByte() != 0; // Starting Task?
	bool topLevelTask = _stream.readByte() != 0;

	taskDefinition.rotation = readRotation();
	taskDefinition.position = readPosition();

	taskDefinition.activateOnStartup = _stream.readByte() != 0;
	if (version == 15) {
		taskDefinition.activateOnStartupRound.resize(3);
		taskDefinition.activateOnStartupRound[0] = _stream.readByte() != 0;
		taskDefinition.activateOnStartupRound[1] = _stream.readByte() != 0;
		taskDefinition.activateOnStartupRound[2] = _stream.readByte() != 0;
	}
	bool gidlessTask = _stream.readByte() != 0;

	taskDefinition.gid = readGID();

	bool b2 = _stream.readByte() != 0; // If it is a non zero position?

	if (version == 15) {
		taskDefinition.rotationPlayer = readRotation();
		taskDefinition.positionPlayer = readPosition();

		taskDefinition.playerCharacter.resize(3);
		taskDefinition.playerCharacter[0] = readGID();
		_stream.skip(8);
		taskDefinition.cinematic = _dp->getString(_stream.readUint32LE());
		taskDefinition.playerCharacter[1] = readGID();
		taskDefinition.playerCharacter[2] = readGID();
	} else {
		_stream.skip(0x44);
	}

	return taskDefinition;
}

Templates::ScriptVariables ObjectBinaryReadStream::readScriptVariables(unsigned int version) {
	Templates::ScriptVariables script{};

	script.codeSize = _stream.readUint32LE();
	script.offsetCode = _stream.readUint32LE();
	script.numHandlers = _stream.readUint32LE();
	script.offsetHandlers = _stream.readUint32LE();
	script.numVariables = _stream.readUint32LE();
	script.offsetVariables = _stream.readUint32LE();
	script.numSignals = _stream.readUint32LE();
	script.offsetSignals = _stream.readUint32LE();

	if (version >= 2) {
		script.numDebugEntries = _stream.readUint32LE();
		script.offsetDebugEntries = _stream.readUint32LE();
	}

	return script;
}

Templates::Script ObjectBinaryReadStream::readScript() {
	Templates::Script script{};

	script.gid = readGID();
	script.script = std::any_cast<Templates::ScriptVariables>(readObject(kScriptVariables));

	return script;
}

Templates::ScriptInstance ObjectBinaryReadStream::readScriptInstance() {
	Templates::ScriptInstance scriptInstance{};

	scriptInstance.gid2 = readGID();
	scriptInstance.gid = readGID();

	scriptInstance.rotation = readRotation();
	scriptInstance.position = readPosition();

	return scriptInstance;
}

Templates::PointLight ObjectBinaryReadStream::readPointLight(unsigned int version) {
	/*
     * Versions:
     * Alan Wake: 11
     * Alan Wakes American Nightmare: 13
     */
	Templates::PointLight pointLight{};

	pointLight.gid = readGID();

	if (version == 13) {
		pointLight.gid2 = readGID();

		pointLight.rotation = readRotation();
		pointLight.position = readPosition();

		float val1 = _stream.readIEEEFloatLE();
		float val2 = _stream.readIEEEFloatLE();
		float val3 = _stream.readIEEEFloatLE();
		float val4 = _stream.readIEEEFloatLE();

		_stream.skip(10);

		pointLight.meshRid = std::any_cast<rid_t>(readObject(kRID));
		pointLight.staticShadowMapRid = std::any_cast<rid_t>(readObject(kRID));

		float val5 = _stream.readIEEEFloatLE();

		_stream.skip(70);
	} else { // Version 11
		_stream.skip(0x94);
	}

	return pointLight;
}

Templates::FloatingScript ObjectBinaryReadStream::readFloatingScript() {
	Templates::FloatingScript floatingScript{};

	floatingScript.gid = readGID();
	floatingScript.script = std::any_cast<Templates::ScriptVariables>(readObject(kScriptVariables));
	floatingScript.rotation = readRotation();
	floatingScript.position = readPosition();

	return floatingScript;
}

Templates::Trigger ObjectBinaryReadStream::readTrigger(unsigned int version) {
	/*
     * Versions:
     * Alan Wake: 18
     * Alan Wakes American Nightmare: 20
     */
	Templates::Trigger trigger{};

	trigger.gid2 = readGID();
	trigger.gid = readGID();

	_stream.skip(4); // Priority?

	trigger.identifier = _dp->getString(_stream.readUint32LE());

	_stream.skip(4);

	trigger.localeString = _dp->getString(_stream.readUint32LE());

	_stream.skip(12);
	unsigned int count = _stream.readUint32LE();
	auto values = _dp->getValues(_stream.readUint32LE(), count);

	if (version == 20)
		_stream.skip(7);
	else
		_stream.skip(3);

	return trigger;
}

Templates::AreaTrigger ObjectBinaryReadStream::readAreaTrigger() {
	Templates::AreaTrigger  areaTrigger{};

	areaTrigger.gid = readGID();

	uint32_t value1 = _stream.readUint32LE();

	areaTrigger.identifier = _dp->getString(_stream.readUint32LE());

	uint32_t numPositions = _stream.readUint32LE();
	areaTrigger.positions = _dp->getPositions2D(_stream.readUint32LE(), numPositions);

	_stream.skip(32);

	return areaTrigger;
}

Templates::TaskContent ObjectBinaryReadStream::readTaskContent() {
	Templates::TaskContent taskContent{};

	unsigned int value1 = _stream.readUint32LE();
	std::string str1 = _dp->getString(_stream.readUint32LE());
	unsigned int value2 = _stream.readUint32LE();

	// List of rids
	unsigned int numRids = _stream.readUint32LE();
	uint32_t offsetRids = _stream.readUint32LE();
	taskContent.rids = _dp->getValues(offsetRids, numRids);

	// List of gids + 8byte padding
	unsigned int value4 = _stream.readUint32LE();
	std::string str3 = _dp->getString(_stream.readUint32LE());

	// Unknown container
	readObject(kAttachmentResources);

	unsigned int count = _stream.readUint32LE();
	for (int i = 0; i < count; ++i) {
		_stream.readUint32LE();
	}

	return taskContent;
}

Templates::AttachmentResource ObjectBinaryReadStream::readAttachmentResources() {
	Templates::AttachmentResource attachmentResource{};

	// TODO
	_stream.skip(24);

	return attachmentResource;
}

Templates::Waypoint ObjectBinaryReadStream::readWaypoint() {
	Templates::Waypoint waypoint{};

	waypoint.gid = readGID();
	waypoint.rotation = readRotation();
	waypoint.position = readPosition();

	return waypoint;
}

Templates::AnimationParameters ObjectBinaryReadStream::readAnimationParameters() {
	Templates::AnimationParameters animationParameters{};

	animationParameters.animationBlendTime = _stream.readIEEEFloatLE();
	animationParameters.halfRotationTime = _stream.readIEEEFloatLE();
	animationParameters.tiltGain = _stream.readIEEEFloatLE();
	animationParameters.tiltRegression = _stream.readIEEEFloatLE();
	animationParameters.tiltAngleRadians = _stream.readIEEEFloatLE();
	animationParameters.tiltAgility = _stream.readIEEEFloatLE();
	animationParameters.tiltScaleForward = _stream.readIEEEFloatLE();
	animationParameters.tiltScaleBackwards = _stream.readIEEEFloatLE();
	animationParameters.animationProfile = _stream.readUint32LE();

	return animationParameters;
}

Templates::KeyFramedObject ObjectBinaryReadStream::readKeyFramedObject() {
	Templates::KeyFramedObject keyFramedObject{};

	keyFramedObject.rotation = readRotation();
	keyFramedObject.position = readPosition();

	keyFramedObject.physicsResource = std::any_cast<rid_t>(readObject(kRID));
	std::string source = _dp->getString(_stream.readUint32LE());
	keyFramedObject.meshResource = std::any_cast<rid_t>(readObject(kRID));
	std::string name = _dp->getString(_stream.readUint32LE());
	_stream.skip(8);
	const uint32_t numRids = _stream.readUint32LE();
	std::vector<rid_t> rids = _dp->getValues(_stream.readUint32LE(), numRids);
	keyFramedObject.gid = readGID();

	keyFramedObject.keyFramer = _stream.readUint32LE();
	_stream.skip(5);

	keyFramedObject.rotation2 = readRotation();
	keyFramedObject.position2 = readPosition();

	return keyFramedObject;
}

Templates::KeyFramer ObjectBinaryReadStream::readKeyFramer() {
	Templates::KeyFramer keyFramer{};

	keyFramer.gid = readGID();
	const uint32_t numKeyFrames = _stream.readUint32LE();
	const auto keyFrames = _dp->getValues(_stream.readUint32LE(), numKeyFrames);
	const uint32_t numKeyFrameAnimations = _stream.readUint32LE();
	const auto keyFrameAnimations = _dp->getValues(_stream.readUint32LE(), numKeyFrameAnimations);

	for (int i = 0; i < numKeyFrames; ++i) {
		keyFramer.keyFrames.emplace_back(keyFrames[i]);
	}

	for (int i = 0; i < numKeyFrameAnimations; ++i) {
		keyFramer.keyFrameAnimations.emplace_back(keyFrameAnimations[i]);
	}

	keyFramer.parentKeyFramer = _stream.readUint32LE();
	const auto val = _stream.readUint32LE();
	const auto oid = _stream.readUint32LE();

	const auto numResources = _stream.readUint32LE();
	keyFramer.resources = _dp->getValues(_stream.readUint32LE(), numResources);
	bool val1 = _stream.readByte();

	return keyFramer;
}

Templates::KeyFrameAnimation ObjectBinaryReadStream::readKeyFrameAnimation() {
	Templates::KeyFrameAnimation keyFrameAnimation{};

	keyFrameAnimation.gid = readGID();
	keyFrameAnimation.startKeyFrame = _stream.readUint32LE();
	keyFrameAnimation.endKeyFrame = _stream.readUint32LE();
	keyFrameAnimation.length = _stream.readIEEEFloatLE();

	// Unknown values
	const auto num1 = _stream.readUint32LE();
	const auto vals1 = _dp->getValues(_stream.readUint32LE(), num1);
	const auto num2 = _stream.readUint32LE();
	const auto vals2 = _dp->getFloats(_stream.readUint32LE(), num2);
	const auto num3 = _stream.readUint32LE();
	const auto vals3 = _dp->getFloats(_stream.readUint32LE(), num3);

	keyFrameAnimation.animationResource = std::any_cast<rid_t>(readObject(kRID));
	_stream.skip(4);
	keyFrameAnimation.nextAnimation = readGID();

	return keyFrameAnimation;
}

Templates::KeyFrame ObjectBinaryReadStream::readKeyFrame() {
	Templates::KeyFrame keyFrame{};

	keyFrame.position = readPosition();
	keyFrame.rotation = readRotation();

	return keyFrame;
}

Templates::FileInfoMetadata ObjectBinaryReadStream::readFileInfoMetadata() {
	Templates::FileInfoMetadata fileInfoMetadata{};

	fileInfoMetadata.fileSize = _stream.readUint32LE();
	fileInfoMetadata.fileDataCRC = _stream.readUint32LE();
	fileInfoMetadata.flags = _stream.readUint32LE();

	return fileInfoMetadata;
}

Templates::FoliageMeshMetadata ObjectBinaryReadStream::readFoliageMeshMetadata() {
	Templates::FoliageMeshMetadata foliageMeshMetadata{};

	foliageMeshMetadata.vertexBufferBytes = _stream.readUint32LE();
	foliageMeshMetadata.indexCount = _stream.readUint32LE();

	foliageMeshMetadata.boundBox = std::any_cast<Common::BoundBox>(readObject(kAABB));

	foliageMeshMetadata.textureRids.resize(_stream.readUint32LE());
	for (auto &textureRid : foliageMeshMetadata.textureRids) {
		textureRid = std::any_cast<rid_t>(readObject(kRID));
	}

	return foliageMeshMetadata;
}

Templates::HavokAnimationMetadata ObjectBinaryReadStream::readHavokAnimationMetadata() {
	Templates::HavokAnimationMetadata havokAnimationMetadata{};

	uint32_t pathLength = _stream.readUint32LE();
	havokAnimationMetadata.animationEventPath = _stream.readFixedSizeString(pathLength, true);

	return havokAnimationMetadata;
}

Templates::MeshMetadata ObjectBinaryReadStream::readMeshMetadata() {
	Templates::MeshMetadata meshMetadata{};

	meshMetadata.vertexBufferBytes = _stream.readUint32LE();
	meshMetadata.indexCount = _stream.readUint32LE();
	meshMetadata.boundBox = std::any_cast<Common::BoundBox>(readObject(kAABB));
	meshMetadata.hasBones = _stream.readByte();
	meshMetadata.textureRids.resize(_stream.readUint32LE());
	for (auto &textureRid : meshMetadata.textureRids) {
		textureRid = std::any_cast<rid_t>(readObject(kRID));
	}

	return meshMetadata;
}

Templates::ParticleSystemMetadata ObjectBinaryReadStream::readParticleSystemMetadata() {
	Templates::ParticleSystemMetadata particleSystemMetadata{};

	particleSystemMetadata.textureRids.resize(_stream.readUint32LE());
	for (auto &textureRid : particleSystemMetadata.textureRids) {
		textureRid = std::any_cast<rid_t>(readObject(kRID));
	}

	return particleSystemMetadata;
}

Templates::TextureMetadata ObjectBinaryReadStream::readTextureMetadata() {
	Templates::TextureMetadata textureMetadata{};

	textureMetadata.type = _stream.readUint32LE();
	textureMetadata.format = _stream.readUint32LE();
	textureMetadata.filter = _stream.readUint32LE();
	textureMetadata.width = _stream.readUint32LE();
	textureMetadata.height = _stream.readUint32LE();
	textureMetadata.depth = _stream.readUint32LE();

	_stream.skip(4);

	for (int i = 0; i < 8; ++i) {
		uint32_t offset = _stream.readUint32LE();
		if (offset != 0xFFFFFFFF)
			textureMetadata.mipmapOffsets.emplace_back(offset);
	}

	textureMetadata.highDetailStreamDistance = _stream.readIEEEFloatLE();
	textureMetadata.useTextureLOD = _stream.readByte() == 1;

	return textureMetadata;
}

GID ObjectBinaryReadStream::readGID() {
	GID gid{};
	gid.type = _stream.readUint32LE();
	gid.id = _stream.readUint32BE();
	return gid;
}

glm::vec3 ObjectBinaryReadStream::readPosition() {
	glm::vec3 position;

	position.x = _stream.readIEEEFloatLE();
	position.y = _stream.readIEEEFloatLE();
	position.z = _stream.readIEEEFloatLE();

	return position;
}

glm::mat3 ObjectBinaryReadStream::readRotation() {
	glm::mat3 rotation;

	rotation[0].x = _stream.readIEEEFloatLE();
	rotation[0].y = _stream.readIEEEFloatLE();
	rotation[0].z = _stream.readIEEEFloatLE();
	rotation[1].x = _stream.readIEEEFloatLE();
	rotation[1].y = _stream.readIEEEFloatLE();
	rotation[1].z = _stream.readIEEEFloatLE();
	rotation[2].x = _stream.readIEEEFloatLE();
	rotation[2].y = _stream.readIEEEFloatLE();
	rotation[2].z = _stream.readIEEEFloatLE();

	return rotation;
}

}
