
#include <ctime>

#include "chromium/chromium.h"
#include "chromium/jsapi/jsapi.h"
#include "chromium/ApplicationImpl.h"
#include "mainloop/mainloop.h"
#include "io/timer.h"
#include "mod_table/mod_table.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "include/cef_app.h"

#include <boost/filesystem.hpp>

namespace chromium
{
	namespace fs = boost::filesystem;

	std::clock_t lastUpdate = 0;

	bool chromiumInited = false;

	CefRefPtr<ApplicationImpl> application;

	bool doChromiumWork()
	{
		std::clock_t now = std::clock();

		if (lastUpdate == 0 || ((float)(now - lastUpdate) / CLOCKS_PER_SEC) > 0.016666f)
		{
			// Try to limit it to 60 updates per second
			CefDoMessageLoopWork();
			lastUpdate = now;
		}

		return true;
	}

	void init()
	{
		if (!Chromium_enable)
		{
			// Just return here, leaving the chromium system uninited
			return;
		}

		// TODO: implement code which works for other platforms (possible using argc and argv
		// to determine the executable path)
		CefMainArgs main_args(GetModuleHandle(NULL));

		WCHAR moduleName[MAX_PATH];
		DWORD result = GetModuleFileNameW(NULL, moduleName, MAX_PATH);

		if (result == ERROR_INSUFFICIENT_BUFFER)
		{
			mprintf(("Path to executable is too long, errors might occur."));
		}
		
		CefSettings settings;
		CefString(&settings.browser_subprocess_path).FromWString((fs::path(moduleName).parent_path() / CHROMIUM_PROCESS).native());

		settings.multi_threaded_message_loop = false;

		settings.background_color = CefColorSetARGB(255, 255, 255, 255);

		settings.remote_debugging_port = 12345;

		CefString(&settings.log_file).FromWString((fs::current_path() / "data" / "chromium.log").native());
		
		application = new ApplicationImpl();

		CefInitialize(main_args, settings, application.get());

		mainloop::addMainloopFunction(doChromiumWork);

		jsapi::init();

		chromiumInited = true;

		return;
	}

	void shutdown()
	{
		if (chromiumInited)
		{
			CefShutdown();
		}
	}

	void addCallback(const CefString& name)
	{
		if (chromiumInited)
		{
			application->AddCallbackName(name);
		}
	}

	void removeCallback(const CefString& name)
	{
		if (chromiumInited)
		{
			application->RemoveCallback(name);
		}
	}

	bool isInited()
	{
		return chromiumInited;
	}
}
