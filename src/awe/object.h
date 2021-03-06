//
// Created by patrick on 06.04.21.
//

#ifndef OPENAWE_OBJECT_H
#define OPENAWE_OBJECT_H

#include <vector>

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
	std::vector<ObjectID> animations;
	std::string type;
};

struct Character {
	GID gid;
	GID classGid;
	std::string identifier;
	glm::vec3 position;
	glm::mat3 rotation;
	std::vector<rid_t> resources;

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
	ObjectID attachmentContainer;
};

struct AttachmentContainer {
	std::vector<GID> spotLights;
	std::vector<GID> particleSystems;
	std::vector<GID> soundInstances;
	std::vector<GID> unk4;
	std::vector<GID> pointLights;
	std::vector<GID> ambientLights;
	std::vector<GID> triggers;
	std::vector<GID> scriptInstances;
	std::vector<GID> lensFlares;
};

struct ScriptVariables {
	uint32_t offsetCode;
	uint32_t offsetHandlers;
	uint32_t offsetVariables;
	uint32_t offsetSignals;
	uint32_t offsetDebugEntries;
	uint32_t codeSize;
	uint32_t numHandlers;
	uint32_t numVariables;
	uint32_t numSignals;
	uint32_t numDebugEntries;
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
	bool useFingersLeft;
	bool useFingersRight;
	bool useFootIK;
	bool attachLeftHand;
	uint32_t legSyncLoopCount;
	bool scriptedBlendIn;
	float scriptedBlendInTime;
	bool scriptedBlendOut;
	bool scriptedMoveCapsule;
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
	std::vector<int32_t> mipmapOffsets;
	float highDetailStreamDistance;
	bool useTextureLOD;
};

struct PointLight {
	GID gid;
	GID gid2;
	glm::mat3x3 rotation;
	glm::vec3 position;
	glm::vec3 color;
	float decay;
	float directionalFalloff;
	bool autoStart;
	bool castShadows;
	float intensity;
	rid_t meshRid;
	rid_t staticShadowMapRid;
	glm::mat3 meshRotation;
	glm::vec3 meshPosition;
	float drainMultiplier;
	bool enableSpecular;
	float shadowMapRange;
	bool enableRangeClip;
	float rangeClip;
};

struct AmbientLightInstance {
	GID gid, scriptGid;
	glm::vec3 position;
	glm::vec3 color;
	float decay;
	float intensity;
	bool autoStart;
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
	bool topLevelTask;
	bool activateOnStartup;
	std::vector<bool> activateOnStartupRound;
	std::vector<GID> playerCharacter;
};

struct TaskContent {
	std::vector<rid_t> rids;
};

struct KeyFramer {
	GID gid;
	ObjectID parentKeyFramer;
	ObjectID attachmentContainer;
	std::vector<ObjectID> keyFrames;
	std::vector<ObjectID> keyFrameAnimations;
	std::vector<rid_t> resources;
	uint32_t initialKeyframe;
};

struct KeyFramedObject {
	glm::mat3 rotation;
	glm::vec3 position;
	rid_t meshResource;
	rid_t physicsResource;
	GID gid;
	ObjectID keyFramer;
	glm::mat3 rotation2;
	glm::vec3 position2;
	uint32_t flags;
	std::vector<rid_t> resources;
	std::string name;
	std::string source;
};

struct KeyFrameAnimation {
	GID gid;
	GID nextAnimation;
	unsigned int startKeyFrame;
	unsigned int endKeyFrame;
	float length;
	rid_t animationResource;
};

struct KeyFrame {
	glm::vec3 position;
	glm::mat3 rotation;
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

struct GameEvent {
	ScriptVariables scriptVariables;
	GID gid;
	std::string name;
};

} // End of namespace AWE::Templates

#endif //OPENAWE_OBJECT_H
