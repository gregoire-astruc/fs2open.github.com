
#include "globalincs/pstypes.h"
#include "pilotfile/pilotfile.h"
#include "playerman/player.h"
#include "ship/ship.h"
#include "weapon/weapon.h"
#include "stats/medals.h"
#include "menuui/techmenu.h"
#include "hud/hudconfig.h"
#include "cutscene/cutscenes.h"
#include "missionui/missionscreencommon.h"
#include "mission/missioncampaign.h"
#include "missionui/missionshipchoice.h"
#include "mission/missionload.h"
#include "sound/audiostr.h"
#include "io/joy.h"
#include "io/mouse.h"
#include "gamesnd/eventmusic.h"
#include "freespace.h"

#define REDALERT_INTERNAL
#include "missionui/redalert.h"

#include <iostream>
#include <sstream>



void pilotfile::csg_read_flags()
{
	// tips?
	p->tips = (int)cfile::read<ubyte>(cfp);
}

void pilotfile::csg_write_flags()
{
	startSection(Section::Flags);

	// tips
	cfile::write<ubyte>((ubyte)p->tips, cfp);

	endSection();
}

void pilotfile::csg_read_info()
{
	char t_string[NAME_LENGTH+1] = { '\0' };
	index_list_t ilist;
	int idx, list_size = 0;
	ubyte allowed = 0;

	if ( !m_have_flags ) {
		throw "Info before Flags!";
	}

	//
	// NOTE: lists may contain missing/invalid entries for current data
	//       this is not necessarily fatal
	//

	// ship list (NOTE: may contain more than MAX_SHIP_CLASSES)
	list_size = cfile::read<int>(cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = ship_info_lookup(t_string);

		ship_list.push_back(ilist);
	}

	// weapon list (NOTE: may contain more than MAX_WEAPON_TYPES)
	list_size = cfile::read<int>(cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = weapon_info_lookup(t_string);

		weapon_list.push_back(ilist);
	}

	// intel list (NOTE: may contain more than MAX_INTEL_ENTRIES)
	list_size = cfile::read<int>(cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = intel_info_lookup(t_string);

		intel_list.push_back(ilist);
	}

	// medals list (NOTE: may contain more than Num_medals)
	list_size = cfile::read<int>(cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = medals_info_lookup(t_string);

		medals_list.push_back(ilist);
	}

	// last ship flown (index into ship_list)
	idx = cfile::read<int>(cfp);

	// check the idx is within bounds
	Assertion ((idx < (int)ship_list.size()), "Campaign file contains an incorrect value for the last flown ship class. No data in ship_list for ship number %d.", idx); 
	if (idx >= (int)ship_list.size())
		idx = -1;
	else if (idx != -1)
		p->last_ship_flown_si_index = ship_list[idx].index;
	else
		p->last_ship_flown_si_index = -1;

	// progression state
	Campaign.prev_mission = cfile::read<int>(cfp);
	Campaign.next_mission = cfile::read<int>(cfp);

	// loop state
	Campaign.loop_reentry = cfile::read<int>(cfp);
	Campaign.loop_enabled = cfile::read<int>(cfp);

	// missions completed
	Campaign.num_missions_completed = cfile::read<int>(cfp);

	// allowed ships
	list_size = (int)ship_list.size();
	for (idx = 0; idx < list_size; idx++) {
		allowed = cfile::read<ubyte>(cfp);

		if (allowed) {
			if (ship_list[idx].index >= 0) {
				Campaign.ships_allowed[ship_list[idx].index] = 1;
			} else {
				m_data_invalid = true;
			}
		}
	}

	// allowed weapons
	list_size = (int)weapon_list.size();
	for (idx = 0; idx < list_size; idx++) {
		allowed = cfile::read<ubyte>(cfp);

		if (allowed) {
			if (weapon_list[idx].index >= 0) {
				Campaign.weapons_allowed[weapon_list[idx].index] = 1;
			} else {
				m_data_invalid = true;
			}
		}
	}

	if (csg_ver >= 2) {
		// single/campaign squad name & image
		cfile::readStringLen(p->s_squad_name, NAME_LENGTH, cfp);
		cfile::readStringLen(p->s_squad_filename, MAX_FILENAME_LEN, cfp);
	}

	// if anything we need/use was missing then it should be considered fatal
	if (m_data_invalid) {
		throw "Invalid data for CSG!";
	}
}

void pilotfile::csg_write_info()
{
	int idx;

	startSection(Section::Info);

	// ship list
	cfile::write<int>(Num_ship_classes, cfp);

	for (idx = 0; idx < Num_ship_classes; idx++) {
		cfile::writeStringLen(Ship_info[idx].name, cfp);
	}

	// weapon list
	cfile::write<int>(Num_weapon_types, cfp);

	for (idx = 0; idx < Num_weapon_types; idx++) {
		cfile::writeStringLen(Weapon_info[idx].name, cfp);
	}

	// intel list
	cfile::write<int>(Intel_info_size, cfp);

	for (idx = 0; idx < Intel_info_size; idx++) {
		cfile::writeStringLen(Intel_info[idx].name, cfp);
	}

	// medals list
	cfile::write<int>(Num_medals, cfp);

	for (idx = 0; idx < Num_medals; idx++) {
		cfile::writeStringLen(Medals[idx].name, cfp);
	}

	// last ship flown
	cfile::write<int>(p->last_ship_flown_si_index, cfp);

	// progression state
	cfile::write<int>(Campaign.prev_mission, cfp);
	cfile::write<int>(Campaign.next_mission, cfp);

	// loop state
	cfile::write<int>(Campaign.loop_enabled, cfp);
	cfile::write<int>(Campaign.loop_reentry, cfp);

	// missions completed
	cfile::write<int>(Campaign.num_missions_completed, cfp);

	// allowed ships
	for (idx = 0; idx < Num_ship_classes; idx++) {
		cfile::write<ubyte>(Campaign.ships_allowed[idx], cfp);
	}

	// allowed weapons
	for (idx = 0; idx < Num_weapon_types; idx++) {
		cfile::write<ubyte>(Campaign.weapons_allowed[idx], cfp);
	}

	// single/campaign squad name & image
	cfile::writeStringLen(p->s_squad_name, cfp);
	cfile::writeStringLen(p->s_squad_filename, cfp);

	endSection();
}

