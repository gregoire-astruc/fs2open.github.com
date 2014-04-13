
#include "globalincs/pstypes.h"
#include "pilotfile/pilotfile.h"
#include "playerman/player.h"
#include "ship/ship.h"
#include "menuui/techmenu.h"
#include "weapon/weapon.h"
#include "hud/hudconfig.h"
#include "stats/medals.h"
#include "hud/hudsquadmsg.h"
#include "gamesnd/eventmusic.h"
#include "osapi/osregistry.h"
#include "sound/audiostr.h"
#include "io/joy.h"
#include "io/mouse.h"
#include "network/multi.h"
#include "freespace.h"
#include "playerman/managepilot.h"


void pilotfile::plr_read_flags()
{
	// tips?
	p->tips = (int)cfile::read<ubyte>(cfp);

	// saved flags
	p->save_flags = cfile::read<int>(cfp);

	// listing mode (single or campaign missions
	p->readyroom_listing_mode = cfile::read<int>(cfp);

	// briefing auto-play
	p->auto_advance = cfile::read<int>(cfp);

	// special rank setting (to avoid having to read all stats on verify)
	p->stats.rank = cfile::read<int>(cfp);

	if (version > 0) 
	{
		p->player_was_multi = cfile::read<int>(cfp);
	} else 
	{
		p->player_was_multi = 0; // Default to single player
	}
}

void pilotfile::plr_write_flags()
{
	startSection(Section::Flags);

	// tips
	cfile::write<ubyte>((unsigned char)p->tips, cfp);

	// saved flags
	cfile::write<int>(p->save_flags, cfp);

	// listing mode (single or campaign missions)
	cfile::write<int>(p->readyroom_listing_mode, cfp);

	// briefing auto-play
	cfile::write<int>(p->auto_advance, cfp);

	// special rank setting (to avoid having to read all stats on verify)
	cfile::write<int>(p->stats.rank, cfp);

	// What game mode we were in last on this pilot
	cfile::write<int>(p->player_was_multi, cfp);

	endSection();
}

void pilotfile::plr_read_info()
{
	if ( !m_have_flags ) {
		throw "Info before Flags!";
	}

	// pilot image
	cfile::io::readStringLen(p->image_filename, MAX_FILENAME_LEN, cfp);

	// multi squad name
	cfile::io::readStringLen(p->m_squad_name, NAME_LENGTH, cfp);

	// squad image
	cfile::io::readStringLen(p->m_squad_filename, MAX_FILENAME_LEN, cfp);

	// active campaign
	cfile::io::readStringLen(p->current_campaign, MAX_FILENAME_LEN, cfp);
}

void pilotfile::plr_write_info()
{
	startSection(Section::Info);

	// pilot image
	cfile::io::writeStringLen(p->image_filename, cfp);

	// multi squad name
	cfile::io::writeStringLen(p->m_squad_name, cfp);

	// squad image
	cfile::io::writeStringLen(p->m_squad_filename, cfp);

	// active campaign
	cfile::io::writeStringLen(p->current_campaign, cfp);

	endSection();
}

void pilotfile::plr_read_hud()
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

	hud_config_set_color(HUD_config.main_color);

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

void pilotfile::plr_write_hud()
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

void pilotfile::plr_read_variables()
{
	int list_size = 0;
	int idx;
	sexp_variable n_var;

	list_size = cfile::read<int>(cfp);

	if (list_size <= 0) {
		return;
	}

	p->variables.reserve(list_size);

	for (idx = 0; idx < list_size; idx++) {
		n_var.type = cfile::read<int>(cfp);
		cfile::io::readStringLen(n_var.text, TOKEN_LENGTH, cfp);
		cfile::io::readStringLen(n_var.variable_name, TOKEN_LENGTH, cfp);

		p->variables.push_back( n_var );
	}
}

