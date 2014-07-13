
#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "globalincs/pstypes.h" // Included to suppress warnings
#include "chromium/jsapi/jsapi.h"

#include "include/cef_app.h"

#include <boost/thread/mutex.hpp>

namespace chromium
{
	bool validateURL(const CefString& url);

	class ApplicationImpl : public CefApp,
									CefBrowserProcessHandler
	{
	private:
		SCP_vector<CefString> mCallbackNames;
		boost::mutex mCallbacknamesLock;

	public:
		ApplicationImpl() : CefApp() {}
		
		void AddCallbackName(const CefString& name);

		void RemoveCallback(const CefString& name);

		// CefApp interface
	public:
		virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override;

		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }

		virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) override;

		// CefBrowserProcessHandler interface
	public:
		virtual void OnContextInitialized() override;

		IMPLEMENT_REFCOUNTING(ApplicationImpl);
	};
}

#endif // APPLICATION_H
