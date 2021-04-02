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
#include <stdexcept>
#include <zlib.h>

#include "src/common/strutil.h"

#include "src/awe/cidfile.h"

static const uint32_t kDeadBeef   = 0xDEADBEEF;
static const uint32_t kDeadBeefV2 = 0xD34DB33F;

static const uint32_t kContentStaticObject      = Common::crc32(Common::toLower("content::StaticObject"));
static const uint32_t kContentSound             = Common::crc32(Common::toLower("content::Sound"));
static const uint32_t kContentLoadingScreenHint = Common::crc32(Common::toLower("content::LoadingScreenHint"));

namespace AWE {

CIDFile::CIDFile(Common::ReadStream &cid, DPFile *dp) : _dp(dp) {
	uint32_t magic = cid.readUint32LE();
	uint32_t contentType = cid.readUint32LE(); // ?
	uint32_t numElements = cid.readUint32LE();

	cid.skip(4); // Always Zero? Or 64bit num elements?

	testFormat(cid);

	DataType type;
	if (_format == kSimple && !cid.eos()) {
		switch (magic) {
			case 10:
				type = kStaticObject;
				break;
			case 17:
				type = kAnimation;
				break;
			case 25:
				type = kSkeleton;
				break;
			case 38:
				type = kCharacterClass;
				break;
			default:
				throw std::runtime_error("Invalid content type");
		}
	}

	_containers.resize(numElements);
	for (auto &container : _containers) {
		if (_format == kSimple)
			container = readSimpleContainer(cid, type);
		else
			container = readContainer(cid);
	}
}

CIDFile::CIDFile(Common::ReadStream &cid, unsigned int numEntries) {
	testFormat(cid);

	_containers.resize(numEntries);
	for (auto &container : _containers) {
		container = readContainer(cid);
	}
}

CIDFile::CIDFile(Common::ReadStream &cid, unsigned int numEntries, CIDFile::DataType type) {
	testFormat(cid);

	_containers.resize(numEntries);
	for (auto &container : _containers) {
		container = readSimpleContainer(cid, type);
	}
}

const std::vector<CIDFile::Container> &CIDFile::getContainers() const {
	return _containers;
}

void CIDFile::testFormat(Common::ReadStream &cid) {
	// Simple test for determining the format of the file
	uint32_t deadbeefTest = cid.readUint32LE();
	switch (deadbeefTest) {
		case kDeadBeef:
			_format = kStructured;
			break;
		case kDeadBeefV2:
			_format = kStructuredV2;
			break;
		default:
			_format = kSimple;
			break;
	}

	cid.skip(-4);
}

CIDFile::Container CIDFile::readContainer(Common::ReadStream &cid) {
	CIDFile::Container container{};

	if (_format == kStructured) {
		uint32_t magicValue = cid.readUint32LE();
		if (magicValue != kDeadBeef)
			throw std::runtime_error("Contianer missing Deadbeef magic id");

		// Basically the length of the whole container between both deadbeef ids.
		uint32_t length = cid.readUint32LE();

		magicValue = cid.readUint32BE();

		cid.skip(4); // Every data block seems to have an id in addition to the magic value

		switch (magicValue) {
			case 0xE9A6EC14:
				container.type = CIDFile::kCellInfo;
				container.data = readCellInfo(cid);
				break;
			case 0x9B622683:
				container.type = CIDFile::kStaticObject;
				container.data = readStaticObject(cid);
				break;
			case 0x8F2362B8:
				container.type = CIDFile::kRID;
				container.data = readRID(cid);
				break;
			case 0x8168FA50:
				container.type = CIDFile::kSkeleton;
				container.data = readSkeleton(cid);
				break;
			case 0x95529251:
				container.type = CIDFile::kCharacter;
				container.data = readCharacter(cid);
				break;
			case 0x3CB2EAE3:
				container.type = CIDFile::kCharacterClass;
				container.data = readCharacterClass(cid);
				break;
			case 0x1A3FD147:
				container.type = CIDFile::kNotebookPage;
				container.data = readNotebookPage(cid);
				break;
			case 0xDDBE9F39:
				container.type = CIDFile::kDynamicObject;
				container.data = readDynamicObject(cid);
				break;
			case 0x53114FD3:
				container.type = CIDFile::kDynamicObjectScript;
				container.data = readDynamicObjectScript(cid);
				break;
			case 0x35E5EB31:
				container.type = CIDFile::kScript;
				container.data = readScript(cid);
				break;
			case 0x7D66BA4F:
				container.type = CIDFile::kAnimation;
				container.data = readAnimation(cid);
				break;
			case 0xA89C7C04:
				container.type = CIDFile::kScriptInstance;
				container.data = readScriptInstance(cid);
				break;
			case 0x97743F7B:
				container.type = CIDFile::kScriptInstanceScript;
				container.data = readObjectScript(cid);
				break;
			case 0x48D5CB7C:
				container.type = CIDFile::kAreaTrigger;
				container.data = readAreaTrigger(cid);
				break;
			case 0x19776FFA:
				container.type = CIDFile::kPhysicsMaterial;
				container.data = readPhysicsMaterial(cid);
				break;
			case 0xAB782FBB:
				container.type = CIDFile::kFileInfoMetadata;
				container.data = readFileInfoMetadata(cid);
				break;
			case 0x41FA4C18:
				container.type = CIDFile::kResourceMetadata;
				container.data = readResourceMetadata(cid);
				break;
			case 0xC9F1CE69:
				container.type = CIDFile::kCharacterScript;
				container.data = readCharacterScript(cid);
				break;
			case 0x8B3D76D8:
				container.type = CIDFile::kAABB;
				container.data = readAABB(cid);
				break;
			case 0x5E30D5ED:
				container.type = CIDFile::kPointLight;
				container.data = readPointLight(cid);
				break;
			case 0xFB6E6BB2:
				container.type = CIDFile::kTrigger;
				container.data = readTrigger(cid);
				break;
			case 0x5B9B2CBC:
				container.type = CIDFile::kFloatingScript;
				container.data = readFloatingScript(cid);
				break;
			case 0x3D5EC18C:
				container.type = CIDFile::kWaypoint;
				container.data = readWaypoint(cid);
				break;
			case 0xD5488A35:
				container.type = CIDFile::kTaskDefinition;
				container.data = readTaskDefinition(cid);
				break;
			case 0xED9F47C5:
				container.type = CIDFile::kTaskContent;
				container.data = readTaskContent(cid);
				break;
			case 0xE64C0531:
				container.type = CIDFile::kSound;
				container.data = readSound(cid);
				break;
			default:
				cid.skip(std::max<size_t>(length - 20, 0));
				break;
		}

		magicValue = cid.readUint32LE();
		if (magicValue != kDeadBeef)
			throw std::runtime_error("Container missing Deadbeef magic id");
	}

	return container;
}

CIDFile::Container CIDFile::readSimpleContainer(Common::ReadStream &cid, CIDFile::DataType type) {
	CIDFile::Container container{};

	container.type = type;
	switch (type) {
		case kStaticObject:
			container.data = readStaticObject(cid);
			break;

		case kSkeleton:
			container.data = readSkeleton(cid);
			break;
		case kAnimation:
			container.data = readAnimation(cid);
			break;
		case kCharacterClass:
			container.data = readCharacterClass(cid);
			break;

		case kFileInfoMetadata:
			container.data = readFileInfoMetadata(cid);
			break;
		case kFoliageMeshMetadata:
			container.data = readFoliageMeshMetadata(cid);
			break;
		case kHavokAnimationMetadata:
			container.data = readHavokAnimationMetadata(cid);
			break;
		case kMeshMetadata:
			container.data = readMeshMetadata(cid);
			break;
		case kParticleSystemMetadata:
			container.data = readParticleSystemMetadata(cid);
			break;
		case kTextureMetadata:
			container.data = readTextureMetadata(cid);
			break;
	}

	return container;
}

CIDFile::StaticObject CIDFile::readStaticObject(Common::ReadStream &cid) {
	CIDFile::StaticObject staticObject{};

	staticObject.rotation = readRotation(cid);
	staticObject.position = readPosition(cid);

	if (_format == kSimple)
		staticObject.physicsResource = cid.readUint32BE();
	else
		staticObject.physicsResource = std::get<RID>(readContainer(cid).data).rid;
	cid.skip(4);
	if (_format == kSimple)
		staticObject.meshResource = cid.readUint32BE();
	else
		staticObject.meshResource = std::get<RID>(readContainer(cid).data).rid;

	cid.skip(17);

	return staticObject;
}

CIDFile::CellInfo CIDFile::readCellInfo(Common::ReadStream &cid) {
	CIDFile::CellInfo cellInfo{};

	cellInfo.x = cid.readUint32LE();
	cellInfo.y = cid.readUint32LE();

	cellInfo.lowDetailFoliageCount = cid.readUint32LE();
	cellInfo.highDetailFoliageCount = cid.readUint32LE();

	return cellInfo;
}

CIDFile::RID CIDFile::readRID(Common::ReadStream &cid) {
	CIDFile::RID rid{};

	rid.rid = cid.readUint32BE();

	return rid;
}

CIDFile::Skeleton CIDFile::readSkeleton(Common::ReadStream &cid) {
	CIDFile::Skeleton skeleton{};

	skeleton.gid = readGID(cid);
	skeleton.name = _dp->getString(cid.readUint32LE());

	if (_format == kSimple)
		skeleton.rid = cid.readUint32LE();
	else
		skeleton.rid = std::get<RID>(readContainer(cid).data).rid;

	skeleton.id = cid.readUint32LE();

	return skeleton;
}

CIDFile::Character CIDFile::readCharacter(Common::ReadStream &cid) {
	CIDFile::Character character{};

	character.gid = readGID(cid);
	character.classGid = readGID(cid);

	// Mesh
	character.meshResource = std::get<RID>(readContainer(cid).data).rid;

	character.rotation = readRotation(cid);
	character.position = readPosition(cid);

	uint32_t numResources = cid.readUint32LE();
	std::vector<rid_t> rids(numResources);
	for (auto &rid : rids) {
		rid = std::get<RID>(readContainer(cid).data).rid;
	}

	cid.skip(4);

	uint32_t identifierLength = cid.readUint32LE();
	character.identifier = cid.readFixedSizeString(identifierLength);

	// Cloth
	character.clothResource = std::get<RID>(readContainer(cid).data).rid;

	// Unknown container
	cid.skip(48);

	// FaceFX
	character.fxaResource = std::get<RID>(readContainer(cid).data).rid;

	cid.skip(1);

	// Animgraphs
	character.animgraphResource = std::get<RID>(readContainer(cid).data).rid;

	cid.skip(9);

	// Additional resources
	rid_t resource1 = std::get<RID>(readContainer(cid).data).rid;
	rid_t resource2 = std::get<RID>(readContainer(cid).data).rid;
	rid_t resource3 = std::get<RID>(readContainer(cid).data).rid;
	rid_t resource4 = std::get<RID>(readContainer(cid).data).rid;

	return character;
}

CIDFile::CharacterClass CIDFile::readCharacterClass(Common::ReadStream &cid) {
	CIDFile::CharacterClass characterClass{};

	characterClass.gid = readGID(cid);

	characterClass.name = _dp->getString(cid.readUint32LE());

	uint32_t numBaseClasses = 4;
	if (_format != kSimple)
		numBaseClasses = cid.readUint32LE();
	for (int i = 0; i < numBaseClasses; ++i) {
		std::string baseClass = _dp->getString(cid.readUint32LE());
		if (!baseClass.empty())
			characterClass.baseClasses.emplace_back(baseClass);
	}

	characterClass.skeletonGid.type = cid.readUint32LE();
	characterClass.skeletonGid.id = cid.readUint32BE();

	if (_format != kSimple) {
		std::string str = _dp->getString(cid.readUint32LE());

		float val1 = cid.readIEEEFloatLE();
		float val2 = cid.readIEEEFloatLE();
		float val3 = cid.readIEEEFloatLE();
		float val4 = cid.readIEEEFloatLE();
		float val5 = cid.readIEEEFloatLE();
		float val6 = cid.readIEEEFloatLE();
	}
	cid.skip(1);
	float val7 = cid.readIEEEFloatLE();
	float val8 = cid.readIEEEFloatLE();

	if (_format == kSimple) {
		cid.skip(0x49);
	} else {
		unsigned int count = cid.readUint32LE();
		std::vector<uint32_t> values = _dp->getValues(cid.readUint32LE(), count);

		// Unknown container
		cid.skip(0x44);
	}

	return characterClass;
}

CIDFile::CharacterScript CIDFile::readCharacterScript(Common::ReadStream &cid) {
	CIDFile::CharacterScript characterScript{};

	characterScript.gid = readGID(cid);
	characterScript.script = std::get<Script>(readContainer(cid).data);

	cid.skip(8); // Always 0?

	return characterScript;
}

CIDFile::NotebookPage CIDFile::readNotebookPage(Common::ReadStream &cid) {
	CIDFile::NotebookPage notebookPage{};

	notebookPage.gid = readGID(cid);

	notebookPage.name = _dp->getString(cid.readUint32LE());

	// Probably GID?
	cid.skip(8);

	notebookPage.episodeNumber = cid.readUint32LE();
	notebookPage.id = cid.readUint32LE();

	notebookPage.onlyInNightmare = cid.readByte() == 1;

	return notebookPage;
}

CIDFile::DynamicObject CIDFile::readDynamicObject(Common::ReadStream &cid) {
	CIDFile::DynamicObject dynamicObject{};

	dynamicObject.rotation = readRotation(cid);
	dynamicObject.position = readPosition(cid);

	dynamicObject.physicsResource = std::get<RID>(readContainer(cid).data).rid;
	dynamicObject.resourcePath = _dp->getString(cid.readUint32LE());
	dynamicObject.meshResource = std::get<RID>(readContainer(cid).data).rid;
	dynamicObject.identifier = _dp->getString(cid.readUint32LE());

	unsigned int unknown1 = cid.readUint32LE();
	unsigned int unknown2 = cid.readUint32LE();
	unsigned int unknown3 = cid.readUint32LE(); // Always 2?

	// Reference into the dp file?
	cid.skip(4);
	//std::string name3 = _dp->getString(cid.readUint32LE());

	dynamicObject.gid = readGID(cid);

	cid.skip(13);

	return dynamicObject;
}

CIDFile::DynamicObjectScript CIDFile::readDynamicObjectScript(Common::ReadStream &cid) {
	CIDFile::DynamicObjectScript dynamicObject{};

	dynamicObject.gid = readGID(cid);
	dynamicObject.script = std::get<Script>(readContainer(cid).data);

	uint32_t value = cid.readUint32LE();
	cid.skip(4);
	//dynamicObject.name = _dp->getString(cid.readUint32LE());

	return dynamicObject;
}

CIDFile::Script CIDFile::readScript(Common::ReadStream &cid) {
	CIDFile::Script script{};

	script.codeSize = cid.readUint32LE();
	script.offsetCode = cid.readUint32LE();
	script.numHandlers = cid.readUint32LE();
	script.offsetHandlers = cid.readUint32LE();
	script.size3 = cid.readUint32LE();
	script.offset3 = cid.readUint32LE();
	script.numSignals = cid.readUint32LE();
	script.offsetSignals = cid.readUint32LE();
	script.size5 = cid.readUint32LE();
	script.offset5 = cid.readUint32LE();

	return script;
}

CIDFile::Animation CIDFile::readAnimation(Common::ReadStream &cid) {
	CIDFile::Animation animation{};

	animation.gid = readGID(cid);
	animation.skeletonGid = readGID(cid);
	animation.id = cid.readUint32LE();

	if (_format == kSimple)
		animation.rid = cid.readUint32BE();
	else
		animation.rid = std::get<RID>(readContainer(cid).data).rid;

	animation.name = _dp->getString(cid.readUint32LE());

	cid.skip(15);

	return animation;
}

CIDFile::ScriptInstance CIDFile::readScriptInstance(Common::ReadStream &cid) {
	CIDFile::ScriptInstance scriptInstance{};

	scriptInstance.gid2 = readGID(cid);
	scriptInstance.gid = readGID(cid);

	scriptInstance.rotation = readRotation(cid);
	scriptInstance.position = readPosition(cid);

	return scriptInstance;
}

CIDFile::ObjectScript CIDFile::readObjectScript(Common::ReadStream &cid) {
	CIDFile::ObjectScript scriptInstanceScript{};

	scriptInstanceScript.gid = readGID(cid);
	scriptInstanceScript.script = std::get<Script>(readContainer(cid).data);

	return scriptInstanceScript;
}

CIDFile::AreaTrigger CIDFile::readAreaTrigger(Common::ReadStream &cid) {
	CIDFile::AreaTrigger  areaTrigger{};

	areaTrigger.gid.type = cid.readUint32LE();
	areaTrigger.gid.id   = cid.readUint32BE();

	uint32_t value1 = cid.readUint32LE();

	areaTrigger.identifier = _dp->getString(cid.readUint32LE());

	uint32_t numPositions = cid.readUint32LE();
	areaTrigger.positions = _dp->getPositions2D(cid.readUint32LE(), numPositions);

	cid.skip(32);

	return areaTrigger;
}

CIDFile::PhysicsMaterial CIDFile::readPhysicsMaterial(Common::ReadStream &cid) {
	CIDFile::PhysicsMaterial physicsMaterial{};

	physicsMaterial.name = _dp->getString(cid.readUint32LE());
	physicsMaterial.gid = readGID(cid);

	physicsMaterial.index = cid.readByte();
	float val1 = cid.readIEEEFloatLE();
	float val2 = cid.readIEEEFloatLE();
	float val3 = cid.readIEEEFloatLE();
	float val4 = cid.readIEEEFloatLE();

	bool val5 = cid.readByte() != 0;

	return physicsMaterial;
}

CIDFile::FileInfoMetadata CIDFile::readFileInfoMetadata(Common::ReadStream &cid) {
	CIDFile::FileInfoMetadata fileInfoMetadata{};

	fileInfoMetadata.fileSize = cid.readUint32LE();
	fileInfoMetadata.fileDataCRC = cid.readUint32LE();
	fileInfoMetadata.flags = cid.readUint32LE();

	return fileInfoMetadata;
}

CIDFile::ResourceMetadata CIDFile::readResourceMetadata(Common::ReadStream &cid) {
	CIDFile::ResourceMetadata resourceMetadata{};

	resourceMetadata.rid = std::get<RID>(readContainer(cid).data).rid;
	resourceMetadata.resourceType = cid.readUint32LE();

	return resourceMetadata;
}

CIDFile::FoliageMeshMetadata CIDFile::readFoliageMeshMetadata(Common::ReadStream &cid) {
	CIDFile::FoliageMeshMetadata foliageMeshMetadata{};

	foliageMeshMetadata.vertexBufferBytes = cid.readUint32LE();
	foliageMeshMetadata.indexCount = cid.readUint32LE();

	foliageMeshMetadata.boundBox.xmin = cid.readIEEEFloatLE();
	foliageMeshMetadata.boundBox.ymin = cid.readIEEEFloatLE();
	foliageMeshMetadata.boundBox.zmin = cid.readIEEEFloatLE();
	foliageMeshMetadata.boundBox.xmax = cid.readIEEEFloatLE();
	foliageMeshMetadata.boundBox.ymax = cid.readIEEEFloatLE();
	foliageMeshMetadata.boundBox.zmax = cid.readIEEEFloatLE();

	foliageMeshMetadata.textureRids.resize(cid.readUint32LE());
	for (auto &textureRid : foliageMeshMetadata.textureRids) {
		if (_format == kSimple)
			textureRid = cid.readUint32BE();
		else
			textureRid = std::get<RID>(readContainer(cid).data).rid;
	}

	return foliageMeshMetadata;
}

CIDFile::HavokAnimationMetadata CIDFile::readHavokAnimationMetadata(Common::ReadStream &cid) {
	CIDFile::HavokAnimationMetadata havokAnimationMetadata{};

	uint32_t pathLength = cid.readUint32LE();
	havokAnimationMetadata.animationEventPath = cid.readFixedSizeString(pathLength, true);
	//havokAnimationMetadata.length = cid.readIEEEFloatLE();

	return havokAnimationMetadata;
}

CIDFile::MeshMetadata CIDFile::readMeshMetadata(Common::ReadStream &cid) {
	CIDFile::MeshMetadata meshMetadata{};

	meshMetadata.vertexBufferBytes = cid.readUint32LE();
	meshMetadata.indexCount = cid.readUint32LE();

	meshMetadata.boundBox.xmin = cid.readIEEEFloatLE();
	meshMetadata.boundBox.ymin = cid.readIEEEFloatLE();
	meshMetadata.boundBox.zmin = cid.readIEEEFloatLE();
	meshMetadata.boundBox.xmax = cid.readIEEEFloatLE();
	meshMetadata.boundBox.ymax = cid.readIEEEFloatLE();
	meshMetadata.boundBox.zmax = cid.readIEEEFloatLE();

	meshMetadata.hasBones = cid.readByte() == 1;

	meshMetadata.textureRids.resize(cid.readUint32LE());
	for (auto &textureRid : meshMetadata.textureRids) {
		if (_format == kSimple)
			textureRid = cid.readUint32BE();
		else
			textureRid = std::get<RID>(readContainer(cid).data).rid;
	}

	return meshMetadata;
}

CIDFile::ParticleSystemMetadata CIDFile::readParticleSystemMetadata(Common::ReadStream &cid) {
	CIDFile::ParticleSystemMetadata particleSystem{};

	particleSystem.textureRids.resize(cid.readUint32LE());
	for (auto &textureRid : particleSystem.textureRids) {
		if (_format == kSimple)
			textureRid = cid.readUint32BE();
		else
			textureRid = std::get<RID>(readContainer(cid).data).rid;
	}

	return particleSystem;
}

CIDFile::TextureMetadata CIDFile::readTextureMetadata(Common::ReadStream &cid) {
	CIDFile::TextureMetadata textureMetadata{};

	textureMetadata.type = cid.readUint32LE();
	textureMetadata.format = cid.readUint32LE();
	textureMetadata.filter = cid.readUint32LE();
	textureMetadata.width = cid.readUint32LE();
	textureMetadata.height = cid.readUint32LE();
	textureMetadata.depth = cid.readUint32LE();

	cid.skip(4); // Mipmap Count?

	for (int i = 0; i < 8; ++i) {
		uint32_t offset = cid.readUint32LE();
		if (offset != 0xFFFFFFFF)
			textureMetadata.mipmapOffsets.push_back(offset);
	}

	textureMetadata.highDetailStreamDistance = cid.readIEEEFloatLE();
	textureMetadata.useTextureLOD = cid.readByte() == 1;

	return textureMetadata;
}

CIDFile::PointLight CIDFile::readPointLight(Common::ReadStream &cid) {
	CIDFile::PointLight pointLight{};

	pointLight.gid = readGID(cid);
	pointLight.gid2 = readGID(cid);

	pointLight.rotation = readRotation(cid);
	pointLight.position = readPosition(cid);

	float val1 = cid.readIEEEFloatLE();
	float val2 = cid.readIEEEFloatLE();
	float val3 = cid.readIEEEFloatLE();
	float val4 = cid.readIEEEFloatLE();

	cid.skip(10);

	pointLight.meshRid = std::get<RID>(readContainer(cid).data).rid;
	pointLight.staticShadowMapRid = std::get<RID>(readContainer(cid).data).rid;

	float val5 = cid.readIEEEFloatLE();

	cid.skip(70);

	return pointLight;
}

CIDFile::Trigger CIDFile::readTrigger(Common::ReadStream &cid) {
	CIDFile::Trigger trigger{};

	trigger.gid2 = readGID(cid);
	trigger.gid = readGID(cid);

	cid.skip(4); // Priority?

	trigger.identifier = _dp->getString(cid.readUint32LE());

	cid.skip(4);

	trigger.localeString = _dp->getString(cid.readUint32LE());

	cid.skip(12);
	unsigned int count = cid.readUint32LE();
	auto values = _dp->getValues(cid.readUint32LE(), count);

	cid.skip(7);

	return trigger;
}

CIDFile::FloatingScript CIDFile::readFloatingScript(Common::ReadStream &cid) {
	CIDFile::FloatingScript floatingScript{};
	
	floatingScript.gid = readGID(cid);
	floatingScript.script = std::get<Script>(readContainer(cid).data);
	floatingScript.rotation = readRotation(cid);
	floatingScript.position = readPosition(cid);
	
	return floatingScript;
}

CIDFile::Waypoint CIDFile::readWaypoint(Common::ReadStream &cid) {
	CIDFile::Waypoint waypoint{};
	
	waypoint.gid = readGID(cid);
	waypoint.rotation = readRotation(cid);
	waypoint.position = readPosition(cid);
	
	return waypoint;
}

CIDFile::TaskDefinition CIDFile::readTaskDefinition(Common::ReadStream &cid) {
	CIDFile::TaskDefinition taskDefinition{};

	taskDefinition.name = _dp->getString(cid.readUint32LE());

	unsigned int count = cid.readUint32LE();
	uint32_t offset = cid.readUint32LE();
	auto values = _dp->getValues(offset, count);

	cid.skip(8); // Another offset and count into the dp file

	bool rootTask = cid.readByte() != 0; // Starting Task?
	bool topLevelTask = cid.readByte() != 0;

	taskDefinition.rotation = readRotation(cid);
	taskDefinition.position = readPosition(cid);

	taskDefinition.activateOnStartup = cid.readByte() != 0;
	cid.skip(3);  // Startup tasks Round 1-3
	bool gidlessTask = cid.readByte() != 0;

	taskDefinition.gid = readGID(cid);

	bool b2 = cid.readByte() != 0; // If it is a non zero position?

	taskDefinition.rotationPlayer = readRotation(cid);
	taskDefinition.positionPlayer = readPosition(cid);

	// Characters?
	GID gid1 = readGID(cid);
	cid.skip(8);
	taskDefinition.cinematic = _dp->getString(cid.readUint32LE());
	GID gid2 = readGID(cid);
	GID gid3 = readGID(cid);

	return taskDefinition;
}

CIDFile::TaskContent CIDFile::readTaskContent(Common::ReadStream &cid) {
	CIDFile::TaskContent taskContent{};

	unsigned int value1 = cid.readUint32LE();
	std::string str1 = _dp->getString(cid.readUint32LE());
	unsigned int value2 = cid.readUint32LE();

	// List of rids
	unsigned int numRids = cid.readUint32LE();
	uint32_t offsetRids = cid.readUint32LE();
	taskContent.rids = _dp->getValues(offsetRids, numRids);

	// List of gids + 8byte padding
	unsigned int value4 = cid.readUint32LE();
	std::string str3 = _dp->getString(cid.readUint32LE());

	// Unknown container
	readContainer(cid);

	unsigned int count = cid.readUint32LE();
	for (int i = 0; i < count; ++i) {
		cid.readUint32LE();
	}

	return taskContent;
}

CIDFile::KeyFramer CIDFile::readKeyFramer(Common::ReadStream &cid) {
	CIDFile::KeyFramer keyFramer{};

	keyFramer.gid = readGID(cid);


	return keyFramer;
}

CIDFile::Sound CIDFile::readSound(Common::ReadStream &cid) {
	CIDFile::Sound sound{};

	sound.gid = readGID(cid);
	sound.threed = cid.readByte() == 1;
	sound.streamed = cid.readByte() == 1;
	sound.looping = cid.readUint32LE();
	sound.volume = cid.readIEEEFloatLE();
	sound.hotspot = cid.readIEEEFloatLE();
	sound.falloff = cid.readIEEEFloatLE();
	sound.volumeVariation = cid.readIEEEFloatLE();
	sound.frequencyVariation = cid.readIEEEFloatLE();

	cid.skip(0x26);

	sound.rid = std::get<RID>(readContainer(cid).data).rid;

	cid.skip(7);

	return sound;
}

CIDFile::AABB CIDFile::readAABB(Common::ReadStream &cid) {
	CIDFile::AABB aabb{};

	aabb.boundBox.xmin = cid.readIEEEFloatLE();
	aabb.boundBox.ymin = cid.readIEEEFloatLE();
	aabb.boundBox.zmin = cid.readIEEEFloatLE();
	aabb.boundBox.xmax = cid.readIEEEFloatLE();
	aabb.boundBox.ymax = cid.readIEEEFloatLE();
	aabb.boundBox.zmax = cid.readIEEEFloatLE();

	return aabb;
}

GID CIDFile::readGID(Common::ReadStream &cid) {
	GID gid;

	gid.type = cid.readUint32LE();
	gid.id = cid.readUint32BE();

	return gid;
}

glm::vec3 CIDFile::readPosition(Common::ReadStream &cid) {
	glm::vec3 position;

	position.x = cid.readIEEEFloatLE();
	position.y = cid.readIEEEFloatLE();
	position.z = cid.readIEEEFloatLE();

	return position;
}

glm::mat3 CIDFile::readRotation(Common::ReadStream &cid) {
	glm::mat3 rotation;

	rotation[0].x = cid.readIEEEFloatLE();
	rotation[0].y = cid.readIEEEFloatLE();
	rotation[0].z = cid.readIEEEFloatLE();
	rotation[1].x = cid.readIEEEFloatLE();
	rotation[1].y = cid.readIEEEFloatLE();
	rotation[1].z = cid.readIEEEFloatLE();
	rotation[2].x = cid.readIEEEFloatLE();
	rotation[2].y = cid.readIEEEFloatLE();
	rotation[2].z = cid.readIEEEFloatLE();

	return rotation;
}

} // End of namespace AWE
