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

#include <spdlog/spdlog.h>

#include "src/awe/rmdparchive.h"
#include "probe.h"

Probe::Probe() {
	_resources = &ResMan;
}

unsigned char Probe::performProbe() {
	spdlog::info("Probing archives...");
	uint8_t allChecks = checkAlanWake() |
						checkNightmare() |
						checkQuantumBreak() |
						checkAlanWakeRemastered();
	if ((allChecks & (allChecks - 1)) != 0) // More that one successful check...
		return kResultUnknown;
	else
	 	return allChecks;
}

ProbeResult Probe::checkAlanWake() {
	// check if old Alan Wake's textures are present in files
	bool hasAlanWake2008 = _resources->hasDirectory("textures/characters/alanwake_2008");
	// check for Stucky's texture variation
	bool hasStuckyVar02 = _resources->hasDirectory("textures/characters/stucky_var02");
	// check for sponsored ads
	bool hasVerizonCellPhone = _resources->hasResource("textures/objects/items/misc/cellphone_verizon_n.tex") &&
							   _resources->hasResource("textures/objects/items/misc/cellphone_verizon.tex");
	bool hasEnergizerBatteries = _resources->hasResource("textures/objects/items/lightsources/energizer_flashlight_test.tex") &&
								 _resources->hasResource("textures/objects/items/lightsources/energizer_lithium.tex") &&
								 _resources->hasResource("textures/objects/items/lightsources/energizer_max.tex");
	// check for "gameplay_example" meshes
	bool hasGameplayExample = _resources->hasResource("objects/backdrop/gameplay_example_0_0.binmsh") &&
							  _resources->hasResource("objects/backdrop/gameplay_example_3_3.binmsh");
	// check for test breakable table mesh
	bool hasBreakableTableTest = _resources->hasResource("objects/test/breakabletable_mesh.binmsh");
	// check for Arial font
	bool hasArial = _resources->hasResource("fonts/Arial.binfnt");

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
	bool hasParadefloat = _resources->hasDirectory("textures/objects/vehicles/new/paradefloat/paradefloat");
	// check for Rose's trailer items
	bool hasRosesTrailer = _resources->hasResource("textures/objects/structures/complexes/trailerpark/rosestrailerpropsd3t.tex") &&
						   _resources->hasResource("textures/objects/structures/complexes/trailerpark/rosepillow_d3t.tex") &&
						   _resources->hasResource("textures/objects/structures/complexes/trailerpark/rosebedsheet_d3t.tex");
	// check for complex Andersson's farm
	bool hasComplexAnderssonsFarm = _resources->hasDirectory("textures/objects/structures/complexes/anderssonfarm/barnandersson") &&
									_resources->hasDirectory("textures/objects/structures/complexes/anderssonfarm/mainbuilding") &&
									_resources->hasDirectory("textures/objects/structures/complexes/anderssonfarm/rockstage") &&
									_resources->hasDirectory("textures/objects/structures/complexes/anderssonfarm/threshingandersson") &&
									_resources->hasDirectory("textures/objects/structures/complexes/anderssonfarm/windmillandersson");
	// check for Zane's book covers
	bool hasZaneBookCovers = _resources->hasResource("textures/objects/props/special/zane_books_covers.tex");
	// check for "missing" posters textures
	bool hasMissingPosters = _resources->hasResource("textures/objects/props/special/missingposter_contestwinner_s.tex") &&
							 _resources->hasResource("textures/objects/props/special/missing_campers_s.tex");
	// check for Alan Wake's sunglasses texture
	bool hasAlanWakeSunglasses = _resources->hasResource("textures/objects/items/misc/alanwakesunglasses_s.tex");
	// check for intentional easter eggs
	bool hasEasterEggs = _resources->hasResource("objects/props/special/easter_eggs_ahma_beer.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_coffee_world.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_fbc_letter.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_jim_figamore.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_kalevala_knights.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_tower_maintenance.binmsh") &&
						 _resources->hasResource("objects/props/special/easter_eggs_watery_attractions.binmsh");
	// check for GDC '09 cinematic animations ?
	bool hasGDC09Animations = _resources->hasResource("animations/male/alanwake/cinematic_gdc09_1100_wake.binhkx") &&
							  _resources->hasResource("animations/male/barrywheeler/cinematic_gdc09_1100_barry.binhkx") &&
							  _resources->hasResource("animations/male/lee/cinematic_gdc09_1300b_lee.binhkx") &&
							  _resources->hasResource("animations/male/rusty/cinematic_gdc09_1300b_rusty.binhkx") &&
							  _resources->hasResource("animations/female/alice/cinematic_gdc09_1050_alice.binhkx");

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

ProbeResult Probe::checkNightmare() {
	// check for game's characters
	bool hasNightmareCharacters = _resources->hasResource("objects/characters/alanwake_awns_mesh.binmsh") &&
								  _resources->hasResource("objects/characters/female_npc_emma_emmasloan.binmsh") &&
								  _resources->hasResource("objects/characters/female_npc_rachel_rachelmeadows.binmsh") &&
								  _resources->hasResource("objects/characters/female_npc_serena_serenavaldivia.binmsh") &&
								  _resources->hasResource("objects/characters/mr_scratch_mr_scratch.binmsh");
	// check for film festival banner
	bool hasFilmFestivalBanner = _resources->hasResource("objects/props/special/film_festival_banner_banner_med.binmsh");
	// check for first level's sattelite
	bool hasSatellite = _resources->hasResource("objects/props/special/level1_misc_satelite.binhvk") &&
						_resources->hasResource("objects/props/special/level1_misc_satelite.binmsh");
	// check for observatory posters
	bool hasObservatoryPosters = _resources->hasResource("objects/props/special/observatory_posters_poster_01.binmsh") &&
								 _resources->hasResource("objects/props/special/observatory_posters_poster_02.binmsh");
	// check for leftover prototype buttons
	bool hasPrototypeButtons = _resources->hasResource("objects/prototype/test_numbers_green_button.binmsh") &&
							   _resources->hasResource("objects/prototype/test_numbers_red_button.binmsh");
	// check for game's main level structures
	bool hasNightmareComplexes = _resources->hasDirectory("objects/structures/complexes/driveintheater") &&
								 _resources->hasDirectory("objects/structures/complexes/observatory") &&
								 _resources->hasDirectory("objects/structures/complexes/reststop");
	// check for Nightmare specific music
	bool hasNightmareMusic = _resources->hasResource("sounds/music/licensed/club_foot_kasabian.fsb") &&
							 _resources->hasResource("sounds/music/potf/potf_balance_slays_the_demon_game_edit.fsb") &&
							 _resources->hasResource("sounds/music/potf/potf_psycho_game_edit.fsb") &&
							 _resources->hasResource("sounds/music/potf/radio_show_03_music.fsb");
	// check for arcade mode game worlds
	bool hasArcadeWorlds = _resources->hasDirectory("worlds/gameworld/levels/arcade6_rim") &&
						   _resources->hasDirectory("worlds/gameworld/levels/arcade5_rim") &&
						   _resources->hasDirectory("worlds/gameworld/levels/arcade4_rim") &&
						   _resources->hasDirectory("worlds/gameworld/levels/arcade3_rim") &&
						   _resources->hasDirectory("worlds/gameworld/levels/arcade2_rim") &&
						   _resources->hasDirectory("worlds/gameworld/levels/arcade1_rim");
	
	if (hasArcadeWorlds && hasFilmFestivalBanner &&
		hasNightmareCharacters && hasNightmareComplexes &&
		hasNightmareMusic && hasObservatoryPosters &&
		hasPrototypeButtons && hasSatellite) {
		spdlog::debug("Looks like Alan Wake's American Nightmare!");
		return kResultNightmare;
	} else {
		spdlog::debug("Doesn't look like Alan Wake's American Nightmare...");
	 	return kResultUnknown;
	}
}

ProbeResult Probe::checkQuantumBreak() {
	// check for Microsoft product placement
	bool hasMicrosoftProducts = _resources->hasResource("animations/intermediate/objects/ringtail/batch39/microsoft_surface_surface_3_p_9ddbe72b.aaa") &&
								_resources->hasResource("animations/intermediate/objects/ringtail/batch39/microsoft_surface_surface_3_pro_mesh_000.nmb") &&
								_resources->hasResource("objects/ringtail/batch39/microsoft_surface_surface_3_pro_tablet_without_keyboard.binfbx") &&
								_resources->hasResource("objects/ringtail/batch45/microsoft_lumia_640.wedmsh");
	// check for game levels
	bool hasQBreakLevels = _resources->hasDirectory("worlds/gameworld/levels/sofias_island") &&
						   _resources->hasDirectory("worlds/gameworld/levels/university") &&
						   _resources->hasDirectory("worlds/gameworld/levels/shipyard") &&
						   _resources->hasDirectory("worlds/gameworld/levels/secretlab") &&
						   _resources->hasDirectory("worlds/gameworld/levels/old_riverport") &&
						   _resources->hasDirectory("worlds/gameworld/levels/default");
	// check for leftover whiteboxing prototype files
	bool hasQBreakPrototype = _resources->hasDirectory("textures/whiteboxing_temp/monarch_hq/lods");
	// check for time machine textures
	bool hasTimeMachine = _resources->hasDirectory("textures/time_machine");
	// check for Nissan Leaf product placement
	bool hasNissanLeaf = _resources->hasResource("objects/ringtail/batch38/nissan_leaf_logos.wedmsh") &&
						 _resources->hasResource("objects/ringtail/batch38/nissan_leaf_monarch.binhvk") &&
						 _resources->hasResource("objects/ringtail/batch38/nissan_leaf_taxi.binhvk");
	// check for Monarch HQ building
	bool hasMonarchHQ = _resources->hasDirectory("objects/structures/monarch_hq");
	// check for games characters
	bool hasQBreakCharacters = _resources->hasDirectory("textures/characters/wiljoy") &&
							   _resources->hasDirectory("textures/characters/jacjoy") &&
							   _resources->hasDirectory("textures/characters/liabur") &&
							   _resources->hasDirectory("textures/characters/nicmar");
	
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