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

#ifndef OPENAWE_OBJECTSTREAM_H
#define OPENAWE_OBJECTSTREAM_H

#include <memory>
#include <variant>
#include <any>

#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/awe/object.h"
#include "src/awe/dpfile.h"
#include "src/awe/types.h"

namespace AWE {

typedef std::any Object;

class ObjectReadStream{
public:
	virtual Object readObject(ObjectType type, unsigned int version = 0) = 0;

protected:
	ObjectReadStream();
};

class ObjectBinaryReadStream : public ObjectReadStream {
public:
	ObjectBinaryReadStream(Common::ReadStream &stream);
	ObjectBinaryReadStream(Common::ReadStream &stream, std::shared_ptr<DPFile> dp);

protected:
	std::shared_ptr<DPFile> _dp;
	Common::ReadStream &_stream;

	rid_t readRID();
	Common::BoundBox readAABB();
	Templates::StaticObject readStaticObject();
	Templates::DynamicObject readDynamicObject(unsigned int version);
	Templates::DynamicObjectScript readDynamicObjectScript(unsigned int version);
	Templates::CellInfo readCellInfo();
	Templates::Animation readAnimation(unsigned int version);
	Templates::Skeleton readSkeleton();
	Templates::SkeletonSetup readSkeletonSetup();
	Templates::NotebookPage readNotebookPage();
	Templates::Sound readSound();
	Templates::Character readCharacter(unsigned int version);
	Templates::CharacterScript readCharacterScript();
	Templates::CharacterClass readCharacterClass(unsigned int version);
	Templates::TaskDefinition readTaskDefinition(unsigned int version);
	Templates::TaskContent readTaskContent();
	Templates::ScriptVariables readScriptVariables(unsigned int version);
	Templates::Script readScript();
	Templates::ScriptInstance readScriptInstance();
	Templates::PointLight readPointLight(unsigned int version);
	Templates::FloatingScript readFloatingScript();
	Templates::Trigger readTrigger(unsigned int version);
	Templates::AreaTrigger readAreaTrigger();
	Templates::AttachmentResource readAttachmentResources();
	Templates::Waypoint readWaypoint();
	Templates::AnimationParameters readAnimationParameters();
	Templates::KeyFramedObject readKeyFramedObject();
	Templates::KeyFramer readKeyFramer();
	Templates::KeyFrameAnimation readKeyFrameAnimation();
	Templates::KeyFrame readKeyFrame();

	Templates::FileInfoMetadata readFileInfoMetadata();
	Templates::FoliageMeshMetadata readFoliageMeshMetadata();
	Templates::HavokAnimationMetadata readHavokAnimationMetadata();
	Templates::MeshMetadata readMeshMetadata();
	Templates::ParticleSystemMetadata readParticleSystemMetadata();
	Templates::TextureMetadata readTextureMetadata();

	GID readGID();
	glm::vec3 readPosition();
	glm::mat3 readRotation();
};

class ObjectBinaryReadStreamV1 : public ObjectBinaryReadStream {
public:
	ObjectBinaryReadStreamV1(Common::ReadStream &stream, std::shared_ptr<DPFile> dp = std::shared_ptr<DPFile>());

	Object readObject(ObjectType type, unsigned int version = 0) override;
};

class ObjectBinaryReadStreamV2 : public ObjectBinaryReadStream {
public:
	ObjectBinaryReadStreamV2(Common::ReadStream &stream, std::shared_ptr<DPFile> dp = std::shared_ptr<DPFile>());

	Object readObject(ObjectType type, unsigned int version = 0) override;

private:
	uint32_t getContentHash(ObjectType type) const;
};

}

#endif //OPENAWE_OBJECTSTREAM_H
