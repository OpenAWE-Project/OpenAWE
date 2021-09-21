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

#include "src/common/exception.h"

#include "src/awe/objectbinaryreadstreamv1.h"

namespace AWE {

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
		case kCharacterClass: object = readCharacterClass(version); break;
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
		case kAnimationParameters: object = readAnimationParameters(); break;
		case kKeyframedObject: object = readKeyFramedObject(); break;

		case kFileInfoMetadata: object = readFileInfoMetadata(); break;
		case kFoliageMeshMetadata: object = readFoliageMeshMetadata(); break;
		case kHavokAnimationMetadata: object = readHavokAnimationMetadata(); break;
		case kTextureMetadata: object = readTextureMetadata(); break;
		case kParticleSystemMetadata: object = readParticleSystemMetadata(); break;
		case kMeshMetadata: object = readMeshMetadata(); break;

		default: throw Common::Exception("Unsupported content type");
	}

	return object;
}

} // End of namespace AWE