void pilotfile::plr_write_variables()
{
	int list_size = 0;
	int idx;

	startSection(Section::Variables);

	list_size = (int)p->variables.size();

	cfile::write<int>(list_size, cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::write<int>(p->variables[idx].type, cfp);
		cfile::io::writeStringLen(p->variables[idx].text, cfp);
		cfile::io::writeStringLen(p->variables[idx].variable_name, cfp);
	}

	endSection();
}

void pilotfile::plr_read_multiplayer()
{
	// netgame options
	p->m_server_options.squad_set = cfile::read<ubyte>(cfp);
	p->m_server_options.endgame_set = cfile::read<ubyte>(cfp);
	p->m_server_options.flags = cfile::read<int>(cfp);
	p->m_server_options.respawn = cfile::read<uint>(cfp);
	p->m_server_options.max_observers = cfile::read<ubyte>(cfp);
	p->m_server_options.skill_level = cfile::read<ubyte>(cfp);
	p->m_server_options.voice_qos = cfile::read<ubyte>(cfp);
	p->m_server_options.voice_token_wait = cfile::read<int>(cfp);
	p->m_server_options.voice_record_time = cfile::read<int>(cfp);
	p->m_server_options.mission_time_limit = (fix)cfile::read<int>(cfp);
	p->m_server_options.kill_limit = cfile::read<int>(cfp);

	// local options
	p->m_local_options.flags = cfile::read<int>(cfp);
	p->m_local_options.obj_update_level = cfile::read<int>(cfp);

	// netgame protocol
	Multi_options_g.protocol = cfile::read<int>(cfp);

	if (Multi_options_g.protocol == NET_VMT) {
		Multi_options_g.protocol = NET_TCP;
	}

	Assert( (Multi_options_g.protocol == NET_IPX) || (Multi_options_g.protocol == NET_TCP) );
}

void pilotfile::plr_write_multiplayer()
{
	startSection(Section::Multiplayer);

	// netgame options
	cfile::write<ubyte>(p->m_server_options.squad_set, cfp);
	cfile::write<ubyte>(p->m_server_options.endgame_set, cfp);
	cfile::write<int>(p->m_server_options.flags, cfp);
	cfile::write<uint>(p->m_server_options.respawn, cfp);
	cfile::write<ubyte>(p->m_server_options.max_observers, cfp);
	cfile::write<ubyte>(p->m_server_options.skill_level, cfp);
	cfile::write<ubyte>(p->m_server_options.voice_qos, cfp);
	cfile::write<int>(p->m_server_options.voice_token_wait, cfp);
	cfile::write<int>(p->m_server_options.voice_record_time, cfp);
	cfile::write<int>((int)p->m_server_options.mission_time_limit, cfp);
	cfile::write<int>(p->m_server_options.kill_limit, cfp);

	// local options
	cfile::write<int>(p->m_local_options.flags, cfp);
	cfile::write<int>(p->m_local_options.obj_update_level, cfp);

	// netgame protocol
	cfile::write<int>(Multi_options_g.protocol, cfp);

	endSection();
}

