
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

		mBrowser = Browser::CreateFullScreenBrowser();
	}

	void ChromiumStateLogic::enterState(GameState oldState)
	{
		if (!mBrowser->Create(mInitialUrl))
		{
			Error(LOCATION, "Failed to initialize browser!");
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
