
#ifndef __LUACALLBACK_H__
#define __LUACALLBACK_H__

#include "globalincs/pstypes.h"
#include "parse/lua/LuaUtil.h"
#include "parse/lua/LuaValue.h"
#include "parse/lua/LuaTable.h"

typedef SCP_vector<LuaValue> LuaValueList;

/**
 * @brief A callback to lua code.
 *
 * Wraps a Lua-function value which can be called as a callback from C++ code into Lua code.
 * Provides ways to set arguments and get return values.
 *
 * @see LuaConvert
 */
class LuaCallback : public LuaValue
{
public:
	/**
	 * @brief Sets the lua_State used by this callback.
	 *
	 * @param L The state to use.
	 */
	LuaCallback(lua_State* L);

	/**
	 * @brief Copy-constructor
	 *
	 * Copies the reference of the other callback and sets it as the reference of this object.
	 *
	 * @param other The other callback.
	 */
	LuaCallback(const LuaCallback& other);

	/**
	 * @brief Compiles a chunck of lua code.
	 *
	 * The supplied code is compiled with the given name set for debug purposes.
	 * The function is then referenced by this object and can be called like any other LuaCallback.
	 *
	 * @param L The lua-state.
	 * @param code The lua-code, can be text and binary.
	 * @param name A string describing the function.
	 *
	 * @exception SCP_string Thrown when the compilation failed. The exception object is the error message.
	 */
	LuaCallback(lua_State* L, const SCP_string& code, const SCP_string& name = "");

	/**
	 * @brief Frees the reference to the function if it exists.
	 */
	~LuaCallback();

	void parseCode(const SCP_string& code, const SCP_string& name = "");

	/**
	 * @brief Sets a C-function
	 *
	 * Sets the given c-function to be used by this callback. This can be used to
	 * have Lua code call a specific C-function.
	 *
	 * @param func The lua_CFunction which will be used.
	 */
	void setCFunction(lua_CFunction func);

	/**
	 * @brief Sets the function environment.
	 *
	 * @param environment The table which will be the new environment of the function.
	 * @return `true` when successful, `false` otherwise
	 */
	bool setEnvironment(const LuaTable& environment);

	/**
	 * @brief Calls the function.
	 *
	 * Pushes the function to the right place on the stack and calls the function. After the method
	 * returns the return value are located on the stack and can be retrieved with getRetnrValue(ValueType&)
	 *
	 * @return The number of returned values or -1 when the function wasn't executed successfully.
	 *
	 * @warning Do not change the stack manually while using this class!
	 */
	LuaValueList call(const LuaValueList& arguments);

	/**
	 * @brief Calls the function. See call().
	 * @return Same as call().
	 */
	LuaValueList operator()(const LuaValueList& arguments);
private:
	bool isCFunction; //!< @c true to indicate that this is a C-function, mainly used for checking the values
};

#endif
