
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
	ChromiumStateLogic::ChromiumStateLogic(const SCP_string& url) : mBrowser(nullptr)
	{
		mInitialUrl.FromString(url.c_str());

		mBrowser = Browser::CreateFullScreenBrowser();
	}

	void ChromiumStateLogic::enterState(GameState oldState)
	{
		if (!mBrowser->Create(mInitialUrl))
		{
			Error(LOCATION, "Failed to initialize browser!");
		}

		// UGH! HACK: Cef somehow doesn't draw fullscreen windows right at first
		// We first go windowed and then back to fix this...
		if (!Cmdline_window)
		{
			if (!Cmdline_fullscreen_window)
			{
				SDL_SetWindowFullscreen(os_get_window(), 0);
				SDL_SetWindowFullscreen(os_get_window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else
			{
				SDL_SetWindowBordered(os_get_window(), SDL_TRUE);
				SDL_SetWindowBordered(os_get_window(), SDL_FALSE);
			}
		}

		mLastUpdate = 0;
	}

	void ChromiumStateLogic::doFrame()
	{
		io::mouse::CursorManager::get()->doFrame();

		os_sleep(10);
	}

	void ChromiumStateLogic::leaveState(GameState newState)
	{
		if (mBrowser)
		{
			if (mBrowser->GetClient()->getMainBrowser().get())
			{
				mBrowser->GetClient()->getMainBrowser()->GetHost()->CloseBrowser(true);
			}
		}
	}
}
