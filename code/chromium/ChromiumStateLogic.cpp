
#include <ctime>

#include "chromium/ChromiumStateLogic.h"
#include "chromium/ClientImpl.h"
#include "osapi/osapi.h"
#include "graphics/2d.h"
#include "sound/sound.h"
#include "io/cursor.h"

#include "freespace.h"

#include "include/cef_app.h"

namespace chromium
{
	ChromiumStateLogic::ChromiumStateLogic(const SCP_string& url) : mBrowser(nullptr)
	{
		mInitialUrl.FromString(url.c_str());

		mBrowser = Browser::CreateBrowser(gr_screen.max_w, gr_screen.max_h);
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
		}

		gr_flip();

		mLastUpdate = std::clock();
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