void pilotfile::plr_read_stats()
{
	int idx, j, list_size = 0;
	index_list_t ilist;
	char t_string[NAME_LENGTH+1];

	// global, all-time stats (used only until campaign stats are loaded)
	all_time_stats.score = cfile::read<int>(cfp);
	all_time_stats.rank = cfile::read<int>(cfp);
	all_time_stats.assists = cfile::read<int>(cfp);
	all_time_stats.kill_count = cfile::read<int>(cfp);
	all_time_stats.kill_count_ok = cfile::read<int>(cfp);
	all_time_stats.bonehead_kills = cfile::read<int>(cfp);

	all_time_stats.p_shots_fired = cfile::read<uint>(cfp);
	all_time_stats.p_shots_hit = cfile::read<uint>(cfp);
	all_time_stats.p_bonehead_hits = cfile::read<uint>(cfp);

	all_time_stats.s_shots_fired = cfile::read<uint>(cfp);
	all_time_stats.s_shots_hit = cfile::read<uint>(cfp);
	all_time_stats.s_bonehead_hits = cfile::read<uint>(cfp);

	all_time_stats.flight_time = cfile::read<uint>(cfp);
	all_time_stats.missions_flown = cfile::read<uint>(cfp);
	all_time_stats.last_flown = (_fs_time_t)cfile::read<int>(cfp);
	all_time_stats.last_backup = (_fs_time_t)cfile::read<int>(cfp);

	// ship kills (contains ships across all mods, not just current)
	list_size = cfile::read<int>(cfp);
	all_time_stats.ship_kills.reserve(list_size);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = ship_info_lookup(t_string);
		ilist.val = cfile::read<int>(cfp);

		all_time_stats.ship_kills.push_back(ilist);
	}

	// medals earned (contains medals across all mods, not just current)
	list_size = cfile::read<int>(cfp);
	all_time_stats.medals_earned.reserve(list_size);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::readStringLen(t_string, NAME_LENGTH,cfp);

		ilist.name = t_string;
		ilist.index = medals_info_lookup(t_string);
		ilist.val = cfile::read<int>(cfp);

		all_time_stats.medals_earned.push_back(ilist);
	}

	// if not in multiplayer mode then set these stats as the player stats
	if ( !(Game_mode & GM_MULTIPLAYER) ) {
		p->stats.score = all_time_stats.score;
		p->stats.rank = all_time_stats.rank;
		p->stats.assists = all_time_stats.assists;
		p->stats.kill_count = all_time_stats.kill_count;
		p->stats.kill_count_ok = all_time_stats.kill_count_ok;
		p->stats.bonehead_kills = all_time_stats.bonehead_kills;

		p->stats.p_shots_fired = all_time_stats.p_shots_fired;
		p->stats.p_shots_hit = all_time_stats.p_shots_hit;
		p->stats.p_bonehead_hits = all_time_stats.p_bonehead_hits;

		p->stats.s_shots_fired = all_time_stats.s_shots_fired;
		p->stats.s_shots_hit = all_time_stats.s_shots_hit;
		p->stats.s_bonehead_hits = all_time_stats.s_bonehead_hits;

		p->stats.flight_time = all_time_stats.flight_time;
		p->stats.missions_flown = all_time_stats.missions_flown;
		p->stats.last_flown = all_time_stats.last_flown;
		p->stats.last_backup = all_time_stats.last_backup;

		// ship kills (have to find ones that match content)
		list_size = (int)all_time_stats.ship_kills.size();
		for (idx = 0; idx < list_size; idx++) {
			j = all_time_stats.ship_kills[idx].index;

			if (j >= 0) {
				p->stats.kills[j] = all_time_stats.ship_kills[idx].val;
			}
		}

		// medals earned (have to find ones that match content)
		list_size = (int)all_time_stats.medals_earned.size();
		for (idx = 0; idx < list_size; idx++) {
			j = all_time_stats.medals_earned[idx].index;

			if (j >= 0) {
				p->stats.medal_counts[j] = all_time_stats.medals_earned[idx].val;
			}
		}
	}
}

void pilotfile::plr_write_stats()
{
	int idx, list_size = 0;

	startSection(Section::Scoring);

	// global, all-time stats
	cfile::write<int>(all_time_stats.score, cfp);
	cfile::write<int>(all_time_stats.rank, cfp);
	cfile::write<int>(all_time_stats.assists, cfp);
	cfile::write<int>(all_time_stats.kill_count, cfp);
	cfile::write<int>(all_time_stats.kill_count_ok, cfp);
	cfile::write<int>(all_time_stats.bonehead_kills, cfp);

	cfile::write<uint>(all_time_stats.p_shots_fired, cfp);
	cfile::write<uint>(all_time_stats.p_shots_hit, cfp);
	cfile::write<uint>(all_time_stats.p_bonehead_hits, cfp);

	cfile::write<uint>(all_time_stats.s_shots_fired, cfp);
	cfile::write<uint>(all_time_stats.s_shots_hit, cfp);
	cfile::write<uint>(all_time_stats.s_bonehead_hits, cfp);

	cfile::write<uint>(all_time_stats.flight_time, cfp);
	cfile::write<uint>(all_time_stats.missions_flown, cfp);
	cfile::write<int>((int)all_time_stats.last_flown, cfp);
	cfile::write<int>((int)all_time_stats.last_backup, cfp);

	// ship kills (contains ships across all mods, not just current)
	list_size = (int)all_time_stats.ship_kills.size();
	cfile::write<int>(list_size, cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::writeStringLen(all_time_stats.ship_kills[idx].name.c_str(), cfp);
		cfile::write<int>(all_time_stats.ship_kills[idx].val, cfp);
	}

	// medals earned (contains medals across all mods, not just current)
	list_size = (int)all_time_stats.medals_earned.size();
	cfile::write<int>(list_size, cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::writeStringLen(all_time_stats.medals_earned[idx].name.c_str(), cfp);
		cfile::write<int>(all_time_stats.medals_earned[idx].val, cfp);
	}

	endSection();
}