void pilotfile::csg_read_missions()
{
	int i, j, idx, list_size;
	cmission *mission;

	if ( !m_have_info ) {
		throw "Missions before Info!";
	}

	for (i = 0; i < Campaign.num_missions_completed; i++) {
		idx = cfile::read<int>(cfp);
		mission = &Campaign.missions[idx];

		mission->completed = 1;

		// flags
		mission->flags = cfile::read<int>(cfp);

		// goals
		mission->num_goals = cfile::read<int>(cfp);

		if (mission->num_goals > 0) {
			mission->goals = (mgoal *) vm_malloc( mission->num_goals * sizeof(mgoal) );
			Verify( mission->goals != NULL );

			memset( mission->goals, 0, mission->num_goals * sizeof(mgoal) );

			for (j = 0; j < mission->num_goals; j++) {
				cfile::readStringLen(mission->goals[j].name, NAME_LENGTH, cfp);
				mission->goals[j].status = cfile::read<char>(cfp);
			}
		}

		// events
		mission->num_events = cfile::read<int>(cfp);

		if (mission->num_events > 0) {
			mission->events = (mevent *) vm_malloc( mission->num_events * sizeof(mevent) );
			Verify( mission->events != NULL );

			memset( mission->events, 0, mission->num_events * sizeof(mevent) );

			for (j = 0; j < mission->num_events; j++) {
				cfile::readStringLen(mission->events[j].name, NAME_LENGTH, cfp);
				mission->events[j].status = cfile::read<char>(cfp);
			}
		}

		// variables
		mission->num_variables = cfile::read<int>(cfp);

		if (mission->num_variables > 0) {
			mission->variables = (sexp_variable *) vm_malloc( mission->num_variables * sizeof(sexp_variable) );
			Verify( mission->variables != NULL );

			memset( mission->variables, 0, mission->num_variables * sizeof(sexp_variable) );

			for (j = 0; j < mission->num_variables; j++) {
				mission->variables[j].type = cfile::read<int>(cfp);
				cfile::readStringLen(mission->variables[j].text, TOKEN_LENGTH, cfp);
				cfile::readStringLen(mission->variables[j].variable_name, TOKEN_LENGTH, cfp);
			}
		}

		// scoring stats
		mission->stats.score = cfile::read<int>(cfp);
		mission->stats.rank = cfile::read<int>(cfp);
		mission->stats.assists = cfile::read<int>(cfp);
		mission->stats.kill_count = cfile::read<int>(cfp);
		mission->stats.kill_count_ok = cfile::read<int>(cfp);
		mission->stats.bonehead_kills = cfile::read<int>(cfp);

		mission->stats.p_shots_fired = cfile::read<uint>(cfp);
		mission->stats.p_shots_hit = cfile::read<uint>(cfp);
		mission->stats.p_bonehead_hits = cfile::read<uint>(cfp);

		mission->stats.s_shots_fired = cfile::read<uint>(cfp);
		mission->stats.s_shots_hit = cfile::read<uint>(cfp);
		mission->stats.s_bonehead_hits = cfile::read<uint>(cfp);

		// ship kills (scoring)
		list_size = (int)ship_list.size();
		for (j = 0; j < list_size; j++) {
			idx = cfile::read<int>(cfp);

			if (ship_list[j].index >= 0) {
				mission->stats.kills[ship_list[j].index] = idx;
			}
		}

		// medals (scoring)
		list_size = (int)medals_list.size();
		for (j = 0; j < list_size; j++) {
			idx = cfile::read<int>(cfp);

			if (medals_list[j].index >= 0) {
				mission->stats.medal_counts[medals_list[j].index] = idx;
			}
		}
	}
}

void pilotfile::csg_write_missions()
{
	int idx, j;
	cmission *mission;

	startSection(Section::Missions);

	for (idx = 0; idx < MAX_CAMPAIGN_MISSIONS; idx++) {
		if (Campaign.missions[idx].completed) {
			mission = &Campaign.missions[idx];

			cfile::write<int>(idx, cfp);

			// flags
			cfile::write<int>(mission->flags, cfp);

			// goals
			cfile::write<int>(mission->num_goals, cfp);

			for (j = 0; j < mission->num_goals; j++) {
				cfile::writeStringLen(mission->goals[j].name, cfp);
				cfile::write<char>(mission->goals[j].status, cfp);
			}

			// events
			cfile::write<int>(mission->num_events, cfp);

			for (j = 0; j < mission->num_events; j++) {
				cfile::writeStringLen(mission->events[j].name, cfp);
				cfile::write<char>(mission->events[j].status, cfp);
			}

			// variables
			cfile::write<int>(mission->num_variables, cfp);

			for (j = 0; j < mission->num_variables; j++) {
				cfile::write<int>(mission->variables[j].type, cfp);
				cfile::writeStringLen(mission->variables[j].text, cfp);
				cfile::writeStringLen(mission->variables[j].variable_name, cfp);
			}

			// scoring stats
			cfile::write<int>(mission->stats.score, cfp);
			cfile::write<int>(mission->stats.rank, cfp);
			cfile::write<int>(mission->stats.assists, cfp);
			cfile::write<int>(mission->stats.kill_count, cfp);
			cfile::write<int>(mission->stats.kill_count_ok, cfp);
			cfile::write<int>(mission->stats.bonehead_kills, cfp);

			cfile::write<uint>(mission->stats.p_shots_fired, cfp);
			cfile::write<uint>(mission->stats.p_shots_hit, cfp);
			cfile::write<uint>(mission->stats.p_bonehead_hits, cfp);

			cfile::write<uint>(mission->stats.s_shots_fired, cfp);
			cfile::write<uint>(mission->stats.s_shots_hit, cfp);
			cfile::write<uint>(mission->stats.s_bonehead_hits, cfp);

			// ship kills (scoring)
			for (j = 0; j < Num_ship_classes; j++) {
				cfile::write<int>(mission->stats.kills[j], cfp);
			}

			// medals earned (scoring)
			for (j = 0; j < Num_medals; j++) {
				cfile::write<int>(mission->stats.medal_counts[j], cfp);
			}
		}
	}

	endSection();
}

