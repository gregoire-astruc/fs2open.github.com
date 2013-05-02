#include "LuaValue.h"


LuaValue::LuaValue(lua_State* state) : luaState( state ), luaType( LUA_TNONE ), neededLuaType( LUA_TNONE )
{
	Assertion(state != NULL, "Invalid lua state passed to LuaValue constructor!");
}

LuaValue::LuaValue(const LuaValue& other) : luaState( other.luaState ), neededLuaType( LUA_TNONE )
{
	other.pushValue();

	this->setReference(LuaReference::create(this->luaState, -1));
}

LuaValue::~LuaValue()
{
}

bool LuaValue::operator==(const LuaValue& other)
{
	Assert(this->luaState == other.luaState);

	this->pushValue();
	other.pushValue();

	bool result = lua_equal(this->luaState, -2, -1) != 0;

	lua_pop(luaState, 2);

	return result;
}

bool LuaValue::operator<(const LuaValue& other)
{
	Assert(this->luaState == other.luaState);

	this->pushValue();
	other.pushValue();

	bool result = lua_lessthan(luaState, -2, -1) != 0;

	lua_pop(luaState, 2);

	return result;
}

bool LuaValue::operator<=(const LuaValue& other)
{
	Assert(this->luaState == other.luaState);

	this->pushValue();
	other.pushValue();

	bool result = lua_equal(luaState, -2, -1) != 0;

	if (!result)
	{
		result = lua_lessthan(luaState, -2, -1) != 0;
	}

	lua_pop(luaState, 2);

	return result;
}

bool LuaValue::operator>=(const LuaValue& other)
{
	Assert(this->luaState == other.luaState);

	this->pushValue();
	other.pushValue();

	bool result = lua_equal(luaState, -2, -1) != 0;

	if (!result)
	{
		// negate the "<" operation
		result = lua_lessthan(luaState, -2, -1) == 0;
	}

	lua_pop(luaState, 2);

	return result;
}

void LuaValue::setReference(const LuaReference& reference)
{
	this->reference = reference;

	if (this->reference.isValid())
	{
		this->reference.pushValue();

		this->luaType = lua_type(luaState, -1);

		lua_pop(luaState, 1);
	}
}

LuaReference LuaValue::getReference() const
{
	return reference;
}

int LuaValue::getLuaType() const
{
	return luaType;
}

void LuaValue::pushValue() const
{
	if (this->reference.isValid())
	{
		this->reference.pushValue();

		// Type check...
		Assertion(lua_type(luaState, -1) == luaType, "Illegal reference type! Expecting \"%s\", got \"s\".", 
			lua_typename(luaState, luaType), lua_typename(luaState, lua_type(luaState, -1)));
	}
}