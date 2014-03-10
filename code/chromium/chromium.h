
#include "globalincs/pstypes.h"

#include "chromium/jsapi/jsapi.h"

#include <include/cef_app.h>

namespace chromium
{
	void init();

	void addCallback(const CefString& name);

	void removeCallback(const CefString& name);

	void addAPIFunction(const CefString& name, const jsapi::FunctionType& function);

	void removeAPIFunction(const CefString& name);

	void shutdown();
}
