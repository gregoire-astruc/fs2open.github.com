
#ifndef __LUATABLE_H__
#define __LUATABLE_H__

#include "parse/lua.h"
#include "parse/lua/LuaUtil.h"
#include "parse/lua/LuaValue.h"

class LuaTable;

/**
 * @brief An iterator for a lua table.
 *
 * Provides a way to iterate over the contents of a lua table:
 *
 * @code{.cpp}
 * LuaTable t = ...
 *
 * LuaTableIterator iter = t.tableIterator();
 *
 * while (iter.toNext())
 * {
 *     <KeyType> key;
 *     <ValueType> value;
 *
 *     LuaConvert::popValue(luaState, key, -2, false);
 *     LuaConvert::popValue(luaState, value, -1);
 * }
 * @endcode
 */
class LuaTableIterator
{
public:
	/**
	 * @brief Moves to the next key-value pair of the table.
	 *
	 * @return @c true when a key-value pair is available, @c false otehrwise
	 */
	bool toNext();
private:
	LuaTableIterator(LuaTable* parent);

	LuaTable* parent;

	friend class LuaTable;
};

/**
 * @brief Class to improve handling of lua tables.
 *
 * This class provides a high-level interface to lua tables without the need to directly call
 * lua-API functions.
 *
 * @see LuaConvert
 */
class LuaTable : public LuaValue
{
public:
	/**
	 * @brief Initializes this table instance.
	 *
	 * Sets the internal lua_State pointer to the specified pointer.
	 *
	 * @param L The state pointer which should be used for the operations
	 */
	LuaTable(lua_State* L);

	/**
	 * @brief Copy-constructor
	 * @param other The other table.
	 */
	LuaTable(const LuaTable& other);

	/**
	 * Dereferences the stored reference to the table if it exists.
	 */
	~LuaTable();

	/**
	 * @brief Sets the metatable.
	 *
	 * The given table will be set as the metatable of this table.
	 *
	 * @param ref The new metatable.
	 * @return Always `true`.
	 */
	bool setMetatable(const LuaTable& ref);

	/**
	 * @brief Adds a value to this lua table.
	 *
	 * @param index The index value to use.
	 * @param value The value to set at the index.
	 */
	template<class IndexType, class ValueType>
	void addValue(const IndexType index, const ValueType value);

	/**
	 * @brief Retrieves a value from the table.
	 *
	 * @param index The index where the value is located.
	 * @param target The target location where the value should be stored.
	 * @return @true when the value could be successfully converted, @false otherwise
	 */
	template<class IndexType, class ValueType>
	bool getValue(const IndexType index, ValueType& target);

	/**
	 * @brief Sets the table as global variable
	 * @param name The name for the global variable
	 */
	void setGlobal(SCP_string name);

	/**
	 * @brief Creates a new table.
	 *
	 * Creates a new table and references it. If there was a reference before then it is freed.
	 */
	void create();

	/**
	 * @brief Gets the length of the table.
	 *
	 * Gets size of the table (the same as using the '#' operator in Lua).
	 *
	 * @return The size value.
	 */
	size_t getLength();

	/**
	 * @brief Creates LuaTableITerator to iterate over the contents of the table.
	 * @return The iterator instance.
	 */
	LuaTableIterator iterator();
};

template<class IndexType, class ValueType>
inline void LuaTable::addValue(const IndexType index, const ValueType value)
{
	// Push the table onto the stack by using the reference
	this->pushValue();

	// Push the index and value onto the stac by using the template functions
	LuaConvert::pushValue(luaState, index);
	LuaConvert::pushValue(luaState, value);

	// Set the value in the table
	lua_settable(luaState, -3);

	// And pop the table again
	lua_pop(luaState, 1);
}

template<class IndexType, class ValueType>
bool LuaTable::getValue(const IndexType index, ValueType& target)
{
	this->pushTable();

	LuaConvert::pushValue(luaState, index);

	lua_gettable(luaState, -2);

	bool ret = LuaConvert::popValue(luaState, target);

	lua_pop(luaState, 1);

	return ret;
}

#endif
