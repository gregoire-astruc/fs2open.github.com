#include "LuaTable.h"

LuaTable::LuaTable(lua_State* L) : LuaValue( L )
{
	neededLuaType = LUA_TTABLE;
}

LuaTable::LuaTable(const LuaTable& other) : LuaValue( other )
{
	Assertion(this->luaType == LUA_TTABLE, "Illegal value type after copying! Expecting \"table\" got \"%s\"",
		lua_typename(luaState, this->luaType));

	neededLuaType = LUA_TTABLE;
}

LuaTable::~LuaTable()
{
}

bool LuaTable::setMetatable(const LuaTable& table)
{
	Assert(table.getReference().lock()->isValid());

	this->pushValue();
	table.pushValue();

	lua_setmetatable(luaState, -2);

	lua_pop(luaState, 1);

	return true;
}

void LuaTable::create()
{
	lua_newtable(luaState);

	setReference(LuaReference::create(luaState));

	lua_pop(luaState, 1);
}

void LuaTable::setGlobal(SCP_string name)
{
	lua_setglobal(luaState, name.c_str());
}

size_t LuaTable::getLength()
{
	this->pushValue();

	size_t length = lua_objlen(luaState, -1);

	lua_pop(luaState, 1);

	return length;
}

LuaTableIterator LuaTable::iterator()
{
	this->pushValue();

	return LuaTableIterator(this);
}

LuaTableIterator::LuaTableIterator(LuaTable* parent) : parent( parent )
{
	Assert(parent != NULL);

	// Prepare the iteration
	lua_pushnil(parent->luaState);
}

bool LuaTableIterator::toNext()
{
	return lua_next(parent->luaState, -2) != 0;
}
