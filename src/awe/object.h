//
// Created by patrick on 06.04.21.
//

#ifndef OPENAWE_OBJECT_H
#define OPENAWE_OBJECT_H

#include "src/awe/types.h"

namespace AWE::Templates {

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

struct StaticObject {
	glm::vec3 position;
	glm::mat3 rotation;

	rid_t meshResource;
	rid_t physicsResource;
};

struct AnimationParameters {
	float animationBlendTime;
	float halfRotationTime;
	float tiltGain;
	float tiltRegression;
	float tiltAngleRadians;
	float tiltAgility;
	float tiltScaleForward;
	float tiltScaleBackwards;
	uint32_t animationProfile;
};

struct CharacterClass {
	GID gid;
	std::string name;
	std::vector<std::string> baseClasses;
	GID skeletonGid;
	std::string parentName;
	float capsuleHeight;
	float capsuleRadius;
	float lethalDoseOfHitEnergy;
	float healthRecoveryStartDelay;
	float healthRecoveryTime;
	float shadowShieldStrength;
	bool strongShield;
	float kickbackMultiplier;
	float timeBetweenDazzles;
	AnimationParameters animationParameters;
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

struct ScriptVariables {
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
	ScriptVariables script;
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
	ScriptVariables script;
};

struct Animation {
	GID gid;
	GID skeletonGid;
	uint32_t id;
	rid_t rid;
	std::string name;
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

struct Script {
	GID gid;
	ScriptVariables script;
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
	ScriptVariables script;
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

struct AttachmentResource {

};

} // End of namespace AWE::Templates

#endif //OPENAWE_OBJECT_H
