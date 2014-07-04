
#ifndef BROWSER_H
#define BROWSER_H
#pragma once

#include "globalincs/pstypes.h"
#include "chromium/ClientImpl.h"

#include <boost/smart_ptr.hpp>

namespace chromium
{
	class Browser
	{
	private:
		CefRefPtr<ClientImpl> mClient;

		SCP_vector<size_t> mHandlerIdentifiers;

		bool mTransparent;

		bool MouseEvent(const SDL_Event& event);

		bool SystemEvent(const SDL_Event& event);

		void addEventHandler(SDL_EventType type, int weigth,
			const std::function<bool(const SDL_Event&)>& listener);

		void CreateBrowserWindow(HWND parentWindow);

		Browser() : mClient(nullptr)
		{
		}
	public:

		~Browser()
		{
		}

		CefRefPtr<ClientImpl> GetClient() { return mClient; }

		bool Create(const CefString& url);

		void Close();

		void SetFocused(bool focused);

		void RegisterEventHandlers();

		void RemoveEventHandlers();

		void LoadURL(const CefString& string);

		void Resize(size_t width, size_t height);

	public:
		static boost::shared_ptr<Browser> CreateOffScreenBrowser(size_t width, size_t height, bool transparent = true);
	};
}

#endif // BROWSER_H
