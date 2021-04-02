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

#ifndef AWE_CIDFILE_H
#define AWE_CIDFILE_H

#include <variant>
#include <optional>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/detail/type_quat.hpp>

#include "src/common/readstream.h"

#include "dpfile.h"
#include "types.h"

namespace AWE {

/*!
 * \brief Load files prefixed with cid_
 *
 * The files prefixed with cid_ contain the major data
 * of Alan Wake and Alan Wakes American Nightmare and use
 * mostly the same format. Exceptions from that rule are
 * for example cid_roadmap.bin files or cid_terraindata.bin
 * files. Alan Wake and Alan Wakes American Nightmare have
 * different binary formats for cid files. Complete groups
 * of files can have a corresponding dp_ prefixed file for
 * storing larger amounts of data.
 */
class CIDFile {
public:
	enum DataType {
		kRID,
		kCellInfo,
		kNotebookPage,
		kSkeleton,
		kSkeletonSetup,
		kStaticObject,
		kDynamicObject,
		kDynamicObjectScript,
		kCharacter,
		kCharacterClass,
		kCharacterScript,
		kScript,
		kAnimation,
		kAmbientLight,
		kScriptInstance,
		kScriptInstanceScript,
		kTrigger,
		kAreaTrigger,
		kPhysicsMaterial,
		kFileInfoMetadata,
		kResourceMetadata,
		kFoliageMeshMetadata,
		kHavokAnimationMetadata,
		kMeshMetadata,
		kParticleSystemMetadata,
		kTextureMetadata,
		kAABB,
		kPointLight,
		kFloatingScript,
		kWaypoint,
		kTaskDefinition,
		kTaskContent,
		kKeyFramer,
		kSound
	};

	struct Skeleton {
		std::string name;
		uint32_t id;
		rid_t rid;
		GID gid;
	};

	struct SkeletonSetup {
		GID rootBoneGid;
		std::string identifier;
	};

	struct CellInfo {
		uint32_t x, y;
		int32_t lowDetailFoliageCount, highDetailFoliageCount;
	};

	struct RID {
		rid_t rid;
	};

	struct StaticObject {
		glm::vec3 position;
		glm::mat3 rotation;

		rid_t meshResource;
		rid_t physicsResource;
	};

	struct CharacterClass {
		GID gid;
		std::string name;
		std::vector<std::string> baseClasses;
		GID skeletonGid;
	};

	struct Character {
		GID gid;
		GID classGid;
		std::string identifier;
		glm::vec3 position;
		glm::mat3 rotation;

		rid_t meshResource;
		rid_t fxaResource;
		rid_t animgraphResource;
		rid_t clothResource;
	};

	struct DynamicObject {
		GID gid;
		glm::vec3 position;
		glm::mat3 rotation;
		rid_t meshResource;
		rid_t physicsResource;
		std::string resourcePath;
		std::string identifier;
	};

	struct Script {
		uint32_t offsetCode;
		uint32_t offsetHandlers;
		uint32_t offset3;
		uint32_t offsetSignals;
		uint32_t offset5;
		uint32_t codeSize;
		uint32_t numHandlers;
		uint32_t size3;
		uint32_t numSignals;
		uint32_t size5;
	};

	struct DynamicObjectScript {
		GID gid;
		Script script;
		std::string name;
	};

	struct NotebookPage {
		GID gid;
		std::string name;
		uint32_t episodeNumber;
		uint32_t id;
		bool onlyInNightmare;
	};

	struct CharacterScript {
		GID gid;
		Script script;
	};

	struct Animation {
		GID gid;
		GID skeletonGid;
		uint32_t id;
		rid_t rid;
		std::string name;
	};

	struct AmbientLight {

	};

	struct AreaTrigger {
		GID gid;
		std::string identifier;
		std::vector<glm::vec2> positions;
	};

	struct ScriptInstance {
		GID gid, gid2;
		glm::vec3 position;
		glm::mat3 rotation;
	};

	struct ObjectScript {
		GID gid;
		Script script;
	};

	struct PhysicsMaterial {
		GID gid;
		byte index;
		std::string name;
	};

	struct FileInfoMetadata {
		uint32_t fileSize;
		uint32_t fileDataCRC;
		uint32_t flags;
	};

	struct ResourceMetadata {
		rid_t rid;
		uint32_t resourceType;
	};

	struct FoliageMeshMetadata {
		uint32_t vertexBufferBytes;
		uint32_t indexCount;
		Common::BoundBox boundBox;
		std::vector<rid_t> textureRids;
	};

	struct HavokAnimationMetadata {
		std::string animationEventPath;
	};

	struct MeshMetadata {
		uint32_t vertexBufferBytes;
		uint32_t indexCount;
		Common::BoundBox boundBox;
		bool hasBones;
		std::vector<rid_t> textureRids;
	};

	struct ParticleSystemMetadata {
		std::vector<rid_t> textureRids;
	};

	struct TextureMetadata {
		uint32_t type;
		uint32_t format;
		uint32_t filter;
		uint32_t width, height;
		uint32_t depth;
		std::vector<uint32_t> mipmapOffsets;
		float highDetailStreamDistance;
		bool useTextureLOD;
	};

	struct AABB {
		Common::BoundBox boundBox;
	};