void pilotfile::plr_read_stats_multi()
{
	int idx, j, list_size = 0;
	index_list_t ilist;
	char t_string[NAME_LENGTH+1];

	// global, all-time stats (used only until campaign stats are loaded)
	multi_stats.score = cfile::read<int>(cfp);
	multi_stats.rank = cfile::read<int>(cfp);
	multi_stats.assists = cfile::read<int>(cfp);
	multi_stats.kill_count = cfile::read<int>(cfp);
	multi_stats.kill_count_ok = cfile::read<int>(cfp);
	multi_stats.bonehead_kills = cfile::read<int>(cfp);

	multi_stats.p_shots_fired = cfile::read<uint>(cfp);
	multi_stats.p_shots_hit = cfile::read<uint>(cfp);
	multi_stats.p_bonehead_hits = cfile::read<uint>(cfp);

	multi_stats.s_shots_fired = cfile::read<uint>(cfp);
	multi_stats.s_shots_hit = cfile::read<uint>(cfp);
	multi_stats.s_bonehead_hits = cfile::read<uint>(cfp);

	multi_stats.flight_time = cfile::read<uint>(cfp);
	multi_stats.missions_flown = cfile::read<uint>(cfp);
	multi_stats.last_flown = (_fs_time_t)cfile::read<int>(cfp);
	multi_stats.last_backup = (_fs_time_t)cfile::read<int>(cfp);

	// ship kills (contains ships across all mods, not just current)
	list_size = cfile::read<int>(cfp);
	multi_stats.ship_kills.reserve(list_size);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::readStringLen(t_string, NAME_LENGTH, cfp);

		ilist.name = t_string;
		ilist.index = ship_info_lookup(t_string);
		ilist.val = cfile::read<int>(cfp);

		multi_stats.ship_kills.push_back(ilist);
	}

	// medals earned (contains medals across all mods, not just current)
	list_size = cfile::read<int>(cfp);
	multi_stats.medals_earned.reserve(list_size);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::readStringLen(t_string, NAME_LENGTH,cfp);

		ilist.name = t_string;
		ilist.index = medals_info_lookup(t_string);
		ilist.val = cfile::read<int>(cfp);

		multi_stats.medals_earned.push_back(ilist);
	}

	// if in multiplayer mode then set these stats as the player stats
	if (Game_mode & GM_MULTIPLAYER) {
		p->stats.score = multi_stats.score;
		p->stats.rank = multi_stats.rank;
		p->stats.assists = multi_stats.assists;
		p->stats.kill_count = multi_stats.kill_count;
		p->stats.kill_count_ok = multi_stats.kill_count_ok;
		p->stats.bonehead_kills = multi_stats.bonehead_kills;

		p->stats.p_shots_fired = multi_stats.p_shots_fired;
		p->stats.p_shots_hit = multi_stats.p_shots_hit;
		p->stats.p_bonehead_hits = multi_stats.p_bonehead_hits;

		p->stats.s_shots_fired = multi_stats.s_shots_fired;
		p->stats.s_shots_hit = multi_stats.s_shots_hit;
		p->stats.s_bonehead_hits = multi_stats.s_bonehead_hits;

		p->stats.flight_time = multi_stats.flight_time;
		p->stats.missions_flown = multi_stats.missions_flown;
		p->stats.last_flown = multi_stats.last_flown;
		p->stats.last_backup = multi_stats.last_backup;

		// ship kills (have to find ones that match content)
		list_size = (int)multi_stats.ship_kills.size();
		for (idx = 0; idx < list_size; idx++) {
			j = multi_stats.ship_kills[idx].index;

			if (j >= 0) {
				p->stats.kills[j] = multi_stats.ship_kills[idx].val;
			}
		}

		// medals earned (have to find ones that match content)
		list_size = (int)multi_stats.medals_earned.size();
		for (idx = 0; idx < list_size; idx++) {
			j = multi_stats.medals_earned[idx].index;

			if (j >= 0) {
				p->stats.medal_counts[j] = multi_stats.medals_earned[idx].val;
			}
		}
	}
}

