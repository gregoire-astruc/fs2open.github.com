
#include "hud/hudscripting.h"

ScriptingGauge::ScriptingGauge():
	HudGauge(HUD_OBJECT_SCRIPTING, HUD_TEXT_FLASH, true, false, 0, 255, 255, 255)
{
	
}

ScriptingGauge::~ScriptingGauge()
{
	if (renderCallback)
	{
		delete renderCallback;
	}

	if (updateCallback)
	{
		delete updateCallback;
	}
}

void ScriptingGauge::initName(const SCP_string& name)
{
	custom_name = name;
}

void ScriptingGauge::setUpdateCallback(const LuaCallback& callback)
{
	if (updateCallback)
	{
		delete updateCallback;
		updateCallback = NULL;
	}

	updateCallback = new LuaCallback(callback);
}

void ScriptingGauge::setRenderCallback(const LuaCallback& callback)
{
	if (renderCallback)
	{
		delete renderCallback;
		renderCallback = NULL;
	}

	renderCallback = new LuaCallback(callback);
}

void ScriptingGauge::render(float frametime)
{
	if (renderCallback)
	{
		LuaValueList list;

		ade_odata data = l_HudGauge.Set(hud_gauge_h(this));

		list.push_back(LuaValue(renderCallback->luaState, data));
		list.push_back(LuaValue(renderCallback->luaState, frametime));

		renderCallback->call(list);
	}
}

void ScriptingGauge::onFrame(float frametime)
{
	if (updateCallback)
	{
		LuaValueList list;

		ade_odata data = l_HudGauge.Set(hud_gauge_h(this));

		list.push_back(LuaValue(updateCallback->luaState, data));
		list.push_back(LuaValue(updateCallback->luaState, frametime));

		updateCallback->call(list);
	}
}
