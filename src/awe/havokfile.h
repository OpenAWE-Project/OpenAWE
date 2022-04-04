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

#ifndef AWE_HAVOKFILE_H
#define AWE_HAVOKFILE_H

#include <map>
#include <any>
#include <vector>
#include <optional>
#include <variant>

#include <glm/detail/type_quat.hpp>

#include "src/common/readstream.h"

namespace AWE {

class HavokFile {
public:
	enum Version {
		kHavok550_r1,
		kHavok2010_2_0_r1
	};

	enum ShapeType {
		kBox,
		kCylinder,
		kConvexTranslate,
		kList
	};

	struct hkNamedVariant {
		std::string name;
		std::string className;
		uint32_t pointer;
	};

	struct hkRootLevelContainer {
		std::vector<hkNamedVariant> variants;
	};

	struct hkaAnimationContainer {
		std::vector<uint32_t> skeletons;
		std::vector<uint32_t> animations;
		std::vector<uint32_t> bindings;
	};

	struct hkaSkeleton {
		struct Bone {
			std::string name;
			glm::vec3 position;
			glm::vec3 scale;
			glm::quat rotation;
			int16_t parentIndex;
			bool translationLocked;
		};

		std::string name;
		std::vector<Bone> bones;
		std::vector<float> referenceFloats;
	};

	struct hkaAnimation {
		struct Track {
			std::vector<glm::vec3> positions;
			std::vector<glm::quat> rotations;
		};
		float duration;
		float blockDuration;
		float frameDuration;
		std::vector<std::vector<Track>> tracks;
		std::map<std::string, size_t> boneToTrack;
	};

	struct hkaAnimationBinding {
		std::string skeletonName;
		uint32_t animation;
		std::vector<int16_t> transformTrackToBoneIndices;
	};

	struct hkpRigidBody {
		uint32_t shape;
		glm::vec4 position;
		glm::quat rotation;
	};

	struct hkpBoxShape {
		glm::vec4 halfExtents;
	};

	struct hkpCylinderShape {
		float radius;
		glm::vec4 p1, p2;
	};

	struct hkpConvexTranslateShape {
		uint32_t shape;
		glm::vec4 translation;
	};

	struct hkpListShape {
		std::vector<uint32_t> shapes;
	};

	struct hkpShape {
		uint64_t userData;
		float radius;
		std::variant<
			hkpBoxShape,
			hkpCylinderShape,
			hkpConvexTranslateShape,
			hkpListShape
		> shape;
		ShapeType type;
	};

	struct RmdPhysicsSystem {
		std::vector<uint32_t> rigidBodies;
	};

	HavokFile(Common::ReadStream &binhkx);

	const hkaAnimationContainer& getAnimationContainer() const;
	const RmdPhysicsSystem& getPhysicsSystem() const;

	hkaSkeleton getSkeleton(uint32_t address);
	hkaAnimation getAnimation(uint32_t address);

	hkpRigidBody getRigidBody(uint32_t address);
	hkpShape getShape(uint32_t address);

private:
	struct Fixup {
		uint32_t targetAddress;
		std::optional<uint32_t> section;
	};

	struct hkArray {
		uint32_t offset;
		uint32_t count;
		uint32_t capacityAndFlags;
	};

	Version _version;

	std::vector<uint32_t> _sectionOffsets;
	std::map<uint32_t, Fixup> _fixups;
	std::map<uint32_t, std::any> _objects;

	hkaAnimationContainer _animationContainer;
	RmdPhysicsSystem _physicsSystem;

	std::vector<uint32_t> readUint32Array(Common::ReadStream &binhkx, hkArray array);
	std::vector<int16_t> readSint16Array(Common::ReadStream &binhkx, hkArray array);
	std::vector<uint32_t>
	readFixupArray(Common::ReadStream &binhkx, HavokFile::hkArray array, uint32_t section);
	std::vector<uint32_t>
	readFixupArray(Common::ReadStream &binhkx, uint32_t offset, uint32_t count, uint32_t section);
	glm::quat read40BitQuaternion(Common::ReadStream &binhkx);

	void readHkRootLevelContainer(Common::ReadStream &binhkx);
	HavokFile::hkArray readHkArray(Common::ReadStream &binhkx, uint32_t section);

	void readHkxScene(Common::ReadStream &binhkx);

	std::any readHkaSkeleton(Common::ReadStream &binhkx, uint32_t section);
	hkaAnimation readHkaSplineCompressedAnimation(Common::ReadStream &binhkx, uint32_t section);
	hkaAnimation readHkaInterleavedUncompressedAnimation(Common::ReadStream &binhkx, uint32_t section);
	hkaAnimation readHkaDeltaCompressedAnimation(Common::ReadStream &binhkx, uint32_t section);
	void readHkaAnimationBinding(Common::ReadStream &binhkx, uint32_t section);
	void readHkaAnimationContainer(Common::ReadStream &binhkx, uint32_t section);
	void readHkaDefaultAnimatedReferenceFrame(Common::ReadStream &binhkx, uint32_t section);

	hkpRigidBody readHkpRigidBody(Common::ReadStream &binhkx, uint32_t section);
	HavokFile::hkpShape readHkpBoxShape(Common::ReadStream &binhkx);
	HavokFile::hkpShape readHkpCylinderShape(Common::ReadStream &binhkx);
	HavokFile::hkpShape readHkpConvexTranslateShape(Common::ReadStream &binhkx, uint32_t section);
	HavokFile::hkpShape readHkpListShape(Common::ReadStream &binhkx, uint32_t section);

	RmdPhysicsSystem readRmdPhysicsSystem(Common::ReadStream &binhkx, uint32_t section);

	void setHeader(std::string headerVersion);

	uint32_t readFixup(Common::ReadStream &binhkx, uint32_t section);

};

} // End of namespace AWE

#endif //AWE_HAVOKFILE_H
