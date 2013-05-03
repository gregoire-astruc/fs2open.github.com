#include "parse/lua/LuaUtil.h"
#include "parse/lua/LuaTable.h"
#include "parse/lua/LuaCallback.h"

LuaReference LuaReference::create(lua_State* state, int position)
{
	Assert(state != NULL);

	lua_pushvalue(state, position);

	LuaReference ref(state, luaL_ref(state, LUA_REGISTRYINDEX));

	ref.pushValue();

	lua_replace(state, position);

	return ref;
}

LuaReference::LuaReference(lua_State* state) :
		luaState(state)
{
	Assert(state != NULL);
}

LuaReference::LuaReference(lua_State* state, int reference) :
		luaState(state), reference(reference)
{
	Assert(state != NULL);
	Assertion(reference >= 0, "Illegal reference number, got %d!", reference);
}

LuaReference::LuaReference(const LuaReference& other) :
		luaState(NULL), reference(-1)
{
	if (other.isValid())
	{
		this->luaState = other.luaState;

		other.pushValue();

		Assert(luaState != NULL);

		this->reference = luaL_ref(luaState, LUA_REGISTRYINDEX);
	}
}

LuaReference::~LuaReference()
{
	if (!keepReference)
	{
		this->removeReference();
	}
}

bool LuaReference::removeReference()
{
	Assert(luaState != NULL);

	if (this->isValid())
	{
		luaL_unref(luaState, LUA_REGISTRYINDEX, reference);
		return true;
	}
	else
	{
		return false;
	}
}

void LuaReference::setKeepReference(bool keep)
{
	this->keepReference = keep;
}

int LuaReference::getReference() const
{
	return reference;
}

bool LuaReference::isValid() const
{
	if (luaState == NULL)
	{
		return false;
	}

	if (reference < 0)
	{
		return false;
	}

	return true;
}

void LuaReference::pushValue() const
{
	if (this->isValid())
	{
		lua_rawgeti(luaState, LUA_REGISTRYINDEX, this->getReference());
	}
}

bool isValidIndex(lua_State* state, int index)
{
	if (1 <= abs(index) && abs(index) <= lua_gettop(state))
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<>
void LuaConvert::pushValue<double>(lua_State* luaState, const double value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushnumber(luaState, value);
}

template<>
void LuaConvert::pushValue<float>(lua_State* luaState, const float value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushnumber(luaState, value);
}

template<>
void LuaConvert::pushValue<int>(lua_State* luaState, const int value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushnumber(luaState, value);
}

template<>
void LuaConvert::pushValue<SCP_string>(lua_State* luaState,
		const SCP_string value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushlstring(luaState, value.c_str(), value.size());
}

template<>
void LuaConvert::pushValue<const char*>(lua_State* luaState, const char* value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushstring(luaState, value);
}

template<>
void LuaConvert::pushValue<bool>(lua_State* luaState, const bool value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushboolean(luaState, value);
}

template<>
void LuaConvert::pushValue<fix>(lua_State* luaState, const fix value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushnumber(luaState, f2fl(value));
}

template<>
void LuaConvert::pushValue<ade_odata>(lua_State* luaState,
		const ade_odata value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	// Copied from lua.cpp

	//WMC - char must be 1 byte, foo.
	Assert(sizeof(char)==1);

	//Create new LUA object and get handle
	char *newod = (char*) lua_newuserdata(luaState,
			value.size + sizeof(ODATA_SIG_TYPE));
	//Create or get object metatable
	luaL_getmetatable(luaState, Ade_table_entries[value.idx].Name);
	//Set the metatable for the object
	lua_setmetatable(luaState, -2);

	//Copy the actual object data to the Lua object
	memcpy(newod, value.buf, value.size);

	//Also copy in the unique sig
	if (value.sig != NULL)
		memcpy(newod + value.size, value.sig, sizeof(ODATA_SIG_TYPE));
	else
	{
		ODATA_SIG_TYPE tempsig = ODATA_SIG_DEFAULT;
		memcpy(newod + value.size, &tempsig, sizeof(ODATA_SIG_TYPE));
	}
}

template<>
void LuaConvert::pushValue<lua_CFunction>(lua_State* luaState,
		const lua_CFunction value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	lua_pushcfunction(luaState, value);
}

template<>
void LuaConvert::pushValue<LuaTable>(lua_State* luaState, const LuaTable value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(value.luaState == luaState,
			"Table and pushValue use different lua_States! Get a coder!");

	value.pushValue();
}

template<>
void LuaConvert::pushValue<LuaCallback>(lua_State* luaState,
		const LuaCallback value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(value.luaState == luaState,
			"Table and pushValue use different lua_States! Get a coder!");

	value.pushValue();
}

template<>
void LuaConvert::pushValue<LuaValue>(lua_State* luaState,
		const LuaValue value)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(value.luaState == luaState,
			"Value and pushValue use different lua_States! Get a coder!");

	value.pushValue();
}

