
#ifndef _LUA_GLOBALS_H
#define _LUA_GLOBALS_H

#include "parse/scripting.h"
#include "parse/lua.h"
#include "object/object.h"
#include "hud/hudscripting.h"

//*************************Lua globals*************************
extern ade_obj<object_h> l_Object;
extern ade_obj<object_h> l_Weapon;
extern ade_obj<object_h> l_Ship;
extern ade_obj<object_h> l_Debris;
extern ade_obj<object_h> l_Asteroid;

extern ade_obj<hud_gauge_h> l_HudGauge;
extern ade_obj<hud_gauge_h> l_ScriptingHudGauge;

#endif