void pilotfile::csg_read_techroom()
{
	int idx, list_size = 0;
	ubyte visible;

	if ( !m_have_info ) {
		throw "Techroom before Info!";
	}

	// visible ships
	list_size = (int)ship_list.size();
	for (idx = 0; idx < list_size; idx++) {
		visible = cfile::read<ubyte>(cfp);

		if (visible) {
			if (ship_list[idx].index >= 0) {
				Ship_info[ship_list[idx].index].flags |= SIF_IN_TECH_DATABASE;
			} else {
				m_data_invalid = true;
			}
		}
	}

	// visible weapons
	list_size = (int)weapon_list.size();
	for (idx = 0; idx < list_size; idx++) {
		visible = cfile::read<ubyte>(cfp);

		if (visible) {
			if (weapon_list[idx].index >= 0) {
				Weapon_info[weapon_list[idx].index].wi_flags |= WIF_IN_TECH_DATABASE;
			} else {
				m_data_invalid = true;
			}
		}
	}

	// visible intel entries
	list_size = (int)intel_list.size();
	for (idx = 0; idx < list_size; idx++) {
		visible = cfile::read<ubyte>(cfp);

		if (visible) {
			if (intel_list[idx].index >= 0) {
				Intel_info[intel_list[idx].index].flags |= IIF_IN_TECH_DATABASE;
			} else {
				m_data_invalid = true;
			}
		}
	}

	// if anything we need/use was missing then it should be considered fatal
	if (m_data_invalid) {
		throw "Invalid data for CSG!";
	}
}

void pilotfile::csg_write_techroom()
{
	int idx;
	ubyte visible;

	startSection(Section::Techroom);

	// visible ships
	for (idx = 0; idx < Num_ship_classes; idx++) {
		// only visible if not in techroom by default
		if ( (Ship_info[idx].flags & SIF_IN_TECH_DATABASE) && !(Ship_info[idx].flags2 & SIF2_DEFAULT_IN_TECH_DATABASE) ) {
			visible = 1;
		} else {
			visible = 0;
		}

		cfile::write<ubyte>(visible, cfp);
	}

	// visible weapons
	for (idx = 0; idx < Num_weapon_types; idx++) {
		// only visible if not in techroom by default
		if ( (Weapon_info[idx].wi_flags & WIF_IN_TECH_DATABASE) && !(Weapon_info[idx].wi_flags2 & WIF2_DEFAULT_IN_TECH_DATABASE) ) {
			visible = 1;
		} else {
			visible = 0;
		}

		cfile::write<ubyte>(visible, cfp);
	}

	// visible intel entries
	for (idx = 0; idx < Intel_info_size; idx++) {
		// only visible if not in techroom by default
		if ( (Intel_info[idx].flags & IIF_IN_TECH_DATABASE) && !(Intel_info[idx].flags & IIF_DEFAULT_IN_TECH_DATABASE) ) {
			visible = 1;
		} else {
			visible = 0;
		}

		cfile::write<ubyte>(visible, cfp);
	}

	endSection();
}

void pilotfile::csg_read_loadout()
{
	int j, count, ship_idx = -1, wep_idx = -1;
	uint idx, list_size = 0;

	if ( !m_have_info ) {
		throw "Loadout before Info!";
	}

	// base info
	cfile::readStringLen(Player_loadout.filename, MAX_FILENAME_LEN, cfp);
	cfile::readStringLen(Player_loadout.last_modified, DATE_TIME_LENGTH, cfp);

	// ship pool
	list_size = ship_list.size();
	for (idx = 0; idx < list_size; idx++) {
		count = cfile::read<int>(cfp);

		if (ship_list[idx].index >= 0) {
			Player_loadout.ship_pool[ship_list[idx].index] = count;
		}
	}

	// weapon pool
	list_size = weapon_list.size();
	for (idx = 0; idx < list_size; idx++) {
		count = cfile::read<int>(cfp);

		if (weapon_list[idx].index >= 0) {
			Player_loadout.weapon_pool[weapon_list[idx].index] = count;
		}
	}

	// player ship loadout
	list_size = (uint)cfile::read<ushort>(cfp);
	for (uint i = 0; i < list_size; i++) {
		wss_unit *slot = NULL;

		if (i < MAX_WSS_SLOTS) {
			slot = &Player_loadout.unit_data[i];
		}

		// ship
		ship_idx = cfile::read<int>(cfp);

		if ( (ship_idx >= (int)ship_list.size()) || (ship_idx < -1) ) { // on the casts, assume that ship & weapon lists will never exceed ~2 billion
			mprintf(("CSG => Parse Warning: Invalid value for ship index (%d), emptying slot.\n", ship_idx));
			ship_idx = -1;
		}

		if (slot) {
			if (ship_idx == -1) { // -1 means no ship in this slot
				slot->ship_class = -1;
			} else {
				slot->ship_class = ship_list[ship_idx].index;
			}
		}

		// primary weapons
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			wep_idx = cfile::read<int>(cfp);

			if ( (wep_idx >= (int)weapon_list.size()) || (wep_idx < -1) ) {
				mprintf(("CSG => Parse Warning: Invalid value for primary weapon index (%d), emptying slot.\n", wep_idx));
				wep_idx = -1;
			}


			if ( slot && (j < MAX_SHIP_PRIMARY_BANKS) ) {
				if (wep_idx == -1) { // -1 means no weapon in this slot
					slot->wep[j] = -1;
				} else {
					slot->wep[j] = weapon_list[wep_idx].index;
				}
			}

			idx = cfile::read<int>(cfp);

			if ( slot && (j < MAX_SHIP_PRIMARY_BANKS) ) {
				slot->wep_count[j] = idx;
			}
		}

		// secondary weapons
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			wep_idx = cfile::read<int>(cfp);

			if ( (wep_idx >= (int)weapon_list.size()) || (wep_idx < -1) ) {
				mprintf(("CSG => Parse Warning: Invalid value for secondary weapon index (%d), emptying slot.\n", wep_idx));
				wep_idx = -1;
			}

			if ( slot && (j < MAX_SHIP_SECONDARY_BANKS) ) {
				if (wep_idx == -1) { // -1 means no weapon in this slot
					slot->wep[j+MAX_SHIP_PRIMARY_BANKS] = -1;
				} else {
					slot->wep[j+MAX_SHIP_PRIMARY_BANKS] = weapon_list[wep_idx].index;
				}
			}

			idx = cfile::read<int>(cfp);

			if ( slot && (j < MAX_SHIP_SECONDARY_BANKS) ) {
				slot->wep_count[j+MAX_SHIP_PRIMARY_BANKS] = idx;
			}
		}
	}	
}

