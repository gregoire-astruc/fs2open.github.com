
#ifndef _HUDSCRIPTING_H
#define _HUDSCRIPTING_H

#include "hud/hud.h"
#include "parse/lua.h"
#include "parse/lua/LuaCallback.h"

// Defined here to be publically available
class hud_gauge_h
{
private:
	HudGauge* hud_gauge;

public:
	hud_gauge_h() : hud_gauge( NULL ) {}

	hud_gauge_h(HudGauge* gauge) : hud_gauge( gauge )
	{ 
		Assertion(gauge != NULL, "Hud gauge pointer is not valid!");
	}

	HudGauge* Get() { return hud_gauge; }

	bool IsValid() { return hud_gauge != NULL; }
};

static ade_obj<hud_gauge_h> l_HudGauge("HudGauge", "HUD Gauge handle");

class ScriptingGauge : public HudGauge
{
private:
	LuaCallback* updateCallback;
	LuaCallback* renderCallback;

public:
	ScriptingGauge();
	virtual ~ScriptingGauge();

	void initName(const SCP_string& name);

	void setUpdateCallback(const LuaCallback& callback);
	void setRenderCallback(const LuaCallback& callback);

	void render(float frametime);
	void onFrame(float frametime);
};

#endif // _HUDSCRIPTING_H