	struct PointLight {
		GID gid;
		GID gid2;
		glm::mat3x3 rotation;
		glm::vec3 position;
		rid_t meshRid;
		rid_t staticShadowMapRid;
	};

	struct Trigger {
		GID gid, gid2;
		std::string localeString, identifier;
	};

	struct FloatingScript {
		GID gid;
		Script script;
		glm::mat3 rotation;
		glm::vec3 position;
	};

	struct Waypoint {
		GID gid;
		glm::mat3 rotation;
		glm::vec3 position;
	};

	struct TaskDefinition {
		GID gid;
		std::string name;
		std::string cinematic;
		glm::mat3 rotation;
		glm::vec3 position;
		glm::mat3 rotationPlayer;
		glm::vec3 positionPlayer;
		bool rootTask;
		bool activateOnStartup;
	};

	struct TaskContent {
		std::vector<rid_t> rids;
	};

	struct KeyFramer {
		GID gid;
	};

	struct Sound {
		GID gid;
		bool threed;
		bool streamed;
		uint32_t looping;
		float volume;
		float hotspot;
		float falloff;
		float volumeVariation;
		float frequencyVariation;
		float occlusion;
		float pitch;
		rid_t rid;
	};

	typedef std::variant<
			Skeleton,
			CellInfo,
			StaticObject,
			RID,
			Character,
			CharacterClass,
			NotebookPage,
			DynamicObject,
			DynamicObjectScript,
			Script,
			Animation,
			ScriptInstance,
			ObjectScript,
			AreaTrigger,
			PhysicsMaterial,
			FileInfoMetadata,
			ResourceMetadata,
			FoliageMeshMetadata,
			HavokAnimationMetadata,
			MeshMetadata,
			ParticleSystemMetadata,
			TextureMetadata,
			CharacterScript,
			AABB,
			PointLight,
			Trigger,
			FloatingScript,
			Waypoint,
			TaskDefinition,
			TaskContent,
			KeyFramer,
			Sound
	> Data;

	struct Container {
		DataType type;
		Data data;
	};

	CIDFile(Common::ReadStream &cid, DPFile *dp = nullptr);

	CIDFile(Common::ReadStream &cid, unsigned int numEntries);

	CIDFile(Common::ReadStream &cid, unsigned int numEntries, DataType type);

	[[nodiscard]] const std::vector<Container> &getContainers() const;

private:
	enum FileFormat {
		kSimple,
		kStructured,
		kStructuredV2
	};

	void testFormat(Common::ReadStream &cid);

	Container readContainer(Common::ReadStream &cid);

	Container readSimpleContainer(Common::ReadStream &cid, DataType type);

	StaticObject readStaticObject(Common::ReadStream &cid);

	DynamicObject readDynamicObject(Common::ReadStream &cid);

	DynamicObjectScript readDynamicObjectScript(Common::ReadStream &cid);

	CellInfo readCellInfo(Common::ReadStream &cid);

	RID readRID(Common::ReadStream &cid);

	Skeleton readSkeleton(Common::ReadStream &cid);

	Character readCharacter(Common::ReadStream &cid);

	CharacterClass readCharacterClass(Common::ReadStream &cid);

	CharacterScript readCharacterScript(Common::ReadStream &cid);

	NotebookPage readNotebookPage(Common::ReadStream &cid);

	Script readScript(Common::ReadStream &cid);

	Animation readAnimation(Common::ReadStream &cid);

	ScriptInstance readScriptInstance(Common::ReadStream &cid);

	ObjectScript readObjectScript(Common::ReadStream &cid);

	AreaTrigger readAreaTrigger(Common::ReadStream &cid);

	PhysicsMaterial readPhysicsMaterial(Common::ReadStream &cid);

	FileInfoMetadata readFileInfoMetadata(Common::ReadStream &cid);

	ResourceMetadata readResourceMetadata(Common::ReadStream &cid);

	FoliageMeshMetadata readFoliageMeshMetadata(Common::ReadStream &cid);

	HavokAnimationMetadata readHavokAnimationMetadata(Common::ReadStream &cid);

	MeshMetadata readMeshMetadata(Common::ReadStream &cid);

	ParticleSystemMetadata readParticleSystemMetadata(Common::ReadStream &cid);

	TextureMetadata readTextureMetadata(Common::ReadStream &cid);

	AABB readAABB(Common::ReadStream &cid);

	PointLight readPointLight(Common::ReadStream &cid);

	Trigger readTrigger(Common::ReadStream &cid);

	FloatingScript readFloatingScript(Common::ReadStream &cid);

	Waypoint readWaypoint(Common::ReadStream &cid);

	TaskDefinition readTaskDefinition(Common::ReadStream &cid);

	TaskContent readTaskContent(Common::ReadStream &cid);

	KeyFramer readKeyFramer(Common::ReadStream &cid);

	Sound readSound(Common::ReadStream &cid);

	// Utility Functions
	//

	GID readGID(Common::ReadStream &cid);

	glm::vec3 readPosition(Common::ReadStream &cid);
	glm::mat3 readRotation(Common::ReadStream &cid);

	FileFormat _format;

	DPFile *_dp;
	std::vector<Container> _containers;
};

} // End of namespace AWE

#endif //AWE_CIDFILE_H