void pilotfile::csg_write_loadout()
{
	int idx, j;

	startSection(Section::Loadout);

	// base info
	cfile::writeStringLen(Player_loadout.filename, cfp);
	cfile::writeStringLen(Player_loadout.last_modified, cfp);

	// ship pool
	for (idx = 0; idx < Num_ship_classes; idx++) {
		cfile::write<int>(Player_loadout.ship_pool[idx], cfp);
	}

	// weapon pool
	for (idx = 0; idx < Num_weapon_types; idx++) {
		cfile::write<int>(Player_loadout.weapon_pool[idx], cfp);
	}

	// play ship loadout
	cfile::write<short>(MAX_WSS_SLOTS, cfp);

	for (idx = 0; idx < MAX_WSS_SLOTS; idx++) {
		wss_unit *slot = &Player_loadout.unit_data[idx];

		// ship
		cfile::write<int>(slot->ship_class, cfp);

		// primary weapons
		cfile::write<int>(MAX_SHIP_PRIMARY_BANKS, cfp);

		for (j = 0; j < MAX_SHIP_PRIMARY_BANKS; j++) {
			cfile::write<int>(slot->wep[j], cfp);
			cfile::write<int>(slot->wep_count[j], cfp);
		}

		// secondary weapons
		cfile::write<int>(MAX_SHIP_SECONDARY_BANKS, cfp);

		for (j = 0; j < MAX_SHIP_SECONDARY_BANKS; j++) {
			cfile::write<int>(slot->wep[j+MAX_SHIP_PRIMARY_BANKS], cfp);
			cfile::write<int>(slot->wep_count[j+MAX_SHIP_PRIMARY_BANKS], cfp);
		}
	}

	endSection();
}

void pilotfile::csg_read_stats()
{
	int idx, list_size = 0;
	int count;

	if ( !m_have_info ) {
		throw "Stats before Info!";
	}

	// scoring stats
	p->stats.score = cfile::read<int>(cfp);
	p->stats.rank = cfile::read<int>(cfp);
	p->stats.assists = cfile::read<int>(cfp);
	p->stats.kill_count = cfile::read<int>(cfp);
	p->stats.kill_count_ok = cfile::read<int>(cfp);
	p->stats.bonehead_kills = cfile::read<int>(cfp);

	p->stats.p_shots_fired = cfile::read<uint>(cfp);
	p->stats.p_shots_hit = cfile::read<uint>(cfp);
	p->stats.p_bonehead_hits = cfile::read<uint>(cfp);

	p->stats.s_shots_fired = cfile::read<uint>(cfp);
	p->stats.s_shots_hit = cfile::read<uint>(cfp);
	p->stats.s_bonehead_hits = cfile::read<uint>(cfp);

	p->stats.flight_time = cfile::read<uint>(cfp);
	p->stats.missions_flown = cfile::read<uint>(cfp);
	p->stats.last_flown = (_fs_time_t)cfile::read<int>(cfp);
	p->stats.last_backup = (_fs_time_t)cfile::read<int>(cfp);

	// ship kills (scoring)
	list_size = (int)ship_list.size();
	for (idx = 0; idx < list_size; idx++) {
		count = cfile::read<int>(cfp);

		if (ship_list[idx].index >= 0) {
			p->stats.kills[ship_list[idx].index] = count;
		}
	}

	// medals earned (scoring)
	list_size = (int)medals_list.size();
	for (idx = 0; idx < list_size; idx++) {
		count = cfile::read<int>(cfp);

		if (medals_list[idx].index >= 0) {
			p->stats.medal_counts[medals_list[idx].index] = count;
		}
	}
}

void pilotfile::csg_write_stats()
{
	int idx;

	startSection(Section::Scoring);

	// scoring stats
	cfile::write<int>(p->stats.score, cfp);
	cfile::write<int>(p->stats.rank, cfp);
	cfile::write<int>(p->stats.assists, cfp);
	cfile::write<int>(p->stats.kill_count, cfp);
	cfile::write<int>(p->stats.kill_count_ok, cfp);
	cfile::write<int>(p->stats.bonehead_kills, cfp);

	cfile::write<uint>(p->stats.p_shots_fired, cfp);
	cfile::write<uint>(p->stats.p_shots_hit, cfp);
	cfile::write<uint>(p->stats.p_bonehead_hits, cfp);

	cfile::write<uint>(p->stats.s_shots_fired, cfp);
	cfile::write<uint>(p->stats.s_shots_hit, cfp);
	cfile::write<uint>(p->stats.s_bonehead_hits, cfp);

	cfile::write<uint>(p->stats.flight_time, cfp);
	cfile::write<uint>(p->stats.missions_flown, cfp);
	cfile::write<int>((int)p->stats.last_flown, cfp);
	cfile::write<int>((int)p->stats.last_backup, cfp);

	// ship kills (scoring)
	for (idx = 0; idx < Num_ship_classes; idx++) {
		cfile::write<int>(p->stats.kills[idx], cfp);
	}

	// medals earned (scoring)
	for (idx = 0; idx < Num_medals; idx++) {
		cfile::write<int>(p->stats.medal_counts[idx], cfp);
	}

	endSection();
}