void pilotfile::plr_write_stats_multi()
{
	int idx, list_size = 0;

	startSection(Section::ScoringMulti);

	// global, all-time stats
	cfile::write<int>(multi_stats.score, cfp);
	cfile::write<int>(multi_stats.rank, cfp);
	cfile::write<int>(multi_stats.assists, cfp);
	cfile::write<int>(multi_stats.kill_count, cfp);
	cfile::write<int>(multi_stats.kill_count_ok, cfp);
	cfile::write<int>(multi_stats.bonehead_kills, cfp);

	cfile::write<uint>(multi_stats.p_shots_fired, cfp);
	cfile::write<uint>(multi_stats.p_shots_hit, cfp);
	cfile::write<uint>(multi_stats.p_bonehead_hits, cfp);

	cfile::write<uint>(multi_stats.s_shots_fired, cfp);
	cfile::write<uint>(multi_stats.s_shots_hit, cfp);
	cfile::write<uint>(multi_stats.s_bonehead_hits, cfp);

	cfile::write<uint>(multi_stats.flight_time, cfp);
	cfile::write<uint>(multi_stats.missions_flown, cfp);
	cfile::write<int>((int)multi_stats.last_flown, cfp);
	cfile::write<int>((int)multi_stats.last_backup, cfp);

	// ship kills (contains medals across all mods, not just current)
	list_size = (int)multi_stats.ship_kills.size();
	cfile::write<int>(list_size, cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::writeStringLen(multi_stats.ship_kills[idx].name.c_str(), cfp);
		cfile::write<int>(multi_stats.ship_kills[idx].val, cfp);
	}

	// medals earned (contains medals across all mods, not just current)
	list_size = (int)multi_stats.medals_earned.size();
	cfile::write<int>(list_size, cfp);

	for (idx = 0; idx < list_size; idx++) {
		cfile::io::writeStringLen(multi_stats.medals_earned[idx].name.c_str(), cfp);
		cfile::write<int>(multi_stats.medals_earned[idx].val, cfp);
	}

	endSection();
}

void pilotfile::plr_read_controls()
{
	int idx, list_size, list_axis;
	short id1, id2, id3;
	int axi, inv;

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

	list_axis = cfile::read<int>(cfp);
	for (idx = 0; idx < list_axis; idx++) {
		axi = cfile::read<int>(cfp);
		inv = cfile::read<int>(cfp);

		if (idx < NUM_JOY_AXIS_ACTIONS) {
			Axis_map_to[idx] = axi;
			Invert_axis[idx] = inv;
		}
	}
}

void pilotfile::plr_write_controls()
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

	cfile::write<int>(NUM_JOY_AXIS_ACTIONS, cfp);

	for (idx = 0; idx < NUM_JOY_AXIS_ACTIONS; idx++) {
		cfile::write<int>(Axis_map_to[idx], cfp);
		cfile::write<int>(Invert_axis[idx], cfp);
	}

	endSection();
}