template<>
bool LuaConvert::popValue<double>(lua_State* luaState, double& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isnumber(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = lua_tonumber(luaState, stackposition);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<float>(lua_State* luaState, float& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isnumber(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = (float) lua_tonumber(luaState, stackposition);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<int>(lua_State* luaState, int& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isnumber(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = (int) lua_tonumber(luaState, stackposition);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<SCP_string>(lua_State* luaState, SCP_string& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isstring(luaState, stackposition))
	{
		return false;
	}
	else
	{
		size_t size;
		const char* string = lua_tolstring(luaState, stackposition, &size);
		target.assign(string, size);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<const char*>(lua_State* luaState, const char*& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	Assertion(target != NULL,
			"Illegal char value target pointer passed to LuaConvert::popValue!");

	if (!lua_isstring(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = lua_tostring(luaState, stackposition);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<bool>(lua_State* luaState, bool& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isboolean(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = lua_toboolean(luaState, stackposition) != 0;

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<fix>(lua_State* luaState, fix& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	if (!lua_isnumber(luaState, stackposition))
	{
		return false;
	}
	else
	{
		float num = (float) lua_tonumber(luaState, stackposition);

		target = fl2f(num);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<ade_odata>(lua_State* luaState, ade_odata& od,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);

	// Copied from lua.cpp
	bool ret = false;

	if (lua_isuserdata(luaState, stackposition))
	{
		//WMC - Get metatable
		lua_getmetatable(luaState, stackposition);
		int mtb_ldx = lua_gettop(luaState);
		Assert(!lua_isnil(luaState, -1));

		//Get ID
		lua_pushstring(luaState, "__adeid");
		lua_rawget(luaState, mtb_ldx);

		if (lua_tonumber(luaState, -1) != od.idx)
		{
			lua_pushstring(luaState, "__adederivid");
			lua_rawget(luaState, mtb_ldx);
			if ((uint) lua_tonumber(luaState, -1) != od.idx)
			{
				LuaError(luaState,
						"Table value is the wrong type of userdata; '%s' given, but '%s' expected",
						Ade_table_entries[(uint) lua_tonumber(luaState, -2)].Name,
						Ade_table_entries[od.idx].GetName());
			}
			lua_pop(luaState, 1);
		}
		lua_pop(luaState, 2);
		if (od.size != ODATA_PTR_SIZE)
		{
			memcpy(od.buf, lua_touserdata(luaState, stackposition), od.size);
			if (od.sig != NULL)
			{
				//WMC - char must be 1
				Assert(sizeof(char) == 1);
				//WMC - Yuck. Copy sig data.
				//Maybe in the future I'll do a packet userdata thing.
				(*od.sig) = *(ODATA_SIG_TYPE*) (*(char **) od.buf + od.size);
			}

			ret = true;
		}
		else
		{
			(*(void**) od.buf) = lua_touserdata(luaState, stackposition);

			ret = true;
		}

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}
	}

	return ret;
}

template<>
bool LuaConvert::popValue<lua_CFunction>(lua_State* luaState,
		lua_CFunction& target, int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);
	Assertion(target != NULL,
			"Invalid c-function value target pointer given to LuaConvert::popValue!");

	if (!lua_iscfunction(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target = lua_tocfunction(luaState, stackposition);

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<LuaTable>(lua_State* luaState, LuaTable& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);
	Assertion(target.luaState == luaState,
			"Target and popValue use different lua_States!");

	if (!lua_istable(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target.setReference(LuaReference::create(luaState, stackposition));

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<LuaCallback>(lua_State* luaState, LuaCallback& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);
	Assertion(target.luaState == luaState,
			"Target and popValue use different lua_States!");

	if (!lua_isfunction(luaState, stackposition))
	{
		return false;
	}
	else
	{
		target.setReference(LuaReference::create(luaState, stackposition));

		if (remove)
		{
			lua_remove(luaState, stackposition);
		}

		return true;
	}
}

template<>
bool LuaConvert::popValue<LuaValue>(lua_State* luaState, LuaValue& target,
		int stackposition, bool remove)
{
	Assertion(luaState != NULL, "Invalid lua_State pointer passed!");
	Assertion(isValidIndex(luaState, stackposition),
			"Index %d is no valid stack position!", stackposition);
	Assertion(target.luaState == luaState,
			"Target and popValue use different lua_States!");

	target.setReference(LuaReference::create(luaState, stackposition));

	if (remove)
	{
		lua_remove(luaState, stackposition);
	}

	return true;
}
