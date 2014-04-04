
#include <ctime>

#include "chromium/ChromiumStateLogic.h"
#include "chromium/ClientImpl.h"
#include "osapi/osapi.h"
#include "graphics/2d.h"
#include "sound/sound.h"
#include "io/cursor.h"
#include "graphics/gropengl.h"

#include "freespace.h"

#include "include/cef_app.h"

#include <SDL_syswm.h>

namespace chromium
{
	ChromiumStateLogic::ChromiumStateLogic(const SCP_string& url) : mBrowser(nullptr), mLastUpdate(0)
	{
		mInitialUrl.FromString(url.c_str());

#ifdef USE_FULLSCREEN_BORWSER
		mBrowser = Browser::CreateFullScreenBrowser();
#else
		int width, height;
		SDL_GetWindowSize(os_get_window(), &width, &height);

		mBrowser = Browser::CreateOffScreenBrowser(width, height, true);
#endif
	}

	void ChromiumStateLogic::enterState(GameState oldState)
	{
		if (!mBrowser->Create(mInitialUrl))
		{
			Error(LOCATION, "Failed to initialize browser!");
		}

#ifdef USE_FULLSCREEN_BORWSER
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version); // initialize info structure with SDL version info

		if (!SDL_GetWindowWMInfo(os_get_window(), &wmInfo))
		{
			// call failed
			mprintf(("Couldn't get window information: %s\n", SDL_GetError()));
			return;
		}

		UpdateWindow(wmInfo.info.win.window);
#else
		mBrowser->RegisterEventHandlers();
#endif

		mLastUpdate = 0;
	}

	void ChromiumStateLogic::doFrame()
	{
#ifdef USE_FULLSCREEN_BORWSER
		os_sleep(5);
#else
		io::mouse::CursorManager::get()->doFrame();

		std::clock_t now = std::clock();

		if (mLastUpdate != 0 && ((float)(now - mLastUpdate) / CLOCKS_PER_SEC) <= 0.016666f)
		{
			os_sleep(5);
			return;
		}

		gr_set_color(255, 255, 255);
		gr_clear();

		if (mBrowser)
		{
			if (bm_is_valid(mBrowser->GetClient()->getBrowserBitmap()))
			{
				gr_set_bitmap(mBrowser->GetClient()->getBrowserBitmap(), GR_ALPHABLEND_FILTER);
				gr_bitmap(0, 0, false);
			}

			mBrowser->SetFocused(true);
		}

		gr_flip();

		mLastUpdate = std::clock();
#endif
	}

	void ChromiumStateLogic::leaveState(GameState newState)
	{
#ifndef USE_FULLSCREEN_BORWSER
		mBrowser->RemoveEventHandlers();
#endif

		if (mBrowser)
		{
			mBrowser->Close();
		}
	}
}
