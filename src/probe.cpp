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

#include <bitset>

#include <spdlog/spdlog.h>

#include "src/awe/resman.h"
#include "src/awe/rmdparchive.h"
#include "src/common/exception.h"
#include "src/probe.h"

Probe::Probe() {}

ProbeResult Probe::performProbe() {
	spdlog::info("Probing archives...");
	std::bitset<8> allChecks = 
		checkAlanWake() |
		checkAmericanNightmare() |
		checkQuantumBreak() |
		checkAlanWakeRemastered() |
		checkControl();
	if (allChecks.none())
		return kResultUnknown;
	else if (allChecks.count() == 1)
		return static_cast<ProbeResult>(allChecks.to_ulong());
	else
		throw CreateException("Ambiguous archives: probe failed to recognise a certain game");	 	
}

ProbeResult Probe::checkAlanWake() {
	// check if old Alan Wake's textures are present in files
	bool hasAlanWake2008 = ResMan.hasDirectory("textures/characters/alanwake_2008");
	// check for Stucky's texture variation
	bool hasStuckyVar02 = ResMan.hasDirectory("textures/characters/stucky_var02");
	// check for sponsored ads
	bool hasVerizonCellPhone = 
		ResMan.hasResource("textures/objects/items/misc/cellphone_verizon_n.tex") &&
		ResMan.hasResource("textures/objects/items/misc/cellphone_verizon.tex");
	bool hasEnergizerBatteries = 
		ResMan.hasResource("textures/objects/items/lightsources/energizer_flashlight_test.tex") &&
		ResMan.hasResource("textures/objects/items/lightsources/energizer_lithium.tex") &&
		ResMan.hasResource("textures/objects/items/lightsources/energizer_max.tex");
	// check for "gameplay_example" meshes
	bool hasGameplayExample = 
		ResMan.hasResource("objects/backdrop/gameplay_example_0_0.binmsh") &&
		ResMan.hasResource("objects/backdrop/gameplay_example_3_3.binmsh");
	// check for test breakable table mesh
	bool hasBreakableTableTest = ResMan.hasResource("objects/test/breakabletable_mesh.binmsh");
	// check for Arial font
	bool hasArial = ResMan.hasResource("fonts/Arial.binfnt");

	if (hasArial && hasAlanWake2008 && hasBreakableTableTest &&
		hasEnergizerBatteries && hasGameplayExample &&
		hasStuckyVar02 && hasVerizonCellPhone) {
		spdlog::debug("Looks like Alan Wake!");
		return kResultAlanWake;
	} else {
		spdlog::debug("Doesn't look like Alan Wake...");
		return kResultUnknown;
	}
}

ProbeResult Probe::checkAlanWakeRemastered() {
	// check for new deerfest parade truck textures
	bool hasParadefloat = ResMan.hasDirectory("textures/objects/vehicles/new/paradefloat/paradefloat");
	// check for Rose's trailer items
	bool hasRosesTrailer = 
		ResMan.hasResource("textures/objects/structures/complexes/trailerpark/rosestrailerpropsd3t.tex") &&
		ResMan.hasResource("textures/objects/structures/complexes/trailerpark/rosepillow_d3t.tex") &&
		ResMan.hasResource("textures/objects/structures/complexes/trailerpark/rosebedsheet_d3t.tex");
	// check for complex Andersson's farm
	bool hasComplexAnderssonsFarm = 
		ResMan.hasDirectory("textures/objects/structures/complexes/anderssonfarm/barnandersson") &&
		ResMan.hasDirectory("textures/objects/structures/complexes/anderssonfarm/mainbuilding") &&
		ResMan.hasDirectory("textures/objects/structures/complexes/anderssonfarm/rockstage") &&
		ResMan.hasDirectory("textures/objects/structures/complexes/anderssonfarm/threshingandersson") &&
		ResMan.hasDirectory("textures/objects/structures/complexes/anderssonfarm/windmillandersson");
	// check for Zane's book covers
	bool hasZaneBookCovers = ResMan.hasResource("textures/objects/props/special/zane_books_covers.tex");
	// check for "missing" posters textures
	bool hasMissingPosters = 
		ResMan.hasResource("textures/objects/props/special/missingposter_contestwinner_s.tex") &&
		ResMan.hasResource("textures/objects/props/special/missing_campers_s.tex");
	// check for Alan Wake's sunglasses texture
	bool hasAlanWakeSunglasses = ResMan.hasResource("textures/objects/items/misc/alanwakesunglasses_s.tex");
	// check for intentional easter eggs
	bool hasEasterEggs = 
		ResMan.hasResource("objects/props/special/easter_eggs_ahma_beer.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_coffee_world.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_fbc_letter.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_jim_figamore.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_kalevala_knights.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_tower_maintenance.binmsh") &&
		ResMan.hasResource("objects/props/special/easter_eggs_watery_attractions.binmsh");
	// check for GDC '09 cinematic animations ?
	bool hasGDC09Animations = 
		ResMan.hasResource("animations/male/alanwake/cinematic_gdc09_1100_wake.binhkx") &&
		ResMan.hasResource("animations/male/barrywheeler/cinematic_gdc09_1100_barry.binhkx") &&
		ResMan.hasResource("animations/male/lee/cinematic_gdc09_1300b_lee.binhkx") &&
		ResMan.hasResource("animations/male/rusty/cinematic_gdc09_1300b_rusty.binhkx") &&
		ResMan.hasResource("animations/female/alice/cinematic_gdc09_1050_alice.binhkx");

	if (hasGDC09Animations && hasAlanWakeSunglasses &&
		hasComplexAnderssonsFarm && hasEasterEggs &&
		hasMissingPosters && hasParadefloat &&
		hasRosesTrailer && hasZaneBookCovers) {
		spdlog::debug("Looks like Alan Wake Remastered!");
		return kResultAlanWakeRemastered;
	} else {
		spdlog::debug("Doesn't look like Alan Wake Remastered...");
		return kResultUnknown;
	}
}

