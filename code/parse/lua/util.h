
#ifndef PARSE_LUA_UTIL_H
#define PARSE_LUA_UTIL_H
#pragma once

#include "chromium/ClientImpl.h"
#include "parse/lua.h"

#include <LuaCpp/LuaTable.hpp>
#include <LuaCpp/LuaFunction.hpp>

namespace lua
{
	namespace util
	{
		void executeCallback(CefRefPtr<chromium::ClientImpl> client, const CefString& name, luacpp::LuaTable& table);

		void addApiFunction(const CefString& name, const luacpp::LuaFunction& function);
	}
}

#endif // PARSE_LUA_ARGS_H
