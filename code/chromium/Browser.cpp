
#include "chromium/chromium.h"
#include "chromium/Browser.h"
#include "osapi/osapi.h"
#include "io/cursor.h"

#include <SDL_keyboard.h>
#include <SDL_syswm.h>

namespace
{
	uint32 GetCefModifiers()
	{
		uint32 state = 0;
		SDL_Keymod modifier = SDL_GetModState();

		if (modifier & KMOD_LSHIFT)
		{
			state |= EVENTFLAG_SHIFT_DOWN | EVENTFLAG_IS_LEFT;
		}
		if (modifier & KMOD_RSHIFT)
		{
			state |= EVENTFLAG_SHIFT_DOWN | EVENTFLAG_IS_RIGHT;
		}

		if (modifier & KMOD_LCTRL)
		{
			state |= EVENTFLAG_CONTROL_DOWN | EVENTFLAG_IS_LEFT;
		}
		if (modifier & KMOD_RCTRL)
		{
			state |= EVENTFLAG_CONTROL_DOWN | EVENTFLAG_IS_RIGHT;
		}

		if (modifier & KMOD_LALT)
		{
			state |= EVENTFLAG_ALT_DOWN | EVENTFLAG_IS_LEFT;
		}
		if (modifier & KMOD_RALT)
		{
			state |= EVENTFLAG_ALT_DOWN | EVENTFLAG_IS_RIGHT;
		}

		if (modifier & KMOD_LGUI)
		{
			state |= EVENTFLAG_COMMAND_DOWN | EVENTFLAG_IS_LEFT;
		}
		if (modifier & KMOD_RGUI)
		{
			state |= EVENTFLAG_COMMAND_DOWN | EVENTFLAG_IS_RIGHT;
		}

		if (modifier & KMOD_NUM)
		{
			state |= EVENTFLAG_NUM_LOCK_ON;
		}

		if (modifier & KMOD_CAPS)
		{
			state |= EVENTFLAG_CAPS_LOCK_ON;
		}

		return state;
	}

	bool isKeyDown(WPARAM wparam)
	{
		return (GetKeyState(wparam) & 0x8000) != 0;
	}

	int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam) {
		int modifiers = 0;
		if (isKeyDown(VK_SHIFT))
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		if (isKeyDown(VK_CONTROL))
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		if (isKeyDown(VK_MENU))
			modifiers |= EVENTFLAG_ALT_DOWN;

		// Low bit set from GetKeyState indicates "toggled".
		if (::GetKeyState(VK_NUMLOCK) & 1)
			modifiers |= EVENTFLAG_NUM_LOCK_ON;
		if (::GetKeyState(VK_CAPITAL) & 1)
			modifiers |= EVENTFLAG_CAPS_LOCK_ON;

		switch (wparam) {
		case VK_RETURN:
			if ((lparam >> 16) & KF_EXTENDED)
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			if (!((lparam >> 16) & KF_EXTENDED))
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_NUMLOCK:
		case VK_NUMPAD0:
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
		case VK_DIVIDE:
		case VK_MULTIPLY:
		case VK_SUBTRACT:
		case VK_ADD:
		case VK_DECIMAL:
		case VK_CLEAR:
			modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_SHIFT:
			if (isKeyDown(VK_LSHIFT))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (isKeyDown(VK_RSHIFT))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_CONTROL:
			if (isKeyDown(VK_LCONTROL))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (isKeyDown(VK_RCONTROL))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_MENU:
			if (isKeyDown(VK_LMENU))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (isKeyDown(VK_RMENU))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_LWIN:
			modifiers |= EVENTFLAG_IS_LEFT;
			break;
		case VK_RWIN:
			modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		}
		return modifiers;
	}
}

namespace chromium
{
	using namespace boost;

	bool Browser::MouseEvent(const SDL_Event& event)
	{
		if (mClient->getMainBrowser() == nullptr)
		{
			return false;
		}

		if (!io::mouse::CursorManager::get()->isCursorShown())
		{
			// Mouse is not shown, don't report mouse events
			return false;
		}

		if (!mClient->isFocused())
		{
			// Special handling for not focused, may need to be changed to be more flexible
			if (event.type != SDL_MOUSEMOTION)
			{
				// Report mouse motion but nothing else
				return false;
			}
		}

		auto browser = mClient->getMainBrowser();
		auto host = browser->GetHost();

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			CefMouseEvent mouseEvent;
			mouseEvent.x = event.button.x;
			mouseEvent.y = event.button.y;
			mouseEvent.modifiers = GetCefModifiers();

			CefBrowserHost::MouseButtonType type;

			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				type = MBT_LEFT;
				break;
			case SDL_BUTTON_RIGHT:
				type = MBT_RIGHT;
				break;
			case SDL_BUTTON_MIDDLE:
				type = MBT_MIDDLE;
				break;
			default:
				return true;
			}

			host->SendMouseClickEvent(mouseEvent, type, event.button.state == SDL_RELEASED, 1);

