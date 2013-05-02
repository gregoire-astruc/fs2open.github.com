#ifndef __LUAUTIL_H__
#define __LUAUTIL_H__

#include "parse/lua.h"

/**
 * @brief A lua-value reference.
 *
 * Wraps a reference to a lua-value and provides a way to handle multiple users of that reference and
 * automatic reference freeing.
 */
class LuaReference
{
public:
	/**
	 * @brief Creates a lua-reference.
	 *
	 * Copies the value at @c position and creates a reference to it. Also replaces
	 * the value at that position with the copied value.
	 *
	 * @param state The state to create the reference in.
	 * @param position The stack position of the value, defaults to the top of the stack (-1).
	 * @return The LuaReference instance which got created.
	 */
	static LuaReference create(lua_State* state, int position = -1);

	/**
	 * @brief Default constructor, do not use directly.
	 */
	LuaReference() :
			luaState(NULL), reference(-1)
	{
	}

	/**
	 * @brief Initializes a lua reference.
	 *
	 * Sets the lua_State which will be used to hold the reference and the actual reference value.
	 *
	 * @param state The lua_State where the reference points to a value.
	 * @param reference The reference value, should be >= 0.
	 */
	LuaReference(lua_State* state, int reference);

	/**
	 * @brief Copy-constructor
	 *
	 * Copies the value of the other reference and creates a new reference of it.
	 *
	 * @param other The other reference.
	 */
	LuaReference(const LuaReference& other);

	/**
	 * @brief Releases the lue reference.
	 */
	~LuaReference();

	/**
	 * @brief Gets the actual reference number.
	 * @return The reference number
	 */
	int getReference() const;

	/**
	 * @brief Checks if the reference is valid.
	 * @return @c true when valid, @c false otherwise.
	 */
	bool isValid() const;

	/**
	 * @brief Removes the Lua reference
	 * @return @c true when the reference was removed @c false otherwise
	 */
	bool removeReference();

	void setKeepReference(bool keep);

	/**
	 * @brief Pushes the referenced value onto the stack.
	 */
	void pushValue() const;
private:
	LuaReference(lua_State* state);

	lua_State* luaState;
	int reference;

	bool keepReference;
};

/**
 * @brief Contains functions to convert C++ values to and from lua values.
 *
 * Currently the following types are supported:
 *   - `double`
 *   - `float`
 *   - `int`
 *   - `SCP_string`
 *   - `const char*`
 *   - `bool`
 *   - `fix`
 *   - `ade_odata` (A userdata value, created by ade_obj.Set or ade_obj.Get)
 *   - `lua_CFunction`
 *   - `LuaTable`
 *   - `LuaCallback`
 *   - `LuaValue` (this will reference any value at the specified poition)
 */
namespace LuaConvert
{
	/**
	 * @brief Pushes the lua value of the given @c value onto the lua stack.
	 * @param luaState The lua_State to push the values to
	 * @param value The value which should be pushed.
	 */
	template<class ValueType>
	void pushValue(lua_State* luaState, const ValueType value);

	/**
	 * @brief Pops a value from the lua stack and stors it.
	 *
	 * This function checks if the topmost value on the lua stack is of the right type and if
	 * that is the case pops this value from the stack and stors the value inside @c target.
	 *
	 * @param luaState The lua_State which should be checked
	 * @param target The location where the value should be stored
	 * @return @c true when the value could successfully be converted,
	 * 		@c false if the topmost value is not of the right type.
	 */
	template<class ValueType>
	bool popValue(lua_State* luaState, ValueType& target, int stackPos = -1,
			bool remove = true);
}

#endif
