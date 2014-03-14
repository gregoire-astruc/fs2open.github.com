
#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include <map>

#include "main.h"

#include "include/cef_app.h"

template<class T>
class IDProvider
{
private:
	T mNextId;

public:
	IDProvider(const T& nextId = 0) : mNextId(nextId) {}

	T getAndIncrement()
	{
		AutoLock guard(this);

		return mNextId++;
	}

	IMPLEMENT_LOCKING(IDProvider);
};

class Application : public CefApp,
							CefRenderProcessHandler
{
private:
	std::map<int, std::pair<CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> mApiCallbackMap;

	std::vector<CefString> mApplicationCallbacks;
	std::map<std::pair<int, CefString>, std::pair<CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> mApplicationCallbackMap;

	std::vector<CefString> mAPIFunctions;

	IDProvider<int> mApplicationCallbackIdProvider;

public:
	IDProvider<int> mApiIdProvider;

	Application();

	void AddAPICallbackFunction(int id, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context);

	int AddApplicationCallback(const CefString& name, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context);

	bool RemoveApplicationCallback(int id);

	void ExecuteCallback(const CefString& callbackName, CefRefPtr<CefListValue> argList, int argListIndex);

	bool HasCallback(const CefString& callbackName);

	bool HasAPIFunction(const CefString& callbackName);

	// CefApp interface
public:
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() { return this; }

	// CefRenderProcessHandler interface
public:
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

	virtual void OnWebKitInitialized();

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;

	IMPLEMENT_REFCOUNTING(Application);

	IMPLEMENT_LOCKING(Application);
};

#endif // APPLICATION_H