			break;
		}
		case SDL_MOUSEMOTION:
		{
			CefMouseEvent mouseEvent;
			mouseEvent.x = event.motion.x;
			mouseEvent.y = event.motion.y;
			mouseEvent.modifiers = GetCefModifiers();

			host->SendMouseMoveEvent(mouseEvent, false);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			CefMouseEvent mouseEvent;

			int mouseX;
			int mouseY;

			SDL_GetMouseState(&mouseX, &mouseY);

			mouseEvent.x = mouseX;
			mouseEvent.y = mouseY;
			mouseEvent.modifiers = GetCefModifiers();

			// Multiplying it with 120 as that works for windows, needs testing on other platforms
			host->SendMouseWheelEvent(mouseEvent, event.wheel.x * 120, event.wheel.y * 120);
			break;
		}

		default:
			break;
		}

		return true;
	}

	bool Browser::SystemEvent(const SDL_Event& event)
	{
		if (!mClient->isFocused())
		{
			return false;
		}

		auto message = event.syswm.msg->msg.win.msg;
		auto wParam = event.syswm.msg->msg.win.wParam;
		auto lParam = event.syswm.msg->msg.win.lParam;

		switch (message)
		{
		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			{
				CefKeyEvent keyEvent;
				keyEvent.windows_key_code = wParam;
				keyEvent.native_key_code = lParam;
				keyEvent.is_system_key = message == WM_SYSCHAR || message == WM_SYSKEYDOWN || message == WM_SYSKEYUP;

				if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
				{
					keyEvent.type = KEYEVENT_RAWKEYDOWN;
				}
				else if (message == WM_KEYUP || message == WM_SYSKEYUP)
				{
					keyEvent.type = KEYEVENT_KEYUP;
				}
				else
				{
					keyEvent.type = KEYEVENT_CHAR;
				}

				keyEvent.modifiers = GetCefKeyboardModifiers(wParam, lParam);

				if (mClient->getMainBrowser().get())
				{
					mClient->getMainBrowser()->GetHost()->SendKeyEvent(keyEvent);
				}
				
				// We handled the event...
				return true;
			}
		}

		return false;
	}

	void Browser::addEventHandler(SDL_EventType type, int weigth,
		const std::function<bool(const SDL_Event&)>& listener)
	{
		mHandlerIdentifiers.push_back(os::addEventListener(type, weigth, listener));
	}

	bool Browser::Create(const CefString& url)
	{
		Assertion(mClient != nullptr, "Can't create browser from default constructed object!");

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version); // initialize info structure with SDL version info

		if (!SDL_GetWindowWMInfo(os_get_window(), &wmInfo))
		{
			// call failed
			mprintf(("Couldn't get window information: %s\n", SDL_GetError()));
			return false;
		}

		CefWindowInfo info;
		info.SetAsOffScreen(wmInfo.info.win.window);
		info.SetTransparentPainting(TRUE);

		CefBrowserSettings settings;
		settings.java = STATE_DISABLED;
		settings.javascript_close_windows = STATE_DISABLED;
		settings.javascript_open_windows = STATE_DISABLED;
		settings.plugins = STATE_DISABLED;
		CefString(&settings.default_encoding).FromASCII("UTF-8");

		return CefBrowserHost::CreateBrowser(info, mClient.get(), url, settings, nullptr);
	}

	shared_ptr<Browser> Browser::CreateBrowser(size_t width, size_t height)
	{
		if (!chromium::isInited())
		{
			Warning(LOCATION, "Chromium subsystem is not inited, that either means you haven't enabled chromium in your mod table or created a browser too early.");
			return nullptr;
		}

		shared_ptr<Browser> browser = shared_ptr<Browser>(new Browser());

		browser->mClient = new ClientImpl(static_cast<int>(width), static_cast<int>(height));

		return browser;
	}

	void Browser::SetFocused(bool focused)
	{
		mClient->setFocused(focused);
	}

	void Browser::RegisterEventHandlers()
	{
		auto mouseHandler = std::bind(&Browser::MouseEvent, this, std::placeholders::_1);
		auto systemHandler = std::bind(&Browser::SystemEvent, this, std::placeholders::_1);

		addEventHandler(SDL_MOUSEBUTTONDOWN, os::DEFAULT_LISTENER_WEIGHT - 1, mouseHandler);
		addEventHandler(SDL_MOUSEBUTTONUP, os::DEFAULT_LISTENER_WEIGHT - 1, mouseHandler);

		addEventHandler(SDL_MOUSEMOTION, os::DEFAULT_LISTENER_WEIGHT - 1, mouseHandler);

		addEventHandler(SDL_MOUSEWHEEL, os::DEFAULT_LISTENER_WEIGHT - 1, mouseHandler);

		addEventHandler(SDL_SYSWMEVENT, os::DEFAULT_LISTENER_WEIGHT - 1, systemHandler);

		// Block keyboard input as we use system events for this
		addEventHandler(SDL_KEYDOWN, os::DEFAULT_LISTENER_WEIGHT - 1, [&](const SDL_Event& event) { return mClient->isFocused(); });
		addEventHandler(SDL_KEYUP, os::DEFAULT_LISTENER_WEIGHT - 1, [&](const SDL_Event& event) { return mClient->isFocused(); });

		addEventHandler(SDL_WINDOWEVENT, os::DEFAULT_LISTENER_WEIGHT - 1, [&](const SDL_Event& event)
		{
			auto browser = mClient->getMainBrowser();
			auto host = browser->GetHost();

			if (event.window.windowID == SDL_GetWindowID(os_get_window())) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_LEAVE:
					host->SendCaptureLostEvent();
					return true;
				}
			}

			return false;
		});
	}

	void Browser::RemoveEventHandlers()
	{
		for (auto ident : mHandlerIdentifiers)
		{
			os::removeEventListener(ident);
		}
	}
}