ProbeResult Probe::checkAmericanNightmare() {
	// check for game's characters
	bool hasNightmareCharacters = 
		ResMan.hasResource("objects/characters/alanwake_awns_mesh.binmsh") &&
		ResMan.hasResource("objects/characters/female_npc_emma_emmasloan.binmsh") &&
		ResMan.hasResource("objects/characters/female_npc_rachel_rachelmeadows.binmsh") &&
		ResMan.hasResource("objects/characters/female_npc_serena_serenavaldivia.binmsh") &&
		ResMan.hasResource("objects/characters/mr_scratch_mr_scratch.binmsh");
	// check for film festival banner
	bool hasFilmFestivalBanner = ResMan.hasResource("objects/props/special/film_festival_banner_banner_med.binmsh");
	// check for first level's sattelite
	bool hasSatellite = 
		ResMan.hasResource("objects/props/special/level1_misc_satelite.binhvk") &&
		ResMan.hasResource("objects/props/special/level1_misc_satelite.binmsh");
	// check for observatory posters
	bool hasObservatoryPosters = 
		ResMan.hasResource("objects/props/special/observatory_posters_poster_01.binmsh") &&
		ResMan.hasResource("objects/props/special/observatory_posters_poster_02.binmsh");
	// check for leftover prototype buttons
	bool hasPrototypeButtons = 
		ResMan.hasResource("objects/prototype/test_numbers_green_button.binmsh") &&
		ResMan.hasResource("objects/prototype/test_numbers_red_button.binmsh");
	// check for game's main level structures
	bool hasNightmareComplexes = 
		ResMan.hasDirectory("objects/structures/complexes/driveintheater") &&
		ResMan.hasDirectory("objects/structures/complexes/observatory") &&
		ResMan.hasDirectory("objects/structures/complexes/reststop");
	// check for Nightmare specific music
	bool hasNightmareMusic = 
		ResMan.hasResource("sounds/music/licensed/club_foot_kasabian.fsb") &&
		ResMan.hasResource("sounds/music/potf/potf_balance_slays_the_demon_game_edit.fsb") &&
		ResMan.hasResource("sounds/music/potf/potf_psycho_game_edit.fsb") &&
		ResMan.hasResource("sounds/music/potf/radio_show_03_music.fsb");
	// check for arcade mode game worlds
	bool hasArcadeWorlds = 
		ResMan.hasDirectory("worlds/gameworld/levels/arcade6_rim") &&
		ResMan.hasDirectory("worlds/gameworld/levels/arcade5_rim") &&
		ResMan.hasDirectory("worlds/gameworld/levels/arcade4_rim") &&
		ResMan.hasDirectory("worlds/gameworld/levels/arcade3_rim") &&
		ResMan.hasDirectory("worlds/gameworld/levels/arcade2_rim") &&
		ResMan.hasDirectory("worlds/gameworld/levels/arcade1_rim");
	
	if (hasArcadeWorlds && hasFilmFestivalBanner &&
		hasNightmareCharacters && hasNightmareComplexes &&
		hasNightmareMusic && hasObservatoryPosters &&
		hasPrototypeButtons && hasSatellite) {
		spdlog::debug("Looks like Alan Wake's American Nightmare!");
		return kResultAmericanNightmare;
	} else {
		spdlog::debug("Doesn't look like Alan Wake's American Nightmare...");
	 	return kResultUnknown;
	}
}