void pilotfile::csg_read_redalert()
{
	int idx, i, j, list_size = 0;
	int count;
	char t_string[MAX_FILENAME_LEN+NAME_LENGTH+1] = { '\0' };
	float hit;
	wep_t weapons;

	if ( !m_have_info ) {
		throw "RedAlert before Info!";
	}

	list_size = cfile::read<int>(cfp);

	if (list_size <= 0) {
		return;
	}

	// about to read new redalert data so flush any existing data
	// otherwise wingman entries will multiply like rabbits
	Red_alert_wingman_status.clear();

	cfile::readStringLen(t_string, MAX_FILENAME_LEN, cfp);

	Red_alert_precursor_mission = t_string;

	for (idx = 0; idx < list_size; idx++) {
		red_alert_ship_status ras;

		cfile::readStringLen(t_string, NAME_LENGTH, cfp);
		ras.name = t_string;

		ras.hull = cfile::read<float>(cfp);

		// ship class, index into ship_list[]
		i = cfile::read<int>(cfp);
		if ( (i >= (int)ship_list.size()) || (i < RED_ALERT_LOWEST_VALID_SHIP_CLASS) ) {
			mprintf(("CSG => Parse Warning: Invalid value for red alert ship index (%d), emptying slot.\n", i));
			ras.ship_class = RED_ALERT_DESTROYED_SHIP_CLASS;
		} else if ( (i < 0 ) && (i >= RED_ALERT_LOWEST_VALID_SHIP_CLASS) ) {  // ship destroyed/exited
			ras.ship_class = i;
		} else {
		ras.ship_class = ship_list[i].index;
		}

		// subsystem hits
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			hit = cfile::read<float>(cfp);
			ras.subsys_current_hits.push_back( hit );
		}

		// subsystem aggregate hits
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			hit = cfile::read<float>(cfp);
			ras.subsys_aggregate_current_hits.push_back( hit );
		}

		// primary weapon loadout and status
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			i = cfile::read<int>(cfp);
			weapons.index = weapon_list[i].index;
			weapons.count = cfile::read<int>(cfp);

			// triggering this means something is really fubar
			if (weapons.index < 0) {
				continue;
			}

			ras.primary_weapons.push_back( weapons );
		}

		// secondary weapon loadout and status
		count = cfile::read<int>(cfp);

		for (j = 0; j < count; j++) {
			i = cfile::read<int>(cfp);
			weapons.index = weapon_list[i].index;
			weapons.count = cfile::read<int>(cfp);

			// triggering this means something is really fubar
			if (weapons.index < 0) {
				continue;
			}

			ras.secondary_weapons.push_back( weapons );
		}

		// this is quite likely a *bad* thing if it doesn't happen
		if (ras.ship_class >= RED_ALERT_LOWEST_VALID_SHIP_CLASS) {
			Red_alert_wingman_status.push_back( ras );
		}
	}
}

void pilotfile::csg_write_redalert()
{
	int idx, j, list_size = 0;
	int count;
	red_alert_ship_status *ras;

	startSection(Section::RedAlert);

	list_size = (int)Red_alert_wingman_status.size();

	cfile::write<int>(list_size, cfp);

	if (list_size) {
		cfile::writeStringLen(Red_alert_precursor_mission.c_str(), cfp);

		for (idx = 0; idx < list_size; idx++) {
			ras = &Red_alert_wingman_status[idx];

			cfile::writeStringLen(ras->name.c_str(), cfp);

			cfile::write<float>(ras->hull, cfp);

			// ship class, should be index into ship_list[] on load
			cfile::write<int>(ras->ship_class, cfp);

			// subsystem hits
			count = (int)ras->subsys_current_hits.size();
			cfile::write<int>(count, cfp);

			for (j = 0; j < count; j++) {
				cfile::write<float>(ras->subsys_current_hits[j], cfp);
			}

			// subsystem aggregate hits
			count = (int)ras->subsys_aggregate_current_hits.size();
			cfile::write<int>(count, cfp);

			for (j = 0; j < count; j++) {
				cfile::write<float>(ras->subsys_aggregate_current_hits[j], cfp);
			}

			// primary weapon loadout and status
			count = (int)ras->primary_weapons.size();
			cfile::write<int>(count, cfp);

			for (j = 0; j < count; j++) {
				cfile::write<int>(ras->primary_weapons[j].index, cfp);
				cfile::write<int>(ras->primary_weapons[j].count, cfp);
			}

			// secondary weapon loadout and status
			count = (int)ras->secondary_weapons.size();
			cfile::write<int>(count, cfp);

			for (j = 0; j < count; j++) {
				cfile::write<int>(ras->secondary_weapons[j].index, cfp);
				cfile::write<int>(ras->secondary_weapons[j].count, cfp);
			}
		}
	}

	endSection();
}

void pilotfile::csg_read_hud()
{
	int idx;

	// flags
	HUD_config.show_flags = cfile::read<int>(cfp);
	HUD_config.show_flags2 = cfile::read<int>(cfp);

	HUD_config.popup_flags = cfile::read<int>(cfp);
	HUD_config.popup_flags2 = cfile::read<int>(cfp);

	// settings
	HUD_config.num_msg_window_lines = cfile::read<ubyte>(cfp);

	HUD_config.rp_flags = cfile::read<int>(cfp);
	HUD_config.rp_dist = cfile::read<int>(cfp);

	// basic colors
	HUD_config.main_color = cfile::read<int>(cfp);
	HUD_color_alpha = cfile::read<int>(cfp);

	if (HUD_color_alpha < HUD_COLOR_ALPHA_USER_MIN) {
		HUD_color_alpha = HUD_COLOR_ALPHA_DEFAULT;
	}

	hud_config_record_color(HUD_config.main_color);

	// gauge-specific colors
	int num_gauges = cfile::read<int>(cfp);

	for (idx = 0; idx < num_gauges; idx++) {
		ubyte red = cfile::read<ubyte>(cfp);
		ubyte green = cfile::read<ubyte>(cfp);
		ubyte blue = cfile::read<ubyte>(cfp);
		ubyte alpha = cfile::read<ubyte>(cfp);

		if (idx >= NUM_HUD_GAUGES) {
			continue;
		}

		HUD_config.clr[idx].red = red;
		HUD_config.clr[idx].green = green;
		HUD_config.clr[idx].blue = blue;
		HUD_config.clr[idx].alpha = alpha;
	}
}

