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
#include "src/common/crc32.h"

#include "src/awe/determinetype.h"

static constexpr uint32_t kCid_StaticObject        = Common::crc32("cid_staticobject");
static constexpr uint32_t kCid_DynamicObject       = Common::crc32("cid_dynamicobject");
static constexpr uint32_t kCid_TaskDefinition      = Common::crc32("cid_taskdefinition");
static constexpr uint32_t kCid_CellInfo            = Common::crc32("cid_cellinfo");
static constexpr uint32_t kCid_Skeleton            = Common::crc32("cid_skeleton");
static constexpr uint32_t kCid_SkeletonSetup       = Common::crc32("cid_skeletonsetup");
static constexpr uint32_t kCid_NotebookPage        = Common::crc32("cid_notebook_page");
static constexpr uint32_t kCid_DynamicObjectScript = Common::crc32("cid_dynamicobjectscript");
static constexpr uint32_t kCid_Character           = Common::crc32("cid_character");
static constexpr uint32_t kCid_CharacterClass      = Common::crc32("cid_characterclass");
static constexpr uint32_t kCid_CharacterScript     = Common::crc32("cid_characterscript");
static constexpr uint32_t kCid_Animation           = Common::crc32("cid_animation");
static constexpr uint32_t kCid_AmbientLight        = Common::crc32("cid_ambientlight");
static constexpr uint32_t kCid_ScriptInstance      = Common::crc32("cid_scriptinstance");
static constexpr uint32_t kCid_Trigger             = Common::crc32("cid_trigger");
static constexpr uint32_t kCid_AreaTrigger         = Common::crc32("cid_areatrigger");
static constexpr uint32_t kCid_PhysicsMaterial     = Common::crc32("cid_physicsmaterial");
static constexpr uint32_t kCid_PointLight          = Common::crc32("cid_pointlight");
static constexpr uint32_t kCid_FloatingScript      = Common::crc32("cid_floatingscript");
static constexpr uint32_t kCid_Waypoint            = Common::crc32("cid_waypoint");
static constexpr uint32_t kCid_TaskContent         = Common::crc32("cid_taskcontent");
static constexpr uint32_t kCid_Keyframer           = Common::crc32("cid_keyframer");
static constexpr uint32_t kCid_KeyframedObject     = Common::crc32("cid_keyframedobject");
static constexpr uint32_t kCid_KeyframeAnimation   = Common::crc32("cid_keyframeanimation");
static constexpr uint32_t kCid_Keyframe            = Common::crc32("cid_keyframe");
static constexpr uint32_t kCid_Sound               = Common::crc32("cid_sound");
static constexpr uint32_t kCid_GameEvent           = Common::crc32("cid_game_event");
static constexpr uint32_t kCid_AttachmentContainer = Common::crc32("cid_attachmentcontainer");

static constexpr uint32_t kCid_ScriptInstanceScript  = Common::crc32("cid_scriptinstancescript");
static constexpr uint32_t kCid_AmbientLightScript    = Common::crc32("cid_ambientlightscript");
static constexpr uint32_t kCid_TriggerScript         = Common::crc32("cid_triggerscript");
static constexpr uint32_t kCid_AreaTriggerScript     = Common::crc32("cid_areatriggerscript");
static constexpr uint32_t kCid_TaskScript            = Common::crc32("cid_taskscript");
static constexpr uint32_t kCid_WaypointScript        = Common::crc32("cid_waypointscript");
static constexpr uint32_t kCid_KeyframerScript       = Common::crc32("cid_keyframerscript");
static constexpr uint32_t kCid_KeyframedObjectScript = Common::crc32("cid_keyframedobjectscript");

namespace AWE {

ObjectType determineObjectTypeByFilename(const std::string &fileName) {
	uint32_t filenameHash = Common::crc32(fileName);
	switch (filenameHash) {
		case kCid_StaticObject:        return kStaticObject;
		case kCid_DynamicObject:       return kDynamicObject;
		case kCid_TaskDefinition:      return kTaskDefinition;
		case kCid_CellInfo:            return kCellInfo;
		case kCid_Skeleton:            return kSkeleton;
		case kCid_SkeletonSetup:       return kSkeletonSetup;
		case kCid_NotebookPage:        return kNotebookPage;
		case kCid_DynamicObjectScript: return kDynamicObjectScript;
		case kCid_Character:           return kCharacter;
		case kCid_CharacterClass:      return kCharacterClass;
		case kCid_CharacterScript:     return kCharacterScript;
		case kCid_Animation:           return kAnimation;
		case kCid_AmbientLight:        return kAmbientLight;
		case kCid_ScriptInstance:      return kScriptInstance;
		case kCid_Trigger:             return kTrigger;
		case kCid_AreaTrigger:         return kAreaTrigger;
		case kCid_PhysicsMaterial:     return kPhysicsMaterial;
		case kCid_PointLight:          return kPointLight;
		case kCid_FloatingScript:      return kFloatingScript;
		case kCid_Waypoint:            return kWaypoint;
		case kCid_TaskContent:         return kTaskContent;
		case kCid_Keyframer:           return kKeyframer;
		case kCid_KeyframedObject:     return kKeyframedObject;
		case kCid_KeyframeAnimation:   return kKeyframeAnimation;
		case kCid_Keyframe:            return kKeyframe;
		case kCid_Sound:               return kSound;
		case kCid_GameEvent:           return kGameEvent;
		case kCid_AttachmentContainer: return kAttachmentContainer;

		case kCid_ScriptInstanceScript:
		case kCid_AmbientLightScript:
		case kCid_TriggerScript:
		case kCid_AreaTriggerScript:
		case kCid_TaskScript:
		case kCid_WaypointScript:
		case kCid_KeyframerScript:
		case kCid_KeyframedObjectScript:
			return kScript;

		default:
			throw Common::Exception("Cannot determine object type from {}", fileName);
	}
}

}
