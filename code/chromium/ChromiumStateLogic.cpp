
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

		int width, height;
		SDL_GetWindowSize(os_get_window(), &width, &height);

		mBrowser = Browser::CreateOffScreenBrowser(width, height, false);
	}

	void ChromiumStateLogic::enterState(GameState oldState)
	{
		if (!mBrowser->Create(mInitialUrl))
		{
			Error(LOCATION, "Failed to initialize browser!");
		}

		mBrowser->RegisterEventHandlers();

		mLastUpdate = 0;
	}

	void ChromiumStateLogic::doFrame()
	{
		io::mouse::CursorManager::doFrame();

		clock_t now = clock();

		if (mLastUpdate != 0 && (static_cast<float>(now - mLastUpdate) / CLOCKS_PER_SEC) <= 1.0f / 60.0f)
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

		mLastUpdate = clock();
	}

	void ChromiumStateLogic::leaveState(GameState newState)
	{
		mBrowser->RemoveEventHandlers();

		if (mBrowser)
		{
			mBrowser->Close();
		}
	}
}
