#include "LuaCallback.h"


LuaCallback::LuaCallback(lua_State* L) : LuaValue( L ),
	isCFunction( false )
{
	neededLuaType = LUA_TFUNCTION;
}

LuaCallback::LuaCallback(const LuaCallback& other) : LuaValue( other )
{
	Assertion(this->luaType == LUA_TFUNCTION, "Illegal value type after copying! Expecting \"function\" got \"%s\"",
		lua_typename(luaState, this->luaType));
	
	neededLuaType = LUA_TFUNCTION;
}

LuaCallback::LuaCallback(lua_State* L, const SCP_string& code, const SCP_string& name) : LuaValue( L ),
	isCFunction( false )
{
	neededLuaType = LUA_TFUNCTION;
	this->parseCode(code, name);
}

LuaCallback::~LuaCallback()
{
}

void LuaCallback::parseCode(const SCP_string& code, const SCP_string& name)
{
	int err = luaL_loadbuffer(this->luaState, code.c_str(), code.length(), name.c_str());

	if (!err)
	{
		this->setReference(LuaReference::create(luaState));
	}
	else
	{
		// Get the error message
		size_t len;
		const char* err = lua_tolstring(luaState, -1, &len);

		lua_pop(luaState, 1);

		throw SCP_string(err, len);
	}
}

void LuaCallback::setCFunction(lua_CFunction func)
{
	Assertion(func != NULL, "Illegal lua_CFunction pointer passed!");

	lua_pushcfunction(luaState, func);

	this->setReference(LuaReference::create(luaState));

	lua_pop(luaState, 1);
}

bool LuaCallback::setEnvironment(const LuaTable& table)
{
	Assert(table.getReference().isValid());

	this->pushValue();
	table.pushValue();
	
	lua_pop(luaState, 1);

	return lua_setfenv(luaState, -2) != 0;
}

LuaValueList LuaCallback::operator()(const LuaValueList& args)
{
	return this->call(args);
}

int callbackErrorFunc(lua_State* L)
{
	LuaError(L);

	return LUA_ERRRUN;
}

LuaValueList LuaCallback::call(const LuaValueList& args)
{
	// push the error function
	lua_pushcfunction(luaState, callbackErrorFunc);
	int err_idx = lua_gettop(luaState);

	// Push the arguments onto the stack
	for (LuaValueList::const_iterator iter = args.begin(); iter != args.end(); ++iter)
	{
		LuaConvert::pushValue(luaState, *iter);
	}

	this->pushValue();

	// actually call the function now!
	int err = lua_pcall(luaState, args.size(), LUA_MULTRET, err_idx);

	// Reset number of arguments
	if (!err)
	{
		int numReturn = lua_gettop(luaState) - err_idx;
		LuaValueList values;
		values.reserve(numReturn);

		LuaValue val(luaState);
		for (int i = 0; i< numReturn; ++i)
		{
			if (LuaConvert::popValue(luaState, val))
			{
				// Add values at the begin as the last return value is on top
				// of the stack.
				values.insert(values.begin(), val);
			}
		}

		// Remove the error function
		lua_remove(luaState, err_idx);
		return values;
	}
	else
	{
		LuaError(luaState, "Failed to execute function: %s", lua_tostring(luaState, -1));

		// Pop the error function and the error message
		lua_pop(luaState, 2);
		return LuaValueList();
	}
}
