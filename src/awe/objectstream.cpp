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

static const uint32_t kDeadBeef   = 0xDEADBEEF;
static const uint32_t kDeadBeefV2 = 0xD34DB33F;

static const uint32_t kContentHashResourceID          = Common::crc32(Common::toLower("content::ResourceID"));
static const uint32_t kContentHashStaticObject        = Common::crc32(Common::toLower("content::StaticObject"));
static const uint32_t kContentHashDynamicObject       = Common::crc32(Common::toLower("content::DynamicObject"));
static const uint32_t kContentHashDynamicObjectScript = Common::crc32(Common::toLower("content::DynamicObjectScript"));
static const uint32_t kContentHashCellInfo            = Common::crc32(Common::toLower("content::CellInfo"));
static const uint32_t kContentHashAnimation           = Common::crc32(Common::toLower("content::Animation"));
static const uint32_t kContentHashSkeleton            = Common::crc32(Common::toLower("content::DBSkeleton"));
static const uint32_t kContentHashSound               = Common::crc32(Common::toLower("content::DBSound"));
static const uint32_t kContentHashSkeletonSetup       = Common::crc32(Common::toLower("content::SkeletonSetup"));
static const uint32_t kContentHashNotebookPage        = Common::crc32(Common::toLower("content::NotebookPage"));
static const uint32_t kContentHashCharacter           = Common::crc32(Common::toLower("content::Character"));
static const uint32_t kContentHashCharacterScript     = Common::crc32(Common::toLower("content::CharacterScript"));
static const uint32_t kContentHashLoadingScreenHint   = Common::crc32(Common::toLower("content::LoadingScreenHint"));
static const uint32_t kContentHashTaskDefinition      = Common::crc32(Common::toLower("content::TaskDefinition"));
static const uint32_t kContentHashScriptVariables     = Common::crc32(Common::toLower("content::ScriptVariables"));
static const uint32_t kContentHashScript              = Common::crc32(Common::toLower("content::Script"));
static const uint32_t kContentHashScriptInstance      = Common::crc32(Common::toLower("content::ScriptInstance"));
static const uint32_t kContentHashPointLight          = Common::crc32(Common::toLower("content::PointLightInstance"));
static const uint32_t kContentHashFloatingScript      = Common::crc32(Common::toLower("content::FloatingScript"));
static const uint32_t kContentHashTrigger             = Common::crc32(Common::toLower("content::Trigger"));
static const uint32_t kContentHashAreaTrigger         = Common::crc32(Common::toLower("content::AreaTrigger"));
static const uint32_t kContentHashTaskContent         = Common::crc32(Common::toLower("content::TaskContent"));
static const uint32_t kContentHashAttachmentResources = Common::crc32(Common::toLower("content::AttachmentResources"));
static const uint32_t kContentHashWaypoint            = Common::crc32(Common::toLower("content::Waypoint"));

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

		// Unknown container
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
	if (version == 15)
		_stream.skip(3);  // Startup tasks Round 1-3
	bool gidlessTask = _stream.readByte() != 0;

	taskDefinition.gid = readGID();

	bool b2 = _stream.readByte() != 0; // If it is a non zero position?

	if (version == 15) {
		taskDefinition.rotationPlayer = readRotation();
		taskDefinition.positionPlayer = readPosition();

		// Characters?
		GID gid1 = readGID();
		_stream.skip(8);
		taskDefinition.cinematic = _dp->getString(_stream.readUint32LE());
		GID gid2 = readGID();
		GID gid3 = readGID();
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
	script.size3 = _stream.readUint32LE();
	script.offset3 = _stream.readUint32LE();
	script.numSignals = _stream.readUint32LE();
	script.offsetSignals = _stream.readUint32LE();

	if (version >= 2) {
		script.size5 = _stream.readUint32LE();
		script.offset5 = _stream.readUint32LE();
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

ObjectBinaryReadStreamV1::ObjectBinaryReadStreamV1(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) : ObjectBinaryReadStream(stream, std::move(dp)) {
}

Object ObjectBinaryReadStreamV1::readObject(ObjectType type, unsigned int version) {
	Object object;
	switch (type) {
		case kRID: object = readRID(); break;
		case kAABB: object = readAABB(); break;
		case kStaticObject: object = readStaticObject(); break;
		case kDynamicObject: object = readDynamicObject(version); break;
		case kDynamicObjectScript: object = readDynamicObjectScript(version); break;
		case kCellInfo: object = readCellInfo(); break;
		case kAnimation: object = readAnimation(version); break;
		case kSkeleton: object = readSkeleton(); break;
		case kSkeletonSetup: object = readSkeletonSetup(); break;
		case kNotebookPage: object = readNotebookPage(); break;
		case kSound: object = readSound(); break;
		case kCharacter: object = readCharacter(version); break;
		case kCharacterScript: object = readCharacterScript(); break;
		case kTaskDefinition: object = readTaskDefinition(version); break;
		case kTaskContent: object = readTaskContent(); break;
		case kScriptVariables: object = readScriptVariables(version); break;
		case kScript: object = readScript(); break;
		case kScriptInstance: object = readScriptInstance(); break;
		case kPointLight: object = readPointLight(version); break;
		case kFloatingScript: object = readFloatingScript(); break;
		case kTrigger: object = readTrigger(version); break;
		case kAreaTrigger: object = readAreaTrigger(); break;
		case kAttachmentResources: object = readAttachmentResources(); break;
		case kWaypoint: object = readWaypoint(); break;

		case kFileInfoMetadata: object = readFileInfoMetadata(); break;
		case kFoliageMeshMetadata: object = readFoliageMeshMetadata(); break;
		case kHavokAnimationMetadata: object = readHavokAnimationMetadata(); break;
		case kTextureMetadata: object = readTextureMetadata(); break;
		case kParticleSystemMetadata: object = readParticleSystemMetadata(); break;
		case kMeshMetadata: object = readMeshMetadata(); break;

		default: throw std::runtime_error("Unsupported content type");
	}

	return object;
}

ObjectBinaryReadStreamV2::ObjectBinaryReadStreamV2(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) :
	ObjectBinaryReadStream(stream, std::move(dp)) {
}

Object ObjectBinaryReadStreamV2::readObject(ObjectType type, unsigned int version) {
	uint32_t magicValue = _stream.readUint32LE();
	if (magicValue != kDeadBeef)
		throw std::runtime_error("Container missing Deadbeef magic id");

	uint32_t size = _stream.readUint32LE();
	uint32_t contentHash = _stream.readUint32LE();
	uint32_t tagVersion = _stream.readUint32LE();

	if (getContentHash(type) != contentHash)
		throw std::runtime_error("Container has unexpected content");

	if (version != tagVersion && version != 0)
		throw std::runtime_error("Container has unexpected version");

	if (version == 0)
		version = tagVersion;

	Object object;
	switch (type) {
		case kRID: object = readRID(); break;
		case kStaticObject: object = readStaticObject(); break;
		case kDynamicObject: object = readDynamicObject(version); break;
		case kDynamicObjectScript: object = readDynamicObjectScript(version); break;
		case kCellInfo: object = readCellInfo(); break;
		case kAnimation: object = readAnimation(version); break;
		case kSkeleton: object = readSkeleton(); break;
		case kSkeletonSetup: object = readSkeletonSetup(); break;
		case kNotebookPage: object = readNotebookPage(); break;
		case kSound: object = readSound(); break;
		case kCharacter: object = readCharacter(version); break;
		case kCharacterScript: object = readCharacterScript(); break;
		case kTaskDefinition: object = readTaskDefinition(version); break;
		case kTaskContent: object = readTaskContent(); break;
		case kScriptVariables: object = readScriptVariables(version); break;
		case kScript: object = readScript(); break;
		case kScriptInstance: object = readScriptInstance(); break;
		case kPointLight: object = readPointLight(version); break;
		case kFloatingScript: object = readFloatingScript(); break;
		case kTrigger: object = readTrigger(version); break;
		case kAreaTrigger: object = readAreaTrigger(); break;
		case kAttachmentResources: object = readAttachmentResources(); break;
		case kWaypoint: object = readWaypoint(); break;
		default: _stream.skip(size - 20);
	}

	magicValue = _stream.readUint32LE();
	if (magicValue != kDeadBeef)
		throw std::runtime_error("Container missing Deadbeef magic id");

	return object;
}

uint32_t ObjectBinaryReadStreamV2::getContentHash(ObjectType type) const {
	switch (type) {
		case kRID: return kContentHashResourceID;
		case kStaticObject: return kContentHashStaticObject;
		case kDynamicObject: return kContentHashDynamicObject;
		case kDynamicObjectScript: return kContentHashDynamicObjectScript;
		case kCellInfo: return kContentHashCellInfo;
		case kAnimation: return kContentHashAnimation;
		case kSkeleton: return kContentHashSkeleton;
		case kSkeletonSetup: return kContentHashSkeletonSetup;
		case kNotebookPage: return kContentHashNotebookPage;
		case kSound: return kContentHashSound;
		case kCharacter: return kContentHashCharacter;
		case kCharacterScript: return kContentHashCharacterScript;
		case kTaskDefinition: return kContentHashTaskDefinition;
		case kTaskContent: return kContentHashTaskContent;
		case kScriptVariables: return kContentHashScriptVariables;
		case kScript: return kContentHashScript;
		case kScriptInstance: return kContentHashScriptInstance;
		case kPointLight: return kContentHashPointLight;
		case kFloatingScript: return kContentHashFloatingScript;
		case kTrigger: return kContentHashTrigger;
		case kAreaTrigger: return kContentHashAreaTrigger;
		case kAttachmentResources: return kContentHashAttachmentResources;
		case kWaypoint: return kContentHashWaypoint;
		default: return 0;
	}
}

}