ProbeResult Probe::checkQuantumBreak() {
	// check for Microsoft product placement
	bool hasMicrosoftProducts = 
		ResMan.hasResource("animations/intermediate/objects/ringtail/batch39/microsoft_surface_surface_3_p_9ddbe72b.aaa") &&
		ResMan.hasResource("animations/intermediate/objects/ringtail/batch39/microsoft_surface_surface_3_pro_mesh_000.nmb") &&
		ResMan.hasResource("objects/ringtail/batch39/microsoft_surface_surface_3_pro_tablet_without_keyboard.binfbx") &&
		ResMan.hasResource("objects/ringtail/batch45/microsoft_lumia_640.wedmsh");
	// check for game levels
	bool hasQBreakLevels = 
		ResMan.hasDirectory("worlds/gameworld/levels/sofias_island") &&
		ResMan.hasDirectory("worlds/gameworld/levels/university") &&
		ResMan.hasDirectory("worlds/gameworld/levels/shipyard") &&
		ResMan.hasDirectory("worlds/gameworld/levels/secretlab") &&
		ResMan.hasDirectory("worlds/gameworld/levels/old_riverport") &&
		ResMan.hasDirectory("worlds/gameworld/levels/default");
	// check for leftover whiteboxing prototype files
	bool hasQBreakPrototype = ResMan.hasDirectory("textures/whiteboxing_temp/monarch_hq/lods");
	// check for time machine textures
	bool hasTimeMachine = ResMan.hasDirectory("textures/time_machine");
	// check for Nissan Leaf product placement
	bool hasNissanLeaf = 
		ResMan.hasResource("objects/ringtail/batch38/nissan_leaf_logos.wedmsh") &&
		ResMan.hasResource("objects/ringtail/batch38/nissan_leaf_monarch.binhvk") &&
		ResMan.hasResource("objects/ringtail/batch38/nissan_leaf_taxi.binhvk");
	// check for Monarch HQ building
	bool hasMonarchHQ = ResMan.hasDirectory("objects/structures/monarch_hq");
	// check for games characters
	bool hasQBreakCharacters = 
		ResMan.hasDirectory("textures/characters/wiljoy") &&
		ResMan.hasDirectory("textures/characters/jacjoy") &&
		ResMan.hasDirectory("textures/characters/liabur") &&
		ResMan.hasDirectory("textures/characters/nicmar");
	
	if (hasQBreakCharacters && hasQBreakPrototype &&
		hasQBreakLevels && hasMicrosoftProducts &&
		hasMonarchHQ && hasNissanLeaf && hasTimeMachine) {
		spdlog::debug("Looks like Quantum Break!");
		return kResultQuantumBreak;
	} else {
		spdlog::debug("Doesn't look like Quantum Break...");
		return kResultUnknown;
	}
}

ProbeResult Probe::checkControl() {
	// Check for Control missions
	bool hasControlMissions =
	    ResMan.hasDirectory("data/worlds/gameworld/layers/hub_mission_01_the_bureau") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/hub_mission_02_the_hotline") &&
	    ResMan.hasDirectory("data/worlds/gameworld/layers/maintenance_main_mission_03_lockdown") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/research_mission_04_marshall") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/maintenance_main_mission_05_blackrock") &&
	    ResMan.hasDirectory("data/worlds/gameworld/layers/containment_main_mission_06_dylan") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/containment_main_mission_07_ordinary") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/maintenance_main_mission_08_ahti") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/research_mission_09_the_hedron") &&
		ResMan.hasDirectory("data/worlds/gameworld/layers/hub_mission_10_nightmare");
	// Check for mopping Ahti
	bool hasAhti =
		ResMan.hasResource("data/animations/intermediate/p7/human/male/ahti/ahti_mopping_loop_02.binanimclip");
	// Check for vending machine
	bool hasVendingMachine = 
	    ResMan.hasResource("data/objects/props/vending_machines/vending_machines_vending_machine.binfbx");

	if (hasControlMissions && hasAhti && hasVendingMachine) {
		spdlog::debug("Looks like Control!");
		return kResultControl;
	} else {
		spdlog::debug("Doesn't look like Control...");
		return kResultUnknown;
	}
}