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
#include <iostream>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "src/common/exception.h"
#include "src/common/nurbs.h"
#include "src/common/bitstream.h"

#include "src/awe/havokfile.h"

namespace AWE {

struct Section {
	std::string sectionName;
	uint32_t absoluteDataStart;
	uint32_t localFixupsOffset;
	uint32_t globalFixupsOffset;
	uint32_t virtualFixupsOffset;
	uint32_t exportsOffset;
	uint32_t importsOffset;
	uint32_t endOffset;
};

enum QuantizationType {
	k8Bit = 0,
	k16Bit,
	k32Bit,
	k40Bit,
	k48Bit
};

struct TransformMask {
	uint8_t quantizationTypes;
	uint8_t positionTypes;
	uint8_t rotationTypes;
	uint8_t scaleTypes;

	QuantizationType transformType;
};

HavokFile::HavokFile(Common::ReadStream &binhkx) {
	const uint32_t magicId1 = binhkx.readUint32LE();
	const uint32_t magicId2 = binhkx.readUint32LE();

	if (magicId1 != 0x57E0E057 || magicId2 != 0x10C0C010)
		throw std::runtime_error("Invalid magic id");

	const uint32_t userTag = binhkx.readUint32LE();
	const uint32_t fileVersion = binhkx.readUint32LE();

	binhkx.skip(4);

	const uint32_t numSections = binhkx.readUint32LE();

	const uint32_t contentsSectionIndex = binhkx.readUint32LE();
	const uint32_t contentsSectionOffset = binhkx.readUint32LE();

	const uint32_t classNameSectionIndex = binhkx.readUint32LE();
	const uint32_t classNameSectionOffset = binhkx.readUint32LE();

	std::string versionName = binhkx.readFixedSizeString(15, true);
	setHeader(versionName);
	binhkx.skip(1);

	const uint32_t flags = binhkx.readUint32LE();

	binhkx.skip(4);

	std::vector<Section> sections;
	sections.resize(numSections);
	for (auto &section : sections) {
		section.sectionName = binhkx.readFixedSizeString(19, true);

		binhkx.skip(1);

		section.absoluteDataStart = binhkx.readUint32LE();
		section.localFixupsOffset = binhkx.readUint32LE();
		section.globalFixupsOffset = binhkx.readUint32LE();
		section.virtualFixupsOffset = binhkx.readUint32LE();
		section.exportsOffset = binhkx.readUint32LE();
		section.importsOffset = binhkx.readUint32LE();
		section.endOffset = binhkx.readUint32LE();

		_sectionOffsets.emplace_back(section.absoluteDataStart);
	}

	const Section &classNameSection = sections[classNameSectionIndex];
	const Section &contentsSection = sections[contentsSectionIndex];

	std::map<uint32_t, std::string> classNames;

	binhkx.seek(classNameSection.absoluteDataStart);
	while (true) {
		const uint32_t tag = binhkx.readUint32LE();
		if ((tag & 0xFFu) == 255)
			break;

		binhkx.skip(1);

		const uint32_t position = binhkx.pos() - classNameSection.absoluteDataStart;
		const std::string className = binhkx.readNullTerminatedString();

		classNames[position] = className;
	}

	binhkx.seek(contentsSection.absoluteDataStart + contentsSection.localFixupsOffset);
	while (true) {
		uint32_t address = binhkx.readUint32LE();
		uint32_t targetAddress = binhkx.readUint32LE();

		if (targetAddress == 0xFFFFFFFF || binhkx.eos() || binhkx.pos() - contentsSection.absoluteDataStart > contentsSection.globalFixupsOffset)
			break;

		Fixup fixup;
		/*if (_version == kHavok2010_2_0_r1)
			fixup.targetAddress = targetAddress + _sectionOffsets[contentsSectionIndex];
		else*/
			fixup.targetAddress = targetAddress;
		fixup.section = contentsSectionIndex;
		_fixups[address] = fixup;
		//fmt::print("Local Fixup {} {}\n", address, targetAddress);
	}

	binhkx.seek(contentsSection.absoluteDataStart + contentsSection.globalFixupsOffset);
	while (true) {
		uint32_t address = binhkx.readUint32LE();
		if (address == 0xFFFFFFFF || binhkx.eos() || binhkx.pos() - contentsSection.absoluteDataStart > contentsSection.virtualFixupsOffset)
			break;

		uint32_t section = binhkx.readUint32LE();
		uint32_t metaAddress = binhkx.readUint32LE();

		Fixup fixup;
		fixup.targetAddress = metaAddress;
		fixup.section = section;
		_fixups[address] = fixup;

		//fmt::print("Global Fixup {} {} {}\n", address, metaAddress, section);
	}

	//std::cout << "------" << std::endl;
	binhkx.seek(contentsSection.absoluteDataStart + contentsSection.virtualFixupsOffset);
	while (!binhkx.eos()) {
		uint32_t address = binhkx.readUint32LE();
		uint32_t section = binhkx.readUint32LE();

		if (address == 0xFFFFFFFF)
			break;

		uint32_t nameAddress = binhkx.readUint32LE();
		std::string name = classNames[nameAddress];

		//std::cout << name << std::endl;
		//fmt::print("Address {}\n", contentsSection.absoluteDataStart + address);

		size_t lastPos = binhkx.pos();
		binhkx.seek(contentsSection.absoluteDataStart + address);

		std::any object;
		if (name == "hkaSkeleton")
			object = readHkaSkeleton(binhkx, contentsSectionIndex);
		else if (name == "hkRootLevelContainer")
			readHkRootLevelContainer(binhkx);
		else if (name == "hkaSplineCompressedAnimation")
			object = readHkaSplineCompressedAnimation(binhkx, contentsSectionIndex);
		else if (name == "hkaInterleavedUncompressedAnimation")
			object = readHkaInterleavedUncompressedAnimation(binhkx, contentsSectionIndex);
		else if (name == "hkaDeltaCompressedAnimation")
			object = readHkaDeltaCompressedAnimation(binhkx, contentsSectionIndex);
		else if (name == "hkaAnimationBinding")
			readHkaAnimationBinding(binhkx, contentsSectionIndex);
		else if (name == "hkaAnimationContainer")
			readHkaAnimationContainer(binhkx, contentsSectionIndex);
		else if (name == "hkaDefaultAnimatedReferenceFrame")
			readHkaDefaultAnimatedReferenceFrame(binhkx, contentsSectionIndex);
		else if (name == "hkxScene")
			readHkxScene(binhkx);
		else if (name == "RmdPhysicsSystem")
			_physicsSystem = readRmdPhysicsSystem(binhkx, contentsSectionIndex);
		else if (name == "hkpRigidBody")
			object = readHkpRigidBody(binhkx, contentsSectionIndex);
		else if (name == "hkpBoxShape")
			object = readHkpBoxShape(binhkx);
		else if (name == "hkpCylinderShape")
			object = readHkpCylinderShape(binhkx);
		else if (name == "hkpConvexTranslateShape")
			object = readHkpConvexTranslateShape(binhkx, contentsSectionIndex);
		else if (name == "hkpConvexTransformShape")
			object = readHkpConvexTransformShape(binhkx, contentsSectionIndex);
		else if (name == "hkpListShape")
			object = readHkpListShape(binhkx, contentsSectionIndex);
		else
			spdlog::warn("TODO: Implement havok class {}", name);

		if (object.has_value())
			_objects[contentsSection.absoluteDataStart + address] = object;
		binhkx.seek(lastPos);
	}
}

const HavokFile::hkaAnimationContainer& HavokFile::getAnimationContainer() const {
	return _animationContainer;
}

const HavokFile::RmdPhysicsSystem& HavokFile::getPhysicsSystem() const {
	return _physicsSystem;
}

HavokFile::hkaSkeleton HavokFile::getSkeleton(uint32_t address) {
	auto skeleton = std::any_cast<hkaSkeleton>(_objects[address]);
	return skeleton;
}

HavokFile::hkaAnimation HavokFile::getAnimation(uint32_t address) {
	auto animation = std::any_cast<hkaAnimation>(_objects[address]);
	return animation;
}

HavokFile::hkpRigidBody HavokFile::getRigidBody(uint32_t address) {
	auto rigidBody = std::any_cast<hkpRigidBody>(_objects[address]);
	return rigidBody;
}

HavokFile::hkpShape HavokFile::getShape(uint32_t address) {
	if (_objects.find(address) == _objects.end())
		throw Common::Exception("Couldn't find shape for address {}", address);
	auto shape = std::any_cast<hkpShape>(_objects[address]);
	return shape;
}

std::vector<uint32_t> HavokFile::readUint32Array(Common::ReadStream &binhkx, HavokFile::hkArray array) {
	std::vector<uint32_t> data(array.count);

	if (array.offset == 0xFFFFFFFF)
		return data;

	binhkx.seek(array.offset);
	for (auto &date : data) {
		date = binhkx.readUint32LE();
	}

	return data;
}

std::vector<int16_t> HavokFile::readSint16Array(Common::ReadStream &binhkx, HavokFile::hkArray array) {
	std::vector<int16_t> data(array.count);

	if (array.offset == 0xFFFFFFFF)
		return data;

	binhkx.seek(array.offset);
	for (auto &date : data) {
		date = binhkx.readUint32LE();
	}

	return data;
}

std::vector<uint32_t>
HavokFile::readFixupArray(Common::ReadStream &binhkx, HavokFile::hkArray array, uint32_t section) {
	std::vector<uint32_t> data(array.count);

	if (array.offset == 0xFFFFFFFF)
		return data;

	binhkx.seek(array.offset);
	for (auto &date : data) {
		date = readFixup(binhkx, section);
	}

	return data;
}

std::vector<uint32_t>
HavokFile::readFixupArray(Common::ReadStream &binhkx, uint32_t offset, uint32_t count, uint32_t section) {
	std::vector<uint32_t> data(count);

	binhkx.seek(offset);
	for (auto &date : data) {
		date = readFixup(binhkx, section);
	}

	return data;
}

glm::quat HavokFile::read40BitQuaternion(Common::ReadStream &binhkx) {
	/*
	 * 40 bit Quaternion structure
	 * - 12 bit x signed integer
	 * - 12 bit y signed integer
	 * - 12 bit z signed integer
	 * - 2 bit shift
	 * - 1 bit invert sign
	 * - 1 bit unused?
	 */
	constexpr float fractal = 0.000345436f;

	glm::quat result(0.0f, 0.0f, 0.0f, 0.0f);

	Common::BitStream8LSB bit(binhkx);

	glm::i16vec4 tmp;
	tmp.x = bit.read(12);
	tmp.y = bit.read(12);
	tmp.z = bit.read(12);
	//binhkx.skip(-3);

	const uint8_t shift = bit.read(2);
	const bool invert = bit.read();

	tmp -= 0x0000000000000801;

	glm::vec4 tmp2(
		glm::vec3(tmp) * fractal,
		0.0f
	);
	tmp2.w = std::sqrt(1.0f - glm::dot(tmp2, tmp2));
	if (invert)
		tmp2.w = -tmp2.w;

	for (unsigned int i = 0; i < 3u - shift; ++i) {
		std::swap(tmp2[3 - i], tmp2[2 - i]);
	}

	result.x = tmp2.x;
	result.y = tmp2.y;
	result.z = tmp2.z;
	result.w = tmp2.w;

	return result;
}

HavokFile::hkArray HavokFile::readHkArray(Common::ReadStream &binhkx, uint32_t section) {
	HavokFile::hkArray array;

	array.offset = readFixup(binhkx, section);
	array.count = binhkx.readUint32LE();
	array.capacityAndFlags = binhkx.readUint32LE();

	return array;
}

void HavokFile::readHkRootLevelContainer(Common::ReadStream &binhkx) {
	/*uint32_t dataOffset = readFixup(binhkx);
	uint32_t numElements = binhkx.readUint32LE();

	binhkx.seek(dataOffset);
	for (int i = 0; i < numElements; ++i) {
		uint32_t nameOffset = readFixup(binhkx);
		uint32_t classNameOffset = readFixup(binhkx);

		uint32_t address = readFixup(binhkx);

		size_t lastPos = binhkx.pos();

		binhkx.seek(nameOffset);
		std::string name = binhkx.readNullTerminatedString();
		binhkx.seek(classNameOffset);
		std::string className = binhkx.readNullTerminatedString();

		binhkx.seek(lastPos);
	}*/

	//fmt::print("RootLevelContainer num_elements={}\n", numElements);
}

void HavokFile::readHkxScene(Common::ReadStream &binhkx) {
	binhkx.skip(8);

	float sceneLength = binhkx.readIEEEFloatLE();

	binhkx.skip(0x44);

	std::vector<glm::vec4> appliedTransform(3);
	for (auto &transform : appliedTransform) {
		transform.x = binhkx.readIEEEFloatLE();
		transform.y = binhkx.readIEEEFloatLE();
		transform.z = binhkx.readIEEEFloatLE();
		transform.w = binhkx.readIEEEFloatLE();
	}

	std::string modeller = binhkx.readNullTerminatedString(4);
	std::string asset = binhkx.readNullTerminatedString(4);
}

std::any HavokFile::readHkaSkeleton(Common::ReadStream &binhkx, uint32_t section) {
	hkaSkeleton s;

	switch (_version) {
		case kHavok2010_2_0_r1: {
			binhkx.skip(8);
			uint32_t nameOffset = readFixup(binhkx, section);

			hkArray parentBoneIndicesArray = readHkArray(binhkx, section);
			hkArray boneArray = readHkArray(binhkx, section);
			hkArray transformArray = readHkArray(binhkx, section);
			hkArray referenceFloatArray = readHkArray(binhkx, section);
			hkArray floatSlotArray = readHkArray(binhkx, section);
			hkArray localFrameArray = readHkArray(binhkx, section);
			hkArray partitionArray = readHkArray(binhkx, section);

			binhkx.seek(nameOffset);
			s.name = binhkx.readNullTerminatedString();

			binhkx.seek(parentBoneIndicesArray.offset);
			std::vector<int16_t> parentIndices(parentBoneIndicesArray.count);
			for (auto &parentIndex : parentIndices) {
				parentIndex = binhkx.readUint16LE();
			}

			binhkx.seek(boneArray.offset);
			for (unsigned int i = 0; i < boneArray.count; ++i) {
				uint32_t boneNameOffset = readFixup(binhkx, section);
				bool translationLocked = binhkx.readUint32LE() == 1;

				size_t lastPos = binhkx.pos();
				binhkx.seek(boneNameOffset);
				std::string boneName = binhkx.readNullTerminatedString();
				binhkx.seek(lastPos);

				hkaSkeleton::Bone bone;
				bone.name = boneName;
				bone.translationLocked = translationLocked;
				bone.parentIndex = parentIndices[i];
				s.bones.emplace_back(bone);
			}

			if (transformArray.offset == 0xFFFFFFFF)
				return s;
			binhkx.seek(transformArray.offset);
			for (unsigned int i = 0; i < transformArray.count; ++i) {
				glm::vec3 position;
				position.x = binhkx.readIEEEFloatLE();
				position.y = binhkx.readIEEEFloatLE();
				position.z = binhkx.readIEEEFloatLE();
				binhkx.skip(4);

				glm::quat rotation;
				rotation.x = binhkx.readIEEEFloatLE();
				rotation.y = binhkx.readIEEEFloatLE();
				rotation.z = binhkx.readIEEEFloatLE();
				rotation.w = binhkx.readIEEEFloatLE();

				glm::vec3 scale;
				scale.x = binhkx.readIEEEFloatLE();
				scale.y = binhkx.readIEEEFloatLE();
				scale.z = binhkx.readIEEEFloatLE();
				binhkx.skip(4);

				hkaSkeleton::Bone &bone = s.bones[i];
				bone.position = position;
				bone.scale = scale;
				bone.rotation = rotation;
			}
			break;
		}
		case kHavok550_r1: {
			const uint32_t nameOffset = readFixup(binhkx, section);
			const uint32_t parentIndicesOffset = readFixup(binhkx, section);
			const uint32_t numParentIndices = binhkx.readUint32LE();
			const uint32_t bonesOffset = readFixup(binhkx, section);
			const uint32_t numBones = binhkx.readUint32LE();
			const uint32_t transformOffset = readFixup(binhkx, section);
			const uint32_t numTransforms = binhkx.readUint32LE();

			binhkx.seek(nameOffset);
			s.name = binhkx.readNullTerminatedString();

			binhkx.seek(parentIndicesOffset);
			std::vector<int16_t> parentIndices(numParentIndices);
			for (auto &parentIndex : parentIndices) {
				parentIndex = binhkx.readSint16LE();
			}

			const auto boneOffsets = readFixupArray(binhkx, bonesOffset, numBones, section);
			binhkx.seek(bonesOffset);
			//s.bones.resize(numBones);
			for (const auto &boneOffset : boneOffsets) {
				binhkx.seek(boneOffset);

				auto &bone = s.bones.emplace_back();
				const uint32_t boneNameOffset = readFixup(binhkx, section);
				bone.translationLocked = binhkx.readUint32LE() == 1;

				binhkx.seek(boneNameOffset);
				bone.name = binhkx.readNullTerminatedString();
			}

			for (unsigned int i = 0; i < parentIndices.size(); ++i) {
				s.bones[i].parentIndex = parentIndices[i];
			}

			binhkx.seek(transformOffset);
			for (unsigned int i = 0; i < numTransforms; ++i) {
				auto &bone = s.bones[i];

				bone.position.x = binhkx.readIEEEFloatLE();
				bone.position.y = binhkx.readIEEEFloatLE();
				bone.position.z = binhkx.readIEEEFloatLE();
				binhkx.skip(4);

				bone.rotation.w = binhkx.readIEEEFloatLE();
				bone.rotation.x = binhkx.readIEEEFloatLE();
				bone.rotation.y = binhkx.readIEEEFloatLE();
				bone.rotation.z = binhkx.readIEEEFloatLE();

				bone.scale.x = binhkx.readIEEEFloatLE();
				bone.scale.y = binhkx.readIEEEFloatLE();
				bone.scale.z = binhkx.readIEEEFloatLE();
				binhkx.skip(4);
			}
			break;
		}
	}

	/*binhkx.seek(referenceFloatArray.offset);
	for (int i = 0; i < referenceFloatArray.count; ++i) {
		s.referenceFloats.emplace_back(binhkx.readIEEEFloatLE());
	}*/

	/*std::vector<uint32_t> floatSlotNameAddresses = readFixupArray(binhkx, floatSlotArray, section);
	for (const auto &nameAddress : floatSlotNameAddresses) {
		binhkx.seek(nameAddress);
		std::string name = binhkx.readNullTerminatedString();
		std::cout << name << std::endl;
	}*/

	return s;
}

HavokFile::hkaAnimation HavokFile::readHkaSplineCompressedAnimation(Common::ReadStream &binhkx, uint32_t section) {
	hkaAnimation animation;

	uint32_t unknown0 = binhkx.readUint32LE();
	uint32_t unknown1 = binhkx.readUint32LE();

	uint32_t type = binhkx.readUint32LE();

	animation.duration = binhkx.readIEEEFloatLE();

	uint32_t numTranformTracks = binhkx.readUint32LE();
	uint32_t numFloatTracks = binhkx.readUint32LE();

	uint32_t extractedMotion = readFixup(binhkx, section);

	hkArray annotationTracks = readHkArray(binhkx, section);
	
	const unsigned int numFrames = binhkx.readUint32LE();
	const unsigned int numBlocks = binhkx.readUint32LE();
	const unsigned int maxFramesPerBlock= binhkx.readUint32LE();
	uint32_t maskAndQuantizationSize = binhkx.readUint32LE();
	animation.blockDuration = binhkx.readIEEEFloatLE();
	float blockInverseDuration = binhkx.readIEEEFloatLE();
	animation.frameDuration = binhkx.readIEEEFloatLE();

	hkArray blockOffsetsArray = readHkArray(binhkx, section);
	hkArray floatBlockOffsets = readHkArray(binhkx, section);
	hkArray transformOffsets = readHkArray(binhkx, section);
	hkArray floatOffsets = readHkArray(binhkx, section);
	hkArray data = readHkArray(binhkx, section);

	binhkx.seek(data.offset);
	std::unique_ptr<Common::ReadStream> dataStream(binhkx.readStream(data.count));

	unsigned int pendingFrames = numFrames;
	std::vector<uint32_t> blockOffsets = readUint32Array(binhkx, blockOffsetsArray);
	for (const auto &offset : blockOffsets) {
		dataStream->seek(offset);

		// Read Transform masks
		std::vector<TransformMask> masks(numTranformTracks);
		for (auto &mask : masks) {
			mask.quantizationTypes = dataStream->readByte();
			mask.positionTypes     = dataStream->readByte();
			mask.rotationTypes     = dataStream->readByte();
			mask.scaleTypes        = dataStream->readByte();
		}

		const size_t begin = dataStream->pos();
		const unsigned int numBlockFrames = std::min(pendingFrames, maxFramesPerBlock);
		pendingFrames -= numBlockFrames;
		std::vector<hkaAnimation::Track> tracks;

		for (const auto &mask : masks) {
			const bool transformSplineX = (mask.positionTypes & 0x10) != 0;
			const bool transformSplineY = (mask.positionTypes & 0x20) != 0;
			const bool transformSplineZ = (mask.positionTypes & 0x40) != 0;
			const bool transformStaticX = (mask.positionTypes & 0x01) != 0;
			const bool transformStaticY = (mask.positionTypes & 0x02) != 0;
			const bool transformStaticZ = (mask.positionTypes & 0x04) != 0;
			const bool rotationTypeSpline = (mask.rotationTypes &0xf0) != 0;
			const bool rotationTypeStatic = (mask.rotationTypes &0x0f) != 0;
			const bool scaleSplineX = (mask.scaleTypes & 0x10) != 0;
			const bool scaleSplineY = (mask.scaleTypes & 0x20) != 0;
			const bool scaleSplineZ = (mask.scaleTypes & 0x40) != 0;
			const bool scaleStaticX = (mask.scaleTypes & 0x01) != 0;
			const bool scaleStaticY = (mask.scaleTypes & 0x02) != 0;
			const bool scaleStaticZ = (mask.scaleTypes & 0x04) != 0;

			const bool transformSpline = transformSplineX || transformSplineY || transformSplineZ;
			const bool transformStatic = transformStaticX || transformStaticY || transformStaticZ;
			const bool scaleSpline = scaleSplineX || scaleSplineY || scaleSplineZ;
			const bool scaleStatic = scaleStaticX || scaleStaticY || scaleStaticZ;

			hkaAnimation::Track track{};

			auto positionQuantizationType = QuantizationType(mask.quantizationTypes & 0x03);
			auto rotationQuantizationType = QuantizationType(((mask.quantizationTypes >> 2u) & 0x0f) + 2);
			auto scaleQuantizationType    = QuantizationType((mask.quantizationTypes >> 6u) & 0x03);

			if (transformStatic || transformSpline)
				track.positions = std::vector<glm::vec3>();

			if (transformSpline) {
				uint16_t numItems = dataStream->readUint16LE();
				uint8_t degree = dataStream->readByte();

				std::vector<uint8_t> knots(numItems + degree + 2);
				for (auto &knot : knots) {
					knot = dataStream->readByte();
				}

				if ((dataStream->pos() - begin) % 4 != 0)
					dataStream->skip(4 - (dataStream->pos() - begin) % 4);

				float minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
				float staticx = 0, staticy = 0, staticz = 0;
				if (transformSplineX) {
					minx = dataStream->readIEEEFloatLE();
					maxx = dataStream->readIEEEFloatLE();
				} else if (transformStaticX) {
					staticx = dataStream->readIEEEFloatLE();
				}

				if (transformSplineY) {
					miny = dataStream->readIEEEFloatLE();
					maxy = dataStream->readIEEEFloatLE();
				} else if (transformStaticY) {
					staticy = dataStream->readIEEEFloatLE();
				}

				if (transformSplineZ) {
					minz = dataStream->readIEEEFloatLE();
					maxz = dataStream->readIEEEFloatLE();
				} else if (transformStaticZ) {
					staticz = dataStream->readIEEEFloatLE();
				}

				std::vector<glm::vec3> positionControlPoints;
				for (int i = 0; i <= numItems; ++i) {
					glm::vec3 position(0);
					switch (positionQuantizationType) {
						case k8Bit:
							if (transformSplineX)
								position.x = static_cast<float>(dataStream->readByte()) * (1.0f / 255.0f);
							if (transformSplineY)
								position.y = static_cast<float>(dataStream->readByte()) * (1.0f / 255.0f);
							if (transformSplineZ)
								position.z = static_cast<float>(dataStream->readByte()) * (1.0f / 255.0f);
							break;

						case k16Bit:
							if (transformSplineX)
								position.x = static_cast<float>(dataStream->readUint16LE()) * (1.0f / 65535.0f);
							if (transformSplineY)
								position.y = static_cast<float>(dataStream->readUint16LE()) * (1.0f / 65535.0f);
							if (transformSplineZ)
								position.z = static_cast<float>(dataStream->readUint16LE()) * (1.0f / 65535.0f);
							break;

						default:
							throw std::runtime_error("Invalid Quantization");
					}

					position.x = minx + (maxx - minx) * position.x;
					position.y = miny + (maxy - miny) * position.y;
					position.z = minz + (maxz - minz) * position.z;

					if (!transformSplineX)
						position.x = staticx;
					if (!transformSplineY)
						position.y = staticy;
					if (!transformSplineZ)
						position.z = staticz;

					positionControlPoints.emplace_back(position);
				}

				Common::NURBS<glm::vec3> nurbs(positionControlPoints, knots, degree);
				for (unsigned int i = 0; i < numBlockFrames; ++i) {
					track.positions.emplace_back(nurbs.interpolate(i));
				}

				if ((dataStream->pos() - begin) % 4 != 0)
					dataStream->skip(4 - (dataStream->pos() - begin) % 4);
			} else if(transformStatic) {
				glm::vec3 position(0);
				if (transformStaticX)
					position.x = dataStream->readIEEEFloatLE();
				if (transformStaticY)
					position.y = dataStream->readIEEEFloatLE();
				if (transformStaticZ)
					position.z = dataStream->readIEEEFloatLE();

				track.positions.emplace_back(position);
			}

			if (rotationTypeSpline) {
				uint16_t numItems = dataStream->readUint16LE();
				uint8_t degree = dataStream->readByte();

				std::vector<uint8_t> knots(numItems + degree + 2);
				for (auto &knot : knots) {
					knot = dataStream->readByte();
				}

				std::vector<glm::quat> rotationControlPoints;
				for (int i = 0; i <= numItems; ++i) {
					glm::quat rotation;

					switch (rotationQuantizationType) {
						case k40Bit:
							rotation = read40BitQuaternion(*dataStream);
							break;
						default:
							throw std::runtime_error("Invalid quantization type");
					}

					rotationControlPoints.emplace_back(rotation);
				}

				Common::NURBS<glm::quat> nurbs(rotationControlPoints, knots, degree);
				for (unsigned int i = 0; i < numBlockFrames; ++i) {
					track.rotations.emplace_back(nurbs.interpolate(i));
				}
			} else if (rotationTypeStatic) {
				glm::quat rotation;
				switch (rotationQuantizationType) {
					case k40Bit:
						rotation = read40BitQuaternion(*dataStream);
						break;
					default:
						throw std::runtime_error("Invalid quantization type");
				}
				track.rotations.emplace_back(rotation);
			}

			if ((dataStream->pos() - begin) % 4 != 0)
				dataStream->skip(4 - (dataStream->pos() - begin) % 4);

			if (scaleSpline) {
				throw Common::Exception("TODO: Spline scale not supported yet");
			} else if (scaleStatic) {
				if (scaleStaticX)
					dataStream->skip(4);
				if (scaleStaticY)
					dataStream->skip(4);
				if (scaleStaticZ)
					dataStream->skip(4);

			}

			tracks.emplace_back(track);
		}

		animation.tracks.emplace_back(tracks);
	}

	binhkx.seek(annotationTracks.offset);
	for (unsigned int i = 0; i < annotationTracks.count; ++i) {
		uint32_t nameOffset = readFixup(binhkx, section);
        uint32_t unknown1 = binhkx.readUint32LE();
        //uint32_t offset1 = readFixup(binhkx, section);
        uint32_t unknown2 = binhkx.readUint32LE();
		//uint32_t offset2 = readFixup(binhkx, section);
		float unknown8 = binhkx.readIEEEFloatLE();

		size_t lastPos = binhkx.pos();
		binhkx.seek(nameOffset);
		std::string boneName = binhkx.readNullTerminatedString();
		binhkx.seek(lastPos);

		animation.boneToTrack[boneName] = i;
	}

	return animation;
}

HavokFile::hkaAnimation
HavokFile::readHkaInterleavedUncompressedAnimation(Common::ReadStream &binhkx, uint32_t section) {
	HavokFile::hkaAnimation animation{};

	// TODO

	return animation;
}

HavokFile::hkaAnimation HavokFile::readHkaDeltaCompressedAnimation(Common::ReadStream &binhkx, uint32_t section) {
	HavokFile::hkaAnimation animation{};

	// TODO

	return animation;
}

void HavokFile::readHkaAnimationBinding(Common::ReadStream &binhkx, uint32_t section) {
	hkaAnimationBinding binding;

	binhkx.skip(8);

	uint32_t nameOffset = readFixup(binhkx,  section);
	uint32_t animation = readFixup(binhkx, section);

	hkArray transformTrackToBoneIndexArray = readHkArray(binhkx, section);
	hkArray floatTrackToFloatSlotIndexArray = readHkArray(binhkx, section);
	hkArray partitionIndices = readHkArray(binhkx, section);

	if (nameOffset != 0xFFFFFFFF) {
		binhkx.seek(nameOffset);
		binding.skeletonName = binhkx.readNullTerminatedString();
	}

	binding.animation = animation;

	binding.transformTrackToBoneIndices.resize(transformTrackToBoneIndexArray.count);
	binhkx.seek(transformTrackToBoneIndexArray.offset);
	for (auto &item : binding.transformTrackToBoneIndices) {
		item = binhkx.readSint16LE();
	}
}

void HavokFile::readHkaAnimationContainer(Common::ReadStream &binhkx, uint32_t section) {
	switch (_version) {
		case kHavok2010_2_0_r1: {
			binhkx.skip(8);

			hkArray skeletons = readHkArray(binhkx, section);
			hkArray animations = readHkArray(binhkx, section);
			hkArray bindings = readHkArray(binhkx, section);
			hkArray boneAttachments = readHkArray(binhkx, section);

			_animationContainer.skeletons = readFixupArray(binhkx, skeletons, section);
			_animationContainer.animations = readFixupArray(binhkx, animations, section);
			_animationContainer.bindings = readFixupArray(binhkx, bindings, section);

			break;
		}

		case kHavok550_r1: {
			const uint32_t skeletons = readFixup(binhkx, section);
			const uint32_t numSkeletons = binhkx.readUint32LE();
			const uint32_t animations = readFixup(binhkx, section);
			const uint32_t numAnimations = binhkx.readUint32LE();

			_animationContainer.skeletons = readFixupArray(binhkx, skeletons, numSkeletons, section);
		}
	}
}

void HavokFile::readHkaDefaultAnimatedReferenceFrame(Common::ReadStream &binhkx, uint32_t section) {
	// TODO
}

HavokFile::RmdPhysicsSystem HavokFile::readRmdPhysicsSystem(Common::ReadStream &binhkx, uint32_t section) {
	RmdPhysicsSystem rmdPhysicsSystem{};

	binhkx.skip(8);

	hkArray rigidBodiesArray = readHkArray(binhkx, section);
	hkArray array2 = readHkArray(binhkx, section);
	hkArray array3 = readHkArray(binhkx, section);
	hkArray array4 = readHkArray(binhkx, section);

	binhkx.skip(12);

	hkArray nameArray = readHkArray(binhkx, section);
	hkArray array6 = readHkArray(binhkx, section);
	hkArray array7 = readHkArray(binhkx, section);

	rmdPhysicsSystem.rigidBodies = readFixupArray(binhkx, rigidBodiesArray, section);
	std::vector<uint32_t> nameOffsets = readFixupArray(binhkx, nameArray, section);
	std::vector<std::string> names;
	for (const auto &offset : nameOffsets) {
		binhkx.seek(offset);
		names.emplace_back(binhkx.readNullTerminatedString());
	}

	return rmdPhysicsSystem;
}

HavokFile::hkpRigidBody HavokFile::readHkpRigidBody(Common::ReadStream &binhkx, uint32_t section) {
	hkpRigidBody rigidBody{};

	binhkx.skip(16);

	switch (_version) {
		case kHavok550_r1: {
			rigidBody.shape = readFixup(binhkx, section);
			binhkx.skip(92);
			uint32_t nameFixup = readFixup(binhkx, section);

			binhkx.seek(nameFixup);
			const std::string name = binhkx.readNullTerminatedString();

			break;
		}

		case kHavok2010_2_0_r1: {
			rigidBody.shape = readFixup(binhkx, section);
			binhkx.skip(100);
			uint32_t nameFixup = readFixup(binhkx, section);
			binhkx.skip(24);
			float friction = binhkx.readIEEEFloatLE();
			float restitution = binhkx.readIEEEFloatLE();

			binhkx.skip(132);

			rigidBody.position.x = binhkx.readIEEEFloatLE();
			rigidBody.position.y = binhkx.readIEEEFloatLE();
			rigidBody.position.z = binhkx.readIEEEFloatLE();
			rigidBody.position.w = binhkx.readIEEEFloatLE();

			binhkx.skip(4 * 4 * 2);

			rigidBody.rotation.x = binhkx.readIEEEFloatLE();
			rigidBody.rotation.y = binhkx.readIEEEFloatLE();
			rigidBody.rotation.z = binhkx.readIEEEFloatLE();
			rigidBody.rotation.w = binhkx.readIEEEFloatLE();

			binhkx.seek(nameFixup);
			std::string name = binhkx.readNullTerminatedString();
			break;
		}
	}

	return rigidBody;
}

HavokFile::hkpShape HavokFile::readHkpBoxShape(Common::ReadStream &binhkx) {
	hkpShape shape{};
	hkpBoxShape boxShape{};

	shape.type = kBox;

	binhkx.skip(8); // hkReferencedObject
	shape.userData = binhkx.readUint64LE(); // hkpShape
	shape.radius = binhkx.readIEEEFloatLE(); // hkpConvexShape
	binhkx.skip(12);

	glm::vec4 halfExtents;
	halfExtents.x = binhkx.readIEEEFloatLE();
	halfExtents.y = binhkx.readIEEEFloatLE();
	halfExtents.z = binhkx.readIEEEFloatLE();
	halfExtents.w = binhkx.readIEEEFloatLE();
	boxShape.halfExtents = halfExtents;
	shape.shape = boxShape;

	return shape;
}

HavokFile::hkpShape HavokFile::readHkpCylinderShape(Common::ReadStream &binhkx) {
	hkpShape shape{};
	hkpCylinderShape cylinderShape{};

	shape.type = kCylinder;

	binhkx.skip(8); // hkReferencedObject
	shape.userData = binhkx.readUint64LE(); // hkpShape
	shape.radius = binhkx.readIEEEFloatLE(); // hkpConvexShape
	//binhkx.skip(0);

	cylinderShape.radius = binhkx.readIEEEFloatLE();
	binhkx.skip(12);
	cylinderShape.p1.x = binhkx.readIEEEFloatLE();
	cylinderShape.p1.y = binhkx.readIEEEFloatLE();
	cylinderShape.p1.z = binhkx.readIEEEFloatLE();
	cylinderShape.p1.w = binhkx.readIEEEFloatLE();
	cylinderShape.p2.x = binhkx.readIEEEFloatLE();
	cylinderShape.p2.y = binhkx.readIEEEFloatLE();
	cylinderShape.p2.z = binhkx.readIEEEFloatLE();
	cylinderShape.p2.w = binhkx.readIEEEFloatLE();

	shape.shape = cylinderShape;

	return shape;
}

HavokFile::hkpShape HavokFile::readHkpConvexTranslateShape(Common::ReadStream &binhkx, uint32_t section) {
	hkpShape shape{};
	hkpConvexTranslateShape convexTranslateShape{};

	shape.type = kConvexTranslate;

	binhkx.skip(8); // hkReferencedObject
	shape.userData = binhkx.readUint64LE(); // hkpShape
	shape.radius = binhkx.readIEEEFloatLE(); // hkpConvexShape
	binhkx.skip(4);
	convexTranslateShape.shape = readFixup(binhkx, section);

	binhkx.skip(4);
	convexTranslateShape.translation.x = binhkx.readIEEEFloatLE();
	convexTranslateShape.translation.y = binhkx.readIEEEFloatLE();
	convexTranslateShape.translation.z = binhkx.readIEEEFloatLE();
	convexTranslateShape.translation.w = binhkx.readIEEEFloatLE();

	shape.shape = convexTranslateShape;

	return shape;
}


HavokFile::hkpShape HavokFile::readHkpConvexTransformShape(Common::ReadStream &binhkx, uint32_t section) {
	hkpShape shape{};
	hkpConvexTransformShape convexTransformShape{};

	shape.type = kConvexTransform;

	binhkx.skip(8); // hkReferencedObject
	shape.userData = binhkx.readUint64LE(); // hkpShape
	shape.radius = binhkx.readIEEEFloatLE(); // hkpConvexShape
	binhkx.skip(4);
	convexTransformShape.shape = readFixup(binhkx, section);

	binhkx.skip(4);

	for (int i = 0; i < 3; ++i) {
		convexTransformShape.rotation[i].x = binhkx.readIEEEFloatLE();
		convexTransformShape.rotation[i].y = binhkx.readIEEEFloatLE();
		convexTransformShape.rotation[i].z = binhkx.readIEEEFloatLE();

		binhkx.skip(4);
	}

	convexTransformShape.translation.x = binhkx.readIEEEFloatLE();
	convexTransformShape.translation.y = binhkx.readIEEEFloatLE();
	convexTransformShape.translation.z = binhkx.readIEEEFloatLE();
	convexTransformShape.translation.w = binhkx.readIEEEFloatLE();

	shape.shape = convexTransformShape;

	return shape;
}

HavokFile::hkpShape HavokFile::readHkpListShape(Common::ReadStream &binhkx, uint32_t section) {
	hkpShape shape{};
	hkpListShape listShape{};

	shape.type = kList;

	binhkx.skip(24);
	hkArray shapeArray = readHkArray(binhkx, section);
	binhkx.seek(shapeArray.offset);
	listShape.shapes.resize(shapeArray.count);
	for (auto &shapeRef: listShape.shapes) {
		shapeRef = readFixup(binhkx, section);
		binhkx.skip(12);
	}

	shape.shape = listShape;

	return shape;
}

void HavokFile::setHeader(std::string headerVersion) {
	if (headerVersion == "Havok-5.5.0-r1")
		_version = kHavok550_r1;
	else if (headerVersion == "hk_2010.2.0-r1")
		_version = kHavok2010_2_0_r1;
	else
		throw std::runtime_error(fmt::format("Unsupported havok version {}", headerVersion));

}

uint32_t HavokFile::readFixup(Common::ReadStream &binhkx, uint32_t section) {
	if (_fixups.find(binhkx.pos() - _sectionOffsets[section]) == _fixups.end()) {
		binhkx.skip(4);
		return 0xffffffff;
	}

	Fixup fixup = _fixups[binhkx.pos() - _sectionOffsets[section]];
	if (fixup.targetAddress == 0)
		throw std::runtime_error("Invalid fixup");

	binhkx.skip(4);

	if (fixup.section)
		fixup.targetAddress += _sectionOffsets[*fixup.section];

	return fixup.targetAddress;
}

} // End of namespace AWE