void pilotfile::plr_read_settings()
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

	// detail
	Detail.setting = cfile::read<int>(cfp);
	Detail.nebula_detail = cfile::read<int>(cfp);
	Detail.detail_distance = cfile::read<int>(cfp);
	Detail.hardware_textures = cfile::read<int>(cfp);
	Detail.num_small_debris = cfile::read<int>(cfp);
	Detail.num_particles = cfile::read<int>(cfp);
	Detail.num_stars = cfile::read<int>(cfp);
	Detail.shield_effects = cfile::read<int>(cfp);
	Detail.lighting = cfile::read<int>(cfp);
	Detail.targetview_model = cfile::read<int>(cfp);
	Detail.planets_suns = cfile::read<int>(cfp);
	Detail.weapon_extras = cfile::read<int>(cfp);
}

void pilotfile::plr_write_settings()
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

	// detail
	cfile::write<int>(Detail.setting, cfp);
	cfile::write<int>(Detail.nebula_detail, cfp);
	cfile::write<int>(Detail.detail_distance, cfp);
	cfile::write<int>(Detail.hardware_textures, cfp);
	cfile::write<int>(Detail.num_small_debris, cfp);
	cfile::write<int>(Detail.num_particles, cfp);
	cfile::write<int>(Detail.num_stars, cfp);
	cfile::write<int>(Detail.shield_effects, cfp);
	cfile::write<int>(Detail.lighting, cfp);
	cfile::write<int>(Detail.targetview_model, cfp);
	cfile::write<int>(Detail.planets_suns, cfp);
	cfile::write<int>(Detail.weapon_extras, cfp);

	endSection();
}

void pilotfile::plr_reset_data()
{
	// internals
	m_have_flags = false;
	m_have_info = false;

	m_data_invalid = false;

	// set all the entries in the control config arrays to -1 (undefined)
	control_config_clear();

	// init stats
	p->stats.init();

	// reset scoring lists
	all_time_stats.ship_kills.clear();
	all_time_stats.medals_earned.clear();

	multi_stats.ship_kills.clear();
	multi_stats.medals_earned.clear();

	// clear variables
	p->variables.clear();

	// reset techroom to defaults (CSG will override this, multi will use defaults)
	tech_reset_to_default();
}

