
#include "chromium/chromium.h"
#include "chromium/Browser.h"
#include "osapi/osapi.h"

namespace chromium
{
	using namespace boost;

	bool Browser::Create(const CefString& url)
	{
		Assertion(mClient != nullptr, "Can't create browser from default constructed object!");

		CefWindowInfo info;
		info.SetAsOffScreen(reinterpret_cast<HWND>(os_get_window()));
		info.SetTransparentPainting(TRUE);

		CefBrowserSettings settings;
		settings.java = STATE_DISABLED;
		settings.javascript_close_windows = STATE_DISABLED;
		settings.javascript_open_windows = STATE_DISABLED;
		settings.plugins = STATE_DISABLED;

		return CefBrowserHost::CreateBrowser(info, mClient.get(), url, settings, nullptr);
	}

	shared_ptr<Browser> Browser::CreateBrowser(size_t width, size_t height)
	{
		if (!chromium::isInited())
		{
			Warning(LOCATION, "Chromium subsystem is not inited, that either means you haven't enable chromium in your mod table or created a browser too early.");
			return nullptr;
		}

		shared_ptr<Browser> browser = shared_ptr<Browser>(new Browser());

		browser->mClient = new ClientImpl(static_cast<int>(width), static_cast<int>(height));

		return browser;
	}
}