void pilotfile::csg_write_hud()
{
	int idx;

	startSection(Section::HUD);

	// flags
	cfile::write<int>(HUD_config.show_flags, cfp);
	cfile::write<int>(HUD_config.show_flags2, cfp);

	cfile::write<int>(HUD_config.popup_flags, cfp);
	cfile::write<int>(HUD_config.popup_flags2, cfp);

	// settings
	cfile::write<ubyte>(HUD_config.num_msg_window_lines, cfp);

	cfile::write<int>(HUD_config.rp_flags, cfp);
	cfile::write<int>(HUD_config.rp_dist, cfp);

	// basic colors
	cfile::write<int>(HUD_config.main_color, cfp);
	cfile::write<int>(HUD_color_alpha, cfp);

	// gauge-specific colors
	cfile::write<int>(NUM_HUD_GAUGES, cfp);

	for (idx = 0; idx < NUM_HUD_GAUGES; idx++) {
		cfile::write<ubyte>(HUD_config.clr[idx].red, cfp);
		cfile::write<ubyte>(HUD_config.clr[idx].green, cfp);
		cfile::write<ubyte>(HUD_config.clr[idx].blue, cfp);
		cfile::write<ubyte>(HUD_config.clr[idx].alpha, cfp);
	}

	endSection();
}

void pilotfile::csg_read_variables()
{
	int idx;

	Campaign.num_variables = cfile::read<int>(cfp);

	if (Campaign.num_variables > 0) {
		Campaign.variables = (sexp_variable *) vm_malloc( Campaign.num_variables * sizeof(sexp_variable) );
		Verify( Campaign.variables != NULL );

		memset( Campaign.variables, 0, Campaign.num_variables * sizeof(sexp_variable) );

		for (idx = 0; idx < Campaign.num_variables; idx++) {
			Campaign.variables[idx].type = cfile::read<int>(cfp);
			cfile::readStringLen(Campaign.variables[idx].text, TOKEN_LENGTH, cfp);
			cfile::readStringLen(Campaign.variables[idx].variable_name, TOKEN_LENGTH, cfp);
		}
	}
}

void pilotfile::csg_write_variables()
{
	int idx;

	startSection(Section::Variables);

	cfile::write<int>(Campaign.num_variables, cfp);

	for (idx = 0; idx < Campaign.num_variables; idx++) {
		cfile::write<int>(Campaign.variables[idx].type, cfp);
		cfile::writeStringLen(Campaign.variables[idx].text, cfp);
		cfile::writeStringLen(Campaign.variables[idx].variable_name, cfp);
	}

	endSection();
}

void pilotfile::csg_read_settings()
{
	// sound/voice/music
	Master_sound_volume = cfile::read<float>(cfp);
	Master_event_music_volume = cfile::read<float>(cfp);
	Master_voice_volume = cfile::read<float>(cfp);

	audiostream_set_volume_all(Master_voice_volume, ASF_VOICE);
	audiostream_set_volume_all(Master_event_music_volume, ASF_EVENTMUSIC);

	if (Master_event_music_volume > 0.0f) {
		Event_music_enabled = 1;
	} else {
		Event_music_enabled = 0;
	}

	Briefing_voice_enabled = cfile::read<int>(cfp);

	// skill level
	Game_skill_level = cfile::read<int>(cfp);

	// input options
	Use_mouse_to_fly = cfile::read<int>(cfp);
	Mouse_sensitivity = cfile::read<int>(cfp);
	Joy_sensitivity = cfile::read<int>(cfp);
	Dead_zone_size = cfile::read<int>(cfp);

	if (csg_ver < 3) {
		// detail
		int dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
		dummy = cfile::read<int>(cfp);
	}
}

void pilotfile::csg_write_settings()
{
	startSection(Section::Settings);

	// sound/voice/music
	cfile::write<float>(Master_sound_volume, cfp);
	cfile::write<float>(Master_event_music_volume, cfp);
	cfile::write<float>(Master_voice_volume, cfp);

	cfile::write<int>(Briefing_voice_enabled, cfp);

	// skill level
	cfile::write<int>(Game_skill_level, cfp);

	// input options
	cfile::write<int>(Use_mouse_to_fly, cfp);
	cfile::write<int>(Mouse_sensitivity, cfp);
	cfile::write<int>(Joy_sensitivity, cfp);
	cfile::write<int>(Dead_zone_size, cfp);

	endSection();
}

void pilotfile::csg_read_controls()
{
	int idx, list_size;
	short id1, id2, id3;

	list_size = (int)cfile::read<ushort>(cfp);

	for (idx = 0; idx < list_size; idx++) {
		id1 = cfile::read<short>(cfp);
		id2 = cfile::read<short>(cfp);
		id3 = cfile::read<short>(cfp);	// unused, at the moment

		if (idx < CCFG_MAX) {
			Control_config[idx].key_id = id1;
			Control_config[idx].joy_id = id2;
		}
	}
}

void pilotfile::csg_write_controls()
{
	int idx;

	startSection(Section::Controls);

	cfile::write<short>(CCFG_MAX, cfp);

	for (idx = 0; idx < CCFG_MAX; idx++) {
		cfile::write<short>(Control_config[idx].key_id, cfp);
		cfile::write<short>(Control_config[idx].joy_id, cfp);
		// placeholder? for future mouse_id?
		cfile::write<short>(-1, cfp);
	}

	endSection();
}

