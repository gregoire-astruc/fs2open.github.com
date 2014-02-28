
#ifndef MAINLOOP_H
#define MAINLOOP_H
#pragma once

#include <functional>

#include "globalincs/pstypes.h"

namespace mainloop
{
	void init();

	void execute();

	void shutdown();

	void addMainloopFunction(const std::function<bool()>& function);
}

#endif // MAINLOOP_H
