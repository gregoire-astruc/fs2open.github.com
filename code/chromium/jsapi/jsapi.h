
#ifndef JSAPI_H
#define JSAPI_H
#pragma once

/*
 * This file is used by both the FSO and chromium executable so be careful about what you include here
 */

#include <functional>

#ifdef BUILDING_CHROMIUMPROCESS
#include "main.h"
#else
#include "globalincs/pstypes.h"
#endif

#include "include/cef_app.h"
#include "include/cef_v8.h"

namespace chromium
{
	namespace jsapi
	{
		const char* const API_MESSAGE_NAME = "fso_jsapi";

		const char* const CALLBACK_MESSAGE_NAME = "fso_callback";

#ifdef BUILDING_CHROMIUMPROCESS
		typedef std::function<bool(const CefString&, CefRefPtr<CefV8Value>, CefString&)> FunctionType;

		void addUnvalidatedFunction(const CefString& name);

		bool validateQuery(const CefString& name, CefRefPtr<CefV8Value> argument, CefString& exception);
#else
		typedef std::function<bool(const CefString&, CefRefPtr<CefListValue>, int, CefRefPtr<CefListValue>)> FunctionType;

		void addFunction(const CefString& name, const FunctionType& apiFunction);

		void addFunction(const CefString& name);

		bool processRendererMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message);
#endif

		void init();
	}
}

#endif // JSAPI_H
