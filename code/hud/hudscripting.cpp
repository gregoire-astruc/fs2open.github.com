
#include "hud/hudscripting.h"
#include "parse/lua_globals.h"

ScriptingGauge::ScriptingGauge():
	HudGauge(HUD_OBJECT_SCRIPTING, HUD_TEXT_FLASH, true, false, 0, 255, 255, 255),
	updateCallback(NULL), renderCallback(NULL)
{
	// Scripting gauges are always custom gauges
	custom_gauge = true;
}

ScriptingGauge::~ScriptingGauge()
{
	if (updateCallback)
	{
		delete updateCallback;
	}

	if (renderCallback)
	{
		delete renderCallback;
	}
}

void ScriptingGauge::initName(const SCP_string& name)
{
	custom_name = name;
}

void ScriptingGauge::setUpdateCallback(const LuaCallback& callback)
{
	this->resetUpdateCallback();

	updateCallback = new LuaCallback(callback);
}

void ScriptingGauge::resetUpdateCallback()
{
	if (updateCallback)
	{
		delete updateCallback;
	}
	
	updateCallback = NULL;
}

void ScriptingGauge::setRenderCallback(const LuaCallback& callback)
{
	this->resetRenderCallback();

	renderCallback = new LuaCallback(callback);
}

void ScriptingGauge::resetRenderCallback()
{
	if (renderCallback)
	{
		delete renderCallback;
	}
	
	renderCallback = NULL;
}

LuaCallback* ScriptingGauge::getUpdateCallback()
{
	return updateCallback;
}

LuaCallback* ScriptingGauge::getRenderCallback()
{
	return renderCallback;
}

void ScriptingGauge::render(float frametime)
{
	if (renderCallback)
	{
		LuaValueList list;

		list.push_back(LuaValue(renderCallback->luaState, l_ScriptingHudGauge.Set(hud_gauge_h(this))));
		list.push_back(LuaValue(renderCallback->luaState, frametime));
		list.push_back(LuaValue(renderCallback->luaState, position[0]));
		list.push_back(LuaValue(renderCallback->luaState, position[1]));

		renderCallback->call(list);
	}
}

bool ScriptingGauge::canRender()
{
	return true;
}

void ScriptingGauge::onFrame(float frametime)
{
	if (updateCallback)
	{
		LuaValueList list;

		list.push_back(LuaValue(renderCallback->luaState, l_ScriptingHudGauge.Set(hud_gauge_h(this))));
		list.push_back(LuaValue(renderCallback->luaState, frametime));

		updateCallback->call(list);
	}
}
