#ifndef __LUAVALUE_H__
#define __LUAVALUE_H__

#include "parse/lua/LuaUtil.h"

/**
 * @brief Represents a Lua-value
 *
 * This class holds a reference to a lua value and provides type checking to ensure that the value is still the same.
 */
class LuaValue
{
public:
	/**
	 * @brief Initializes the lua value
	 *
	 * The instance does not point to a lua-value after the constructor has finished. To reference
	 * a value use #getReference()
	 *
	 * @param state The lua state
	 */
	LuaValue(lua_State* state);

	/**
	 * @brief Copy-constructor
	 * @param other The other LuaValue.
	 */
	LuaValue(const LuaValue& other);

	template<class ValueType>
	LuaValue(lua_State* state, const ValueType value);

	/**
	 * @brief Releases the reference
	 */
	virtual ~LuaValue();

	/**
	 * @brief Sets a new LuaReference.
	 *
	 * @param reference The new lua reference.
	 */
	virtual void setReference(const LuaReference& reference);

	/**
	 * @brief Gets the LuaReference.
	 *
	 * This reference is used to reference the actual lua value.
	 *
	 * @return The LuaReference instance.
	 */
	virtual LuaReference getReference() const;

	/**
	 * @brief Gets the lua type of this value.
	 * @return One of the LUA_T* defines.
	 */
	virtual int getLuaType() const;

	template<class ValueType>
	bool setValue(const ValueType target);

	template<class ValueType>
	bool getValue(ValueType& target) const;

	/**
	 * @brief Pushes this lua value onto the stack.
	 */
	virtual void pushValue() const;

	/**
	 * @brief Checks for equality of the lua values.
	 * @param other The other lua value.
	 * @return `true` when the value are equal as specified by the lua "==" operator.
	 */
	virtual bool operator==(const LuaValue& other);

	/**
	 * @brief Checks for inequality of the lua values.
	 * @param other The other lua value.
	 * @return `true` when the value are equal as specified by the lua "~=" operator.
	 */
	virtual bool operator!=(const LuaValue& other)
	{
		return !(*this == other);
	}

	/**
	 * @brief Checks if the other value is bigger than this value.
	 * @param other The other lua value.
	 * @return `true` when the second value is bigger than this value as specified
	 * 			by lua.
	 */
	virtual bool operator<(const LuaValue& other);

	/**
	 * @brief Checks if this value is bigger than the other value.
	 * @param other The other lua value.
	 * @return `true` when this value is bigger than the other value as specified
	 * 			by lua.
	 */
	virtual bool operator>(const LuaValue& other)
	{
		return !(*this <= other);
	}

	/**
	 * @brief Check if this value is less than or equal to the other value.
	 * @param other The other value.
	 * @return `true` when this value is less than or equal to the other value.
	 */
	virtual bool operator<=(const LuaValue& other);

	/**
	 * @brief Check if this value is bigger than or equal to the other value.
	 * @param other The other value.
	 * @return `true` when this value is bigger than or equal to the other value.
	 */
	virtual bool operator>=(const LuaValue& other);

	lua_State* luaState; //!< The lua state of this value.
protected:
	LuaReference reference;

	int luaType;

	int neededLuaType;
};

template<class ValueType>
LuaValue::LuaValue(lua_State* state, const ValueType value) : luaState( state ), luaType( LUA_TNONE ), neededLuaType( LUA_TNONE )
{
	Assert(luaState != NULL);

	LuaConvert::pushValue(luaState, value);

	setReference(LuaReference::create(luaState));
}

template<class ValueType>
bool LuaValue::setValue(const ValueType target)
{
	LuaConvert::pushValue(luaState, target);

	this->setReference(LuaReference::create(luaState));

	Assertion(neededLuaType == LUA_TNONE || luaType == neededLuaType, "Needed lua type and actual lua type do not match!\n"
		"Got type \"%s\" but need \"%s\"!", lua_typename(luaState, luaType), lua_typename(luaState, neededLuaType));

	return true;
}

template<class ValueType>
bool LuaValue::getValue(ValueType& target) const
{
	this->pushValue();

	bool ret = LuaConvert::popValue(luaState, target);

	if (!ret)
	{
		// If the operation wasn't successful then we need to pop the value manually
		lua_pop(luaState, 1);
	}

	return ret;
}

#endif
