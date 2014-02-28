
#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include <map>

#include <boost/thread/mutex.hpp>

#include "main.h"

#include "include/cef_app.h"

class Application : public CefApp,
							CefRenderProcessHandler
{
private:
	std::map<int, std::pair<CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> callbackMap;

	boost::mutex callbackMapLock;

public:
	void AddCallbackFunction(int id, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context);

	// CefApp interface
public:
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() { return this; }

	// CefRenderProcessHandler interface
public:
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

	virtual void OnWebKitInitialized();

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info);

	IMPLEMENT_REFCOUNTING(Application)
};

#endif // APPLICATION_H
