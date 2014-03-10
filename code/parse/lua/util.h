
#ifndef PARSE_LUA_UTIL_H
#define PARSE_LUA_UTIL_H
#pragma once

#include "chromium/ClientImpl.h"
#include "parse/lua.h"

#include <LuaCpp/LuaTable.hpp>

namespace lua
{
	namespace util
	{
		void executeCallback(CefRefPtr<chromium::ClientImpl> client, const CefString& name, luacpp::LuaTable& table);
	}
}

#endif // PARSE_LUA_ARGS_H