void pilotfile::plr_close()
{
	if (cfp) {
		cfile::io::close(cfp);
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

bool pilotfile::load_player(const char *callsign, player *_p)
{
	// if we're a standalone server in multiplayer, just fill in some bogus values
	// since we don't have a pilot file
	if ( (Game_mode & GM_MULTIPLAYER) && (Game_mode & GM_STANDALONE_SERVER) ) {
		Player->insignia_texture = -1;
		strcpy_s(Player->callsign, NOX("Standalone"));
		strcpy_s(Player->short_callsign, NOX("Standalone"));

		return true;
	}

	// set player ptr first thing
	p = _p;

	if ( !p ) {
		Assert( (Player_num >= 0) && (Player_num < MAX_PLAYERS) );
		p = &Players[Player_num];
	}

	filename = callsign;
	filename += ".plr";

	if ( filename.size() == 4 ) {
		mprintf(("PLR => Invalid filename '%s'!\n", filename.c_str()));
		return false;
	}

	cfp = cfile::io::open(filename, cfile::MODE_READ, cfile::OPEN_NORMAL, cfile::TYPE_PLAYERS);

	if ( !cfp ) {
		mprintf(("PLR => Unable to open '%s' for reading!\n", filename.c_str()));
		return false;
	}

	unsigned int plr_id = cfile::read<uint>(cfp);

	if (plr_id != PLR_FILE_ID) {
		mprintf(("PLR => Invalid header id for '%s'!\n", filename.c_str()));
		plr_close();
		return false;
	}

	// version, should be able to just ignore it
	version = cfile::read<ubyte>(cfp);

	mprintf(("PLR => Loading '%s' with version %d...\n", filename.c_str(), version));

	plr_reset_data();

	// the point of all this: read in the PLR contents
	while ( !cfile::io::eof(cfp) ) {
		ushort section_id = cfile::read<ushort>(cfp);
		uint section_size = cfile::read<uint>(cfp);

		size_t start_pos = cfile::io::tell(cfp);

		// safety, to help protect against long reads
		cfile::io::setMaxReadLength(cfp, section_size);

		try {
			switch (section_id) {
				case Section::Flags:
					mprintf(("PLR => Parsing:  Flags...\n"));
					m_have_flags = true;
					plr_read_flags();
					break;

				case Section::Info:
					mprintf(("PLR => Parsing:  Info...\n"));
					m_have_info = true;
					plr_read_info();
					break;

				case Section::Variables:
					mprintf(("PLR => Parsing:  Variables...\n"));
					plr_read_variables();
					break;

				case Section::HUD:
					mprintf(("PLR => Parsing:  HUD...\n"));
					plr_read_hud();
					break;

				case Section::Scoring:
					mprintf(("PLR => Parsing:  Scoring...\n"));
					plr_read_stats();
					break;

				case Section::ScoringMulti:
					mprintf(("PLR => Parsing:  ScoringMulti...\n"));
					plr_read_stats_multi();
					break;

				case Section::Multiplayer:
					mprintf(("PLR => Parsing:  Multiplayer...\n"));
					plr_read_multiplayer();
					break;

				case Section::Controls:
					mprintf(("PLR => Parsing:  Controls...\n"));
					plr_read_controls();
					break;

				case Section::Settings:
					mprintf(("PLR => Parsing:  Settings...\n"));
					plr_read_settings();
					break;

				default:
					mprintf(("PLR => Skipping unknown section 0x%04x!\n", section_id));
					break;
			}
		} catch (cfile::MaxReadLengthException &msg) {
			// read to max section size, move to next section, discarding
			// extra/unknown data
			mprintf(("PLR => (0x%04x) %s\n", section_id, msg.what()));
		} catch (const char *err) {
			mprintf(("PLR => ERROR: %s\n", err));
			plr_close();
			return false;
		}

		// reset safety catch
		cfile::io::setMaxReadLength(cfp, 0);

		// skip to next section (if not already there)
		size_t offset_pos = (start_pos + section_size) - cfile::io::tell(cfp);

		if (offset_pos) {
			cfile::io::seek(cfp, offset_pos, cfile::SEEK_MODE_CUR);
			mprintf(("PLR => WARNING: Advancing to the next section. %i bytes were skipped!\n", offset_pos));
		}
	}

	// restore the callsign into the Player structure
	strcpy_s(p->callsign, callsign);

	// restore the truncated callsign into Player structure
	pilot_set_short_callsign(p, SHORT_CALLSIGN_PIXEL_W);

	player_set_squad_bitmap(p, p->m_squad_filename, true);

	hud_squadmsg_save_keys();

	// set last pilot
	os_config_write_string(NULL, "LastPlayer", (char*)callsign);

	mprintf(("PLR => Loading complete!\n"));

	// cleanup and return
	plr_close();

	return true;
}

bool pilotfile::save_player(player *_p)
{
	// never save a pilot file for the standalone server in multiplayer
	if ( (Game_mode & GM_MULTIPLAYER) && (Game_mode & GM_STANDALONE_SERVER) ) {
		return false;
	}

	// set player ptr first thing
	p = _p;

	if ( !p ) {
		Assert( (Player_num >= 0) && (Player_num < MAX_PLAYERS) );
		p = &Players[Player_num];
	}

	if ( !strlen(p->callsign) ) {
		return false;
	}

	filename = p->callsign;
	filename += ".plr";

	if ( filename.size() == 4 ) {
		mprintf(("PLR => Invalid filename '%s'!\n", filename.c_str()));
		return false;
	}

	// open it, hopefully...
	cfp = cfile::io::open(filename, cfile::MODE_WRITE, cfile::OPEN_NORMAL, cfile::TYPE_PLAYERS);

	if ( !cfp ) {
		mprintf(("PLR => Unable to open '%s' for saving!\n", filename.c_str()));
		return false;
	}

	// header and version
	cfile::write<int>(PLR_FILE_ID, cfp);
	cfile::write<ubyte>(PLR_VERSION, cfp);

	mprintf(("PLR => Saving '%s' with version %d...\n", filename.c_str(), (int)PLR_VERSION));

	// flags and info sections go first
	mprintf(("PLR => Saving:  Flags...\n"));
	plr_write_flags();
	mprintf(("PLR => Saving:  Info...\n"));
	plr_write_info();

	// everything else is next, not order specific
	mprintf(("PLR => Saving:  Scoring...\n"));
	plr_write_stats();
	mprintf(("PLR => Saving:  ScoringMulti...\n"));
	plr_write_stats_multi();
	mprintf(("PLR => Saving:  HUD...\n"));
	plr_write_hud();
	mprintf(("PLR => Saving:  Variables...\n"));
	plr_write_variables();
	mprintf(("PLR => Saving:  Multiplayer...\n"));
	plr_write_multiplayer();
	mprintf(("PLR => Saving:  Controls...\n"));
	plr_write_controls();
	mprintf(("PLR => Saving:  Settings...\n"));
	plr_write_settings();

	// Done!
	mprintf(("PLR => Saving complete!\n"));

	plr_close();

	return true;
}

bool pilotfile::verify(const char *fname, int *rank)
{
	player t_plr;

	// set player ptr first thing
	p = &t_plr;

	filename = fname;

	if ( filename.size() == 4 ) {
		mprintf(("PLR => Invalid filename '%s'!\n", filename.c_str()));
		return false;
	}

	cfp = cfile::io::open((char*)filename.c_str(), cfile::MODE_READ, cfile::OPEN_NORMAL, cfile::TYPE_PLAYERS);

	if ( !cfp ) {
		mprintf(("PLR => Unable to open '%s'!\n", filename.c_str()));
		return false;
	}

	unsigned int plr_id = cfile::read<uint>(cfp);

	if (plr_id != PLR_FILE_ID) {
		mprintf(("PLR => Invalid header id for '%s'!\n", filename.c_str()));
		plr_close();
		return false;
	}

	// version, should be able to just ignore it
	ubyte plr_ver = cfile::read<ubyte>(cfp);

	mprintf(("PLR => Verifying '%s' with version %d...\n", filename.c_str(), (int)plr_ver));

	// the point of all this: read in the PLR contents
	while ( !m_have_flags && !cfile::io::eof(cfp) ) {
		ushort section_id = cfile::read<ushort>(cfp);
		uint section_size = cfile::read<uint>(cfp);

		size_t start_pos = cfile::io::tell(cfp);

		// safety, to help protect against long reads
		cfile::io::setMaxReadLength(cfp, section_size);

		try {
			switch (section_id) {
				case Section::Flags:
					mprintf(("PLR => Parsing:  Flags...\n"));
					m_have_flags = true;
					plr_read_flags();
					break;

				default:
					break;
			}
		} catch (cfile::MaxReadLengthException &msg) {
			// read to max section size, move to next section, discarding
			// extra/unknown data
			mprintf(("PLR => (0x%04x) %s\n", section_id, msg.what()));
		} catch (const char *err) {
			mprintf(("PLR => ERROR: %s\n", err));
			plr_close();
			return false;
		}

		// reset safety catch
		cfile::io::setMaxReadLength(cfp, 0);

		// skip to next section (if not already there)
		size_t offset_pos = (start_pos + section_size) - cfile::io::tell(cfp);

		if (offset_pos) {
			mprintf(("PLR => Warning: (0x%04x) Short read, information may have been lost!\n", section_id));
			cfile::io::seek(cfp, offset_pos, cfile::SEEK_MODE_CUR);
		}
	}

	if (rank) {
		*rank = p->stats.rank;
	}

	mprintf(("PLR => Verifying complete!\n"));

	// cleanup and return
	plr_close();

	return true;
}

