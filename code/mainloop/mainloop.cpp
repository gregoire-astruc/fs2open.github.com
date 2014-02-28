
#include <functional>

#include "mainloop/mainloop.h"
#include "gamesequence/gamesequence.h"
#include "osapi/osapi.h"

namespace mainloop
{
	SCP_vector<std::function<bool()>> mainloopFunctions;

	bool standardMainloop()
	{
		int state = gameseq_process_events();

		if (state == GS_STATE_QUIT_GAME)
		{
			return false;
		}

		return true;
	}

	bool osPoll()
	{
		os_poll();

		return true;
	}

	void init()
	{
		addMainloopFunction(osPoll);
		addMainloopFunction(standardMainloop);
	}

	void execute()
	{
		bool execute = true;
		while (execute)
		{
			for (const std::function<bool()>& func : mainloopFunctions)
			{
				if (!func())
				{
					execute = false;
					break;
				}
			}
		}
	}

	void shutdown()
	{
		mainloopFunctions.clear();
	}

	void addMainloopFunction(const std::function<bool()>& function)
	{
		mainloopFunctions.push_back(function);
	}
}