void pilotfile::csg_read_cutscenes() {
	size_t list_size = cfile::read<uint>(cfp);

	for(size_t i = 0; i < list_size; i++) {
		char tempFilename[MAX_FILENAME_LEN];

		cfile::readStringLen(tempFilename, MAX_FILENAME_LEN, cfp);
		cutscene_mark_viewable(tempFilename);
	}
}

void pilotfile::csg_write_cutscenes() {
	SCP_vector<cutscene_info>::iterator cut;

	startSection(Section::Cutscenes);

	size_t viewableScenes = 0;
	for(cut = Cutscenes.begin(); cut != Cutscenes.end(); ++cut) {
		if(cut->viewable)
			viewableScenes ++;
	}
	cfile::write<uint>(viewableScenes, cfp);

	for(cut = Cutscenes.begin(); cut != Cutscenes.end(); ++cut) {
		if(cut->viewable)
			cfile::writeStringLen(cut->filename, cfp);
	}

	endSection();
}

/*
 * Only used for quick start missions
 */
void pilotfile::csg_read_lastmissions()
{
	int i;

	// restore list of most recently played missions
	Num_recent_missions = cfile::read<int>( cfp );
	Assert(Num_recent_missions <= MAX_RECENT_MISSIONS);
	for ( i = 0; i < Num_recent_missions; i++ ) {
		char *cp;

		cfile::readStringLen( Recent_missions[i], MAX_FILENAME_LEN, cfp);
		// Remove the extension (safety check: shouldn't exist anyway)
		cp = strchr(Recent_missions[i], '.');
			if (cp)
				*cp = 0;
	}
}

/*
 * Only used for quick start missions
 */

void pilotfile::csg_write_lastmissions()
{
	int i;

	startSection(Section::LastMissions);

	// store list of most recently played missions
	cfile::write<int>(Num_recent_missions, cfp);
	for (i=0; i<Num_recent_missions; i++) {
		cfile::writeStringLen(Recent_missions[i], cfp);
	}

	endSection();
}

void pilotfile::csg_reset_data()
{
	int idx;
	cmission *mission;

	// internals
	m_have_flags = false;
	m_have_info = false;

	m_data_invalid = false;

	// init stats
	p->stats.init();

	// zero out allowed ships/weapons
	memset(Campaign.ships_allowed, 0, sizeof(Campaign.ships_allowed));
	memset(Campaign.weapons_allowed, 0, sizeof(Campaign.weapons_allowed));

	// reset campaign status
	Campaign.prev_mission = -1;
	Campaign.next_mission = -1;
	Campaign.num_missions_completed = 0;

	// techroom reset
	tech_reset_to_default();

	// clear out variables
	if (Campaign.variables) {
		Campaign.num_variables = 0;
		vm_free(Campaign.variables);
		Campaign.variables = NULL;
	}

	// clear out mission stuff
	for (idx = 0; idx < MAX_CAMPAIGN_MISSIONS; idx++) {
		mission = &Campaign.missions[idx];

		if (mission->goals) {
			mission->num_goals = 0;
			vm_free(mission->goals);
			mission->goals = NULL;
		}

		if (mission->events) {
			mission->num_events = 0;
			vm_free(mission->events);
			mission->events = NULL;
		}

		if (mission->variables) {
			mission->num_variables = 0;
			vm_free(mission->variables);
			mission->variables = NULL;
		}

		mission->stats.init();
	}
}

void pilotfile::csg_close()
{
	if (cfp) {
		cfile::close(cfp);
		cfp = NULL;
	}

	p = NULL;
	filename = "";

	ship_list.clear();
	weapon_list.clear();
	intel_list.clear();
	medals_list.clear();

	m_have_flags = false;
	m_have_info = false;
}

