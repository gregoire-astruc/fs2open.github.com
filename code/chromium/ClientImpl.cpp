
#include "bmpman/bmpman.h"
#include "chromium/ClientImpl.h"
#include "chromium/jsapi/jsapi.h"
#include "graphics/2d.h"
#include "osapi/osapi.h"

namespace chromium
{
	ClientImpl::ClientImpl(int widthIn, int heightIn)
		: width(widthIn), height(heightIn), bitmapData(NULL), browserBitmapHandle(-1)
	{
	}

	ClientImpl::~ClientImpl()
	{
		if (browserBitmapHandle >= 0)
		{
			bm_release(browserBitmapHandle);
			browserBitmapHandle = -1;
		}

		if (bitmapData != NULL)
		{
			vm_free(bitmapData);
			bitmapData = NULL;
		}
	}

	void ClientImpl::executeCallback(CefString const& callbackName, CefRefPtr<CefListValue> values)
	{
		CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(jsapi::CALLBACK_MESSAGE_NAME);

		CefRefPtr<CefListValue> argumentList = message->GetArgumentList();

		argumentList->SetString(0, callbackName);
		argumentList->SetList(1, values);

		mainBrowser->SendProcessMessage(PID_RENDERER, message);
	}

	void ClientImpl::executeCallback(CefString const& callbackName, CefRefPtr<CefDictionaryValue> values)
	{
		CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(jsapi::CALLBACK_MESSAGE_NAME);

		CefRefPtr<CefListValue> argumentList = message->GetArgumentList();

		argumentList->SetString(0, callbackName);
		argumentList->SetDictionary(1, values);

		mainBrowser->SendProcessMessage(PID_RENDERER, message);
	}

	bool ClientImpl::forceClose()
	{
		if (mainBrowser == nullptr)
		{
			return false;
		}

		mainBrowser->GetHost()->CloseBrowser(true);

		return false;
	}

	void ClientImpl::setFocused(bool focused)
	{
		mFocused = focused;

		if (mainBrowser != nullptr)
		{
			mainBrowser->GetHost()->SendFocusEvent(mFocused);
		}
	}

	bool ClientImpl::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
	{
		if (mainBrowser == nullptr)
		{
			return false;
		}

		if (!mainBrowser->IsSame(browser))
		{
			return false;
		}

		if (message->GetName() == jsapi::API_MESSAGE_NAME)
		{
			jsapi::processRendererMessage(browser, message);

			return true;
		}

		return false;
	}

	void ClientImpl::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		Assertion(mainBrowser.get() == nullptr, "Sub-browsers are not supported!");

		mainBrowser = browser;

		mainBrowser->GetHost()->SetFocus(mFocused);

		// 32-bit per pixel ==> 4 Bytes for each pixel
		bitmapData = vm_malloc(width * height * 4);
		memset(bitmapData, 0, width * height * 4);

		browserBitmapHandle = bm_create(32, width, height, bitmapData, BMP_TEX_XPARENT);
	}

	void ClientImpl::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
		mainBrowser = nullptr;
	}

	bool ClientImpl::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
	{
		rect.width = width;
		rect.height = height;

		return true;
	}

	void ClientImpl::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
		const RectList &dirtyRects, const void *buffer, int width, int height)
	{
		if (type == PET_VIEW)
		{
			Assertion(this->width == width, "Paint event width is not the same as the set width!");
			Assertion(this->height == height, "Paint event height is not the same as the set height!");

			gr_update_texture(browserBitmapHandle, 32, buffer, width, height);
		}
		else if (type == PET_POPUP && mPopupRect.width > 0 && mPopupRect.height > 0)
		{
			gr_update_texture(browserBitmapHandle, 32, buffer, width, height, mPopupRect.x, mPopupRect.y);
		}
	}

	void ClientImpl::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
	{
		if (!show)
		{
			// Clear the popup rectangle.
			ClearPopupRects();
		}
	}

	void ClientImpl::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
	{
		if (rect.width <= 0 || rect.height <= 0)
			return;

		mOriginalPopupRect = rect;
		mPopupRect = GetPopupRectInWebView(mOriginalPopupRect);
	}

	CefRect ClientImpl::GetPopupRectInWebView(const CefRect& original_rect)
	{
		CefRect rc(original_rect);
		// if x or y are negative, move them to 0.
		if (rc.x < 0)
			rc.x = 0;
		if (rc.y < 0)
			rc.y = 0;
		// if popup goes outside the view, try to reposition origin
		if (rc.x + rc.width > width)
			rc.x = width - rc.width;
		if (rc.y + rc.height > height)
			rc.y = height - rc.height;
		// if x or y became negative, move them to 0 again.
		if (rc.x < 0)
			rc.x = 0;
		if (rc.y < 0)
			rc.y = 0;
		return rc;
	}

	void ClientImpl::ClearPopupRects()
	{
		mPopupRect.Set(0, 0, 0, 0);
		mOriginalPopupRect.Set(0, 0, 0, 0);
	}
}