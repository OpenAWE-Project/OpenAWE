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
#include "src/common/strutil.h"
#include "src/common/crc32.h"

#include "src/awe/objectbinaryreadstreamv2.h"

static const uint32_t kDeadBeef   = 0xDEADBEEF;

static constexpr auto kContentHashResourceID           = Common::crc32(Common::toLower("content::ResourceID"));
static constexpr auto kContentHashStaticObject         = Common::crc32(Common::toLower("content::StaticObject"));
static constexpr auto kContentHashDynamicObject        = Common::crc32(Common::toLower("content::DynamicObject"));
static constexpr auto kContentHashDynamicObjectScript  = Common::crc32(Common::toLower("content::DynamicObjectScript"));
static constexpr auto kContentHashCellInfo             = Common::crc32(Common::toLower("content::CellInfo"));
static constexpr auto kContentHashAnimation            = Common::crc32(Common::toLower("content::Animation"));
static constexpr auto kContentHashSkeleton             = Common::crc32(Common::toLower("content::DBSkeleton"));
static constexpr auto kContentHashSound                = Common::crc32(Common::toLower("content::DBSound"));
static constexpr auto kContentHashSkeletonSetup        = Common::crc32(Common::toLower("content::SkeletonSetup"));
static constexpr auto kContentHashNotebookPage         = Common::crc32(Common::toLower("content::NotebookPage"));
static constexpr auto kContentHashCharacter            = Common::crc32(Common::toLower("content::Character"));
static constexpr auto kContentHashCharacterClass       = Common::crc32(Common::toLower("content::DBCharacterClass"));
static constexpr auto kContentHashCharacterScript      = Common::crc32(Common::toLower("content::CharacterScript"));
static constexpr auto kContentHashLoadingScreenHint    = Common::crc32(Common::toLower("content::LoadingScreenHint"));
static constexpr auto kContentHashTaskDefinition       = Common::crc32(Common::toLower("content::TaskDefinition"));
static constexpr auto kContentHashScriptVariables      = Common::crc32(Common::toLower("content::ScriptVariables"));
static constexpr auto kContentHashScript               = Common::crc32(Common::toLower("content::Script"));
static constexpr auto kContentHashScriptInstance       = Common::crc32(Common::toLower("content::ScriptInstance"));
static constexpr auto kContentHashPointLight           = Common::crc32(Common::toLower("content::PointLightInstance"));
static constexpr auto kContentHashSpotLight            = Common::crc32(Common::toLower("content::SpotLightInstance"));
static constexpr auto kContentHashAmbientLight         = Common::crc32(Common::toLower("content::AmbientLightInstance"));
static constexpr auto kContentHashFloatingScript       = Common::crc32(Common::toLower("content::FloatingScript"));
static constexpr auto kContentHashTrigger              = Common::crc32(Common::toLower("content::Trigger"));
static constexpr auto kContentHashAreaTrigger          = Common::crc32(Common::toLower("content::AreaTrigger"));
static constexpr auto kContentHashTaskContent          = Common::crc32(Common::toLower("content::TaskContent"));
static constexpr auto kContentHashAttachmentResources  = Common::crc32(Common::toLower("content::AttachmentResources"));
static constexpr auto kContentHashWaypoint             = Common::crc32(Common::toLower("content::Waypoint"));
static constexpr auto kContentCharacterClothParameters = Common::crc32(Common::toLower("content::Character::ClothParameters"));
static constexpr auto kContentHashAnimationParameters  = Common::crc32(Common::toLower("content::AnimationParameters"));
static constexpr auto kContentHashKeyframedObject      = Common::crc32(Common::toLower("content::KeyframedObject"));
static constexpr auto kContentHashKeyframeAnimation    = Common::crc32(Common::toLower("content::KeyframeAnimation"));
static constexpr auto kContentHashKeyframer            = Common::crc32(Common::toLower("content::Keyframer"));
static constexpr auto kContentHashKeyframe             = Common::crc32(Common::toLower("content::Keyframe"));
static constexpr auto kContentHashGameEvent            = Common::crc32(Common::toLower("content::GameEvent"));
static constexpr auto kContentHashAttachmentContainer  = Common::crc32(Common::toLower("content::AttachmentContainer"));
static constexpr auto kContentHashWeapon               = Common::crc32(Common::toLower("content::Weapon"));

namespace AWE {

ObjectBinaryReadStreamV2::ObjectBinaryReadStreamV2(Common::ReadStream &stream, std::shared_ptr<DPFile> dp) :
ObjectBinaryReadStream(stream, std::move(dp)) {
}

Object ObjectBinaryReadStreamV2::readObject(ObjectType type, unsigned int version) {
	size_t begin = _stream.pos();
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
	ObjectStream::object(object, type, version);

	magicValue = _stream.readUint32LE();
	if (magicValue != kDeadBeef)
		throw Common::Exception("Container missing Deadbeef magic id");

	size_t end = _stream.pos();
	if (end - begin != size)
		throw Common::Exception("Invalid size of object tag");

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
		case kCharacterClass: return kContentHashCharacterClass;
		case kCharacterScript: return kContentHashCharacterScript;
		case kTaskDefinition: return kContentHashTaskDefinition;
		case kTaskContent: return kContentHashTaskContent;
		case kScriptVariables: return kContentHashScriptVariables;
		case kScript: return kContentHashScript;
		case kScriptInstance: return kContentHashScriptInstance;
		case kPointLight: return kContentHashPointLight;
		case kSpotLight: return kContentHashSpotLight;
		case kAmbientLight: return kContentHashAmbientLight;
		case kFloatingScript: return kContentHashFloatingScript;
		case kTrigger: return kContentHashTrigger;
		case kAreaTrigger: return kContentHashAreaTrigger;
		case kAttachmentResources: return kContentHashAttachmentResources;
		case kWaypoint: return kContentHashWaypoint;
		case kAnimationParameters: return kContentHashAnimationParameters;
		case kKeyframedObject: return kContentHashKeyframedObject;
		case kKeyframer: return kContentHashKeyframer;
		case kKeyframeAnimation: return kContentHashKeyframeAnimation;
		case kKeyframe: return kContentHashKeyframe;
		case kGameEvent: return kContentHashGameEvent;
		case kAttachmentContainer: return kContentHashAttachmentContainer;
		case kWeapon: return kContentHashWeapon;
		default: return 0;
	}
}

}
