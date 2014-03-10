
#ifndef JSAPI_H
#define JSAPI_H
#pragma once

/*
 * This file is used by both the FSO and chromium executable so be careful about what you include here
 */

#ifndef BUILDING_CHROMIUMPROCESS

#include <functional>

#include "globalincs/pstypes.h"

#include "include/cef_app.h"
#include "include/cef_v8.h"

#endif

namespace chromium
{
	namespace jsapi
	{
		const char* const API_MESSAGE_NAME = "fso_jsapi";

		const char* const CALLBACK_MESSAGE_NAME = "fso_callback";

#ifndef BUILDING_CHROMIUMPROCESS
		typedef std::function<bool(const CefString&, CefRefPtr<CefListValue>, int, CefRefPtr<CefListValue>)> FunctionType;

		void addFunction(const CefString& name, const FunctionType& apiFunction);

		void removeFunction(const CefString& name);

		void getFunctionNames(SCP_vector<CefString>& outVec);

		bool processRendererMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message);

		void init();
#endif
	}
}

#endif // JSAPI_H
