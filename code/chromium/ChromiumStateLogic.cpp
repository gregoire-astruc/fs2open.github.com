
#include <ctime>

#include "chromium/ChromiumStateLogic.h"
#include "chromium/ClientImpl.h"
#include "osapi/osapi.h"
#include "graphics/2d.h"
#include "sound/sound.h"

#include "freespace.h"

#include "include/cef_app.h"

void game_maybe_draw_mouse(float frametime);
namespace chromium
{
	ChromiumStateLogic::ChromiumStateLogic(const SCP_string& url) : clientPtr(nullptr)
	{
		initialUrl.FromString(url.c_str());
		clientPtr = new ClientImpl(gr_screen.max_w, gr_screen.max_h);

		clientPtr->addJavascriptCallback("test");
	}

	void ChromiumStateLogic::enterState(GameState oldState)
	{
		CefWindowInfo info;
		info.SetAsOffScreen((HWND) os_get_window());
		info.SetTransparentPainting(true);

		CefBrowserSettings settings;
		settings.java = STATE_DISABLED;
		settings.javascript_close_windows = STATE_DISABLED;
		settings.javascript_open_windows = STATE_DISABLED;
		settings.plugins = STATE_DISABLED;

		bool result = CefBrowserHost::CreateBrowser(info, clientPtr.get(), initialUrl, settings, nullptr);

		if (!result)
		{
			Error(LOCATION, "Failed to create browser!");
		}

		lastUpdate = 0;
	}

	void ChromiumStateLogic::doFrame()
	{
		game_maybe_draw_mouse(flFrametime);

		std::clock_t now = std::clock();

		if (lastUpdate != 0 && ((float)(now - lastUpdate) / CLOCKS_PER_SEC) <= 0.016666f)
		{
			Sleep(5);
			return;
		}
		
		gr_set_color(255, 255, 255);
		gr_clear();

		if (bm_is_valid(clientPtr->getBrowserBitmap()))
		{
			gr_set_bitmap(clientPtr->getBrowserBitmap(), GR_ALPHABLEND_FILTER);
			gr_bitmap(0, 0, false);
		}

		gr_flip();

		lastUpdate = std::clock();
	}

	void ChromiumStateLogic::leaveState(GameState newState)
	{
		if (clientPtr.get() && clientPtr->getMainBrowser().get())
		{
			clientPtr->getMainBrowser()->GetHost()->CloseBrowser(true);
		}
	}
}
