
#include <ctime>

#include "chromium/chromium.h"
#include "chromium/jsapi/jsapi.h"
#include "chromium/ApplicationImpl.h"

#include "mainloop/mainloop.h"
#include "io/timer.h"
#include "mod_table/mod_table.h"
#include "cmdline/cmdline.h"

#ifdef WIN32
#include <Windows.h>
#endif
#ifdef SCP_UNIX
#include <unistd.h>
#endif

#include "include/cef_app.h"
//#include "include/cef_sandbox_win.h"

#include <boost/filesystem.hpp>

namespace chromium
{
	namespace fs = boost::filesystem;

	clock_t lastUpdate = 0;

	bool chromiumInited = false;

	CefRefPtr<ApplicationImpl> application;

	SCP_vector<boost::weak_ptr<Browser>> knownBrowsers;

	bool doChromiumWork()
	{
		clock_t now = clock();

		if (lastUpdate == 0 || (static_cast<float>(now - lastUpdate) / CLOCKS_PER_SEC) > 0.016666f)
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

		knownBrowsers.clear();
		
		CefSettings settings;
		
		// TODO: implement code which works for other platforms (possible using argc and argv
		// to determine the executable path)
		// See http://stackoverflow.com/a/1024937
#ifdef WIN32
		CefMainArgs main_args(GetModuleHandle(nullptr));

		WCHAR moduleName[MAX_PATH];
		DWORD result = GetModuleFileNameW(nullptr, moduleName, MAX_PATH);

		if (result == ERROR_INSUFFICIENT_BUFFER)
		{
			mprintf(("Path to executable is too long, errors might occur."));
		}
		
		CefString(&settings.browser_subprocess_path).FromWString((fs::path(moduleName).parent_path() / CHROMIUM_PROCESS).wstring());
		settings.windowless_rendering_enabled = true;
		
#elif SCP_UNIX
		CefMainArgs main_args(Cmdline_argc, Cmdline_argv);
		
		char moduleName[MAX_PATH];
		size_t moduleLen = readlink("/proc/self/exe", moduleName, MAX_PATH);
		
		if (moduleLen == MAX_PATH)
		{
			mprintf(("Path to executable is too long, errors might occur."));
		}
		moduleName[moduleLen - 1] = '\0';
		
		CefString(&settings.browser_subprocess_path).FromString((fs::path(moduleName).parent_path() / CHROMIUM_PROCESS).string());
#endif
		settings.multi_threaded_message_loop = false;
		settings.remote_debugging_port = 12345;

		CefString(&settings.log_file).FromWString((fs::current_path() / "data" / "chromium.log").wstring());
		
		application = new ApplicationImpl();

		CefInitialize(main_args, settings, application.get(), nullptr);

		mainloop::addMainloopFunction(doChromiumWork);

		jsapi::init();

		chromiumInited = true;
	}

	void shutdown()
	{
		if (chromiumInited)
		{
			for (auto& browser : knownBrowsers)
			{
				if (!browser.expired())
				{
					browser.lock()->Close();
				}
			}

			knownBrowsers.clear();

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

	void addBrowserInstance(boost::weak_ptr<Browser> browser)
	{
		knownBrowsers.push_back(browser);
	}

	bool isInited()
	{
		return chromiumInited;
	}
}
