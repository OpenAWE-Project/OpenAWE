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

#include <gtest/gtest.h>

#include "src/awe/determinetype.h"

TEST(DetermineType, validType) {
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_staticobject"), kStaticObject);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_dynamicobject"), kDynamicObject);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_taskdefinition"), kTaskDefinition);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_cellinfo"), kCellInfo);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_skeleton"), kSkeleton);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_skeletonsetup"), kSkeletonSetup);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_notebook_page"), kNotebookPage);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_dynamicobjectscript"), kDynamicObjectScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_character"), kCharacter);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_characterclass"), kCharacterClass);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_characterscript"), kCharacterScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_animation"), kAnimation);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_ambientlight"), kAmbientLight);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_scriptinstance"), kScriptInstance);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_trigger"), kTrigger);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_areatrigger"), kAreaTrigger);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_physicsmaterial"), kPhysicsMaterial);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_pointlight"), kPointLight);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_floatingscript"), kFloatingScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_waypoint"), kWaypoint);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_taskcontent"), kTaskContent);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframer"), kKeyframer);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframedobject"), kKeyframedObject);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframeanimation"), kKeyframeAnimation);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframe"), kKeyframe);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_sound"), kSound);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_game_event"), kGameEvent);

	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_scriptinstancescript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_ambientlightscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_triggerscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_areatriggerscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_taskscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_waypointscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframerscript"), kScript);
	EXPECT_EQ(AWE::determineObjectTypeByFilename("cid_keyframedobjectscript"), kScript);
}

TEST(DetermineType, invalidType) {
	EXPECT_ANY_THROW(AWE::determineObjectTypeByFilename("cid_invalid"));
}
