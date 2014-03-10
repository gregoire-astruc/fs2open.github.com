
#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include "main.h"

#include "include/cef_app.h"

template<class T>
class IDProvider
{
private:
	T mNextId;

	boost::mutex mIdLock;

public:
	IDProvider(const T& nextId = 0) : mNextId(nextId) {}

	T getAndIncrement()
	{
		boost::lock_guard<boost::mutex> guard(mIdLock);

		return mNextId++;
	}
};

class Application : public CefApp,
							CefRenderProcessHandler
{
private:
	std::map<int, std::pair<CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> mApiCallbackMap;
	boost::mutex mApiCallbackMapLock;

	std::vector<CefString> mApplicationCallbacks;
	std::map<std::pair<int, CefString>, std::pair<CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> mApplicationCallbackMap;
	boost::mutex mApplicationCallbackMapLock;

	IDProvider<int> mApplicationCallbackIdProvider;

public:
	IDProvider<int> mApiIdProvider;

	Application();

	void AddAPICallbackFunction(int id, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context);

	int AddApplicationCallback(const CefString& name, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context);

	bool RemoveApplicationCallback(int id);

	void ExecuteCallback(const CefString& callbackName, CefRefPtr<CefListValue> argList, int argListIndex);

	bool hasCallback(const CefString& callbackName)
	{
		return std::find(mApplicationCallbacks.begin(), mApplicationCallbacks.end(), callbackName) != mApplicationCallbacks.end();
	}

	// CefApp interface
public:
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() { return this; }

	// CefRenderProcessHandler interface
public:
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

	virtual void OnWebKitInitialized();

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;

	IMPLEMENT_REFCOUNTING(Application)
};

#endif // APPLICATION_H
