
#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "globalincs/pstypes.h" // Included to suppress warnings
#include "include/cef_app.h"

namespace chromium
{
	class ApplicationImpl : public CefApp,
									CefBrowserProcessHandler
	{
	public:
		ApplicationImpl() : CefApp() {}
		
		// CefApp interface
	public:
		virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar);

		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() { return this; }

		// CefBrowserProcessHandler interface
	public:
		virtual void OnContextInitialized();

		IMPLEMENT_REFCOUNTING(ApplicationImpl);
	};
}

#endif // APPLICATION_H
