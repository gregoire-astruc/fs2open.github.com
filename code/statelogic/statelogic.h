
#ifndef STATELOGIC_H
#define STATELOGIC_H
#pragma once

#include "globalincs/pstypes.h"
#include "gamesequence/gamesequence.h"

namespace statelogic
{
	class StateLogic
	{
	public:
		virtual void enterState(GameState oldState) = 0;

		virtual void doFrame() = 0;

		virtual void leaveState(GameState newState) = 0;
	};

	void init();

	void enterState(GameState oldState, GameState newState);

	void doFrame(GameState state);

	void leaveState(GameState oldState, GameState newState);

	void setStateLogic(GameState state, StateLogic* logic);

	void shutdown();
}

#endif // STATELOGIC_H