bool pilotfile::load_savefile(const char *campaign)
{
	char base[_MAX_FNAME] = { '\0' };
	SCP_stringstream buf;

	if (Game_mode & GM_MULTIPLAYER) {
		return false;
	}

	if ( (campaign == NULL) || !strlen(campaign) ) {
		return false;
	}

	// set player ptr first thing
	Assert( (Player_num >= 0) && (Player_num < MAX_PLAYERS) );
	p = &Players[Player_num];

	// build up filename for the savefile...
	_splitpath((char*)campaign, NULL, NULL, base, NULL);

	buf << p->callsign << "." << base << ".csg";

	filename = buf.str().c_str();

	// if campaign file doesn't exist, abort so we don't load irrelevant data
	buf.str(SCP_string());
	buf << base << FS_CAMPAIGN_FILE_EXT;
	if (!cfile::exists(buf.str(), cfile::TYPE_MISSIONS) ) {
		mprintf(("CSG => Unable to find campaign file '%s'!\n", buf.str().c_str()));
		return false;
	}

	// we need to reset this early, in case open fails and we need to create
	m_data_invalid = false;

	// open it, hopefully...
	cfp = cfile::open(filename, cfile::MODE_READ, cfile::OPEN_NORMAL, cfile::TYPE_PLAYERS);

	if ( !cfp ) {
		mprintf(("CSG => Unable to open '%s' for reading!\n", filename.c_str()));
		return false;
	}

	unsigned int csg_id = cfile::read<uint>(cfp);

	if (csg_id != CSG_FILE_ID) {
		mprintf(("CSG => Invalid header id for '%s'!\n", filename.c_str()));
		csg_close();
		return false;
	}

	// version, now used
	csg_ver = cfile::read<ubyte>(cfp);

	mprintf(("CSG => Loading '%s' with version %d...\n", filename.c_str(), (int)csg_ver));

	csg_reset_data();

	// the point of all this: read in the CSG contents
	while ( !cfile::eof(cfp) ) {
		ushort section_id = cfile::read<ushort>(cfp);
		uint section_size = cfile::read<uint>(cfp);

		size_t start_pos = cfile::tell(cfp);

		// safety, to help protect against long reads
		cfile::setMaxReadLength(cfp, section_size);

		try {
			switch (section_id) {
				case Section::Flags:
					mprintf(("CSG => Parsing:  Flags...\n"));
					m_have_flags = true;
					csg_read_flags();
					break;

				case Section::Info:
					mprintf(("CSG => Parsing:  Info...\n"));
					m_have_info = true;
					csg_read_info();
					break;

				case Section::Variables:
					mprintf(("CSG => Parsing:  Variables...\n"));
					csg_read_variables();
					break;

				case Section::HUD:
					mprintf(("CSG => Parsing:  HUD...\n"));
					csg_read_hud();
					break;

				case Section::RedAlert:
					mprintf(("CSG => Parsing:  RedAlert...\n"));
					csg_read_redalert();
					break;

				case Section::Scoring:
					mprintf(("CSG => Parsing:  Scoring...\n"));
					csg_read_stats();
					break;

				case Section::Loadout:
					mprintf(("CSG => Parsing:  Loadout...\n"));
					csg_read_loadout();
					break;

				case Section::Techroom:
					mprintf(("CSG => Parsing:  Techroom...\n"));
					csg_read_techroom();
					break;

				case Section::Missions:
					mprintf(("CSG => Parsing:  Missions...\n"));
					csg_read_missions();
					break;

				case Section::Settings:
					mprintf(("CSG => Parsing:  Settings...\n"));
					csg_read_settings();
					break;

				case Section::Controls:
					mprintf(("CSG => Parsing:  Controls...\n"));
					csg_read_controls();
					break;

				case Section::Cutscenes:
					mprintf(("CSG => Parsing:  Cutscenes...\n"));
					csg_read_cutscenes();
					break;

				case Section::LastMissions:
					mprintf(("CSG => Parsing:  Last Missions...\n"));
					csg_read_lastmissions();
					break;

				default:
					mprintf(("CSG => Skipping unknown section 0x%04x!\n", section_id));
					break;
			}
		} catch (cfile::MaxReadLengthException &msg) {
			// read to max section size, move to next section, discarding
			// extra/unknown data
			mprintf(("CSG => Warning: (0x%04x) %s\n", section_id, msg.what()));
		} catch (const char *err) {
			mprintf(("CSG => ERROR: %s\n", err));
			csg_close();
			return false;
		}

		// reset safety catch
		cfile::setMaxReadLength(cfp, 0);

		// skip to next section (if not already there)
		size_t offset_pos = (start_pos + section_size) - cfile::tell(cfp);

		if (offset_pos) {
			mprintf(("CSG => Warning: (0x%04x) Short read, information may have been lost!\n", section_id));
			cfile::seek(cfp, offset_pos, cfile::SEEK_MODE_CUR);
		}
	}

	// if the campaign (for whatever reason) doesn't have a squad image, use the multi one
	if (p->s_squad_filename[0] == '\0') {
		strcpy_s(p->s_squad_filename, p->m_squad_filename);
	}
	player_set_squad_bitmap(p, p->s_squad_filename, false);

	mprintf(("CSG => Loading complete!\n"));

	// cleanup and return
	csg_close();

	return true;
}

bool pilotfile::save_savefile()
{
	char base[_MAX_FNAME] = { '\0' };
	std::ostringstream buf;

	if (Game_mode & GM_MULTIPLAYER) {
		return false;
	}

	// set player ptr first thing
	Assert( (Player_num >= 0) && (Player_num < MAX_PLAYERS) );
	p = &Players[Player_num];

	if ( !strlen(Campaign.filename) ) {
		return false;
	}

	// build up filename for the savefile...
	_splitpath(Campaign.filename, NULL, NULL, base, NULL);

	buf << p->callsign << "." << base << ".csg";

	filename = buf.str().c_str();

	// make sure that we can actually save this safely
	if (m_data_invalid) {
		mprintf(("CSG => Skipping save of '%s' due to invalid data check!\n", filename.c_str()));
		return false;
	}

	// validate the number of red alert entries
	// assertion before writing so that we don't corrupt the .csg by asserting halfway through writing
	// assertion should also prevent loss of major campaign progress
	// i.e. lose one mission, not several missions worth (in theory)
	Assertion(Red_alert_wingman_status.size() <= MAX_SHIPS, "Invalid number of Red_alert_wingman_status entries: %u\n", Red_alert_wingman_status.size());

	// open it, hopefully...
	cfp = cfile::open((char*)filename.c_str(), cfile::MODE_WRITE, cfile::OPEN_NORMAL, cfile::TYPE_PLAYERS);

	if ( !cfp ) {
		mprintf(("CSG => Unable to open '%s' for saving!\n", filename.c_str()));
		return false;
	}

	// header and version
	cfile::write<int>(CSG_FILE_ID, cfp);
	cfile::write<ubyte>(CSG_VERSION, cfp);

	mprintf(("CSG => Saving '%s' with version %d...\n", filename.c_str(), (int)CSG_VERSION));

	// flags and info sections go first
	mprintf(("CSG => Saving:  Flags...\n"));
	csg_write_flags();
	mprintf(("CSG => Saving:  Info...\n"));
	csg_write_info();

	// everything else is next, not order specific
	mprintf(("CSG => Saving:  Missions...\n"));
	csg_write_missions();
	mprintf(("CSG => Saving:  Techroom...\n"));
	csg_write_techroom();
	mprintf(("CSG => Saving:  Loadout...\n"));
	csg_write_loadout();
	mprintf(("CSG => Saving:  Scoring...\n"));
	csg_write_stats();
	mprintf(("CSG => Saving:  RedAlert...\n"));
	csg_write_redalert();
	mprintf(("CSG => Saving:  HUD...\n"));
	csg_write_hud();
	mprintf(("CSG => Saving:  Variables...\n"));
	csg_write_variables();
	mprintf(("CSG => Saving:  Settings...\n"));
	csg_write_settings();
	mprintf(("CSG => Saving:  Controls...\n"));
	csg_write_controls();
	mprintf(("CSG => Saving:  Cutscenes...\n"));
	csg_write_cutscenes();
	mprintf(("CSG => Saving:  Last Missions...\n"));
	csg_write_lastmissions();

	// Done!
	mprintf(("CSG => Saving complete!\n"));

	csg_close();

	return true;
}

