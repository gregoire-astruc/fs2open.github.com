
#include "bmpman/bmpman.h"
#include "chromium/ClientImpl.h"
#include "chromium/jsapi/jsapi.h"
#include "graphics/2d.h"

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

	void ClientImpl::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
	{
		// For future usage (probably lua <-> javascript bindings)
	}

	void ClientImpl::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		Assertion(mainBrowser.get() == nullptr, "Sub-browsers are not supported!");

		mainBrowser = browser;

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
		Assertion(this->width == width, "Paint event width is not the same as the set width!");
		Assertion(this->height == height, "Paint event height is not the same as the set height!");

		memcpy(bitmapData, buffer, width * height * 4);
		gr_update_texture(browserBitmapHandle, 32, reinterpret_cast<ubyte*>(bitmapData), width, height);
	}
}