
#include "statelogic/statelogic.h"

#include "freespace.h"
#include "parse/parselo.h"

#include <boost/unordered_map.hpp>

#include <chromium/ChromiumStateLogic.h>

#include <boost/shared_ptr.hpp>

#include <boost/algorithm/string.hpp>

namespace statelogic
{
	using namespace boost;

	typedef unordered_map<GameState, shared_ptr<StateLogic>> LogicMap;

	LogicMap gameLogicObjects;

	void parse_html_replacement(GameState state)
	{
		required_string("+URL:");
		SCP_string url;
		stuff_string(url, F_NAME);

		setStateLogic(state, new chromium::ChromiumStateLogic(url));
	}

	void parse_state_table(const char* filename)
	{
		read_file_text(filename, cfile::TYPE_TABLES);
		reset_parse();

		required_string("#States");

		while (optional_string("$State:"))
		{
			SCP_string stateName;
			stuff_string(stateName, F_NAME);

			char** found = std::find_if(GS_state_text, GS_state_text + GS_NUM_STATES,
				[&](const char* name){return iequals(name, stateName); });

			if (found == GS_state_text + GS_NUM_STATES)
			{
				Warning(LOCATION, "Couldn't find game state \"%s\"!", stateName.c_str());
				skip_to_start_of_string("$State:");
				continue;
			}

			size_t index = std::distance(GS_state_text, found);
			GameState state = static_cast<GameState>(index);

			Assertion(index >= GS_STATE_MAIN_MENU && index < GS_NUM_STATES,
				"State index is out of range, name \"%s\" might be invalid!", stateName.c_str());

			required_string("+Type:");
			SCP_string type;
			stuff_string(type, F_NAME);

			if (iequals(type, "HTML"))
			{
				parse_html_replacement(state);
			}
			else
			{
				Warning(LOCATION, "Unknown replacement type %s!", type.c_str());
			}
		}

		required_string("#End");
	}

	void init()
	{
		if (cfile::exists("states.tbl", cfile::TYPE_TABLES))
		{
			parse_state_table("states.tbl");
		}

		parse_modular_table("*-sta.tbm", parse_state_table);
	}

	bool hasCustomLogic(GameState state)
	{
		return gameLogicObjects.find(state) != gameLogicObjects.end();
	}

	void enterState(GameState old_state, GameState new_state)
	{
		if (hasCustomLogic(new_state))
		{
			gameLogicObjects[new_state]->enterState(old_state);
		}
		else
		{
			game_enter_state(old_state, new_state);
		}
	}

	void doFrame(GameState state)
	{
		if (hasCustomLogic(state))
		{
			gameLogicObjects[state]->doFrame();
		}
		else
		{
			game_do_state(state);
		}
	}

	void leaveState(GameState old_state, GameState new_state)
	{
		if (hasCustomLogic(old_state))
		{
			gameLogicObjects[old_state]->leaveState(new_state);
		}
		else
		{
			game_leave_state(old_state, new_state);
		}
	}

	void setStateLogic(GameState state, StateLogic* logic)
	{
		gameLogicObjects.insert(std::make_pair(state, shared_ptr<StateLogic>(logic)));
	}

	void shutdown()
	{
		gameLogicObjects.clear();
	}
}