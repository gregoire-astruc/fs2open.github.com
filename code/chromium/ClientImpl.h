
#ifndef CLIENTIMPL_H
#define CLIENTIMPL_H

#include "globalincs/pstypes.h"

#include "include/cef_client.h"
#include "include/cef_browser_process_handler.h"

namespace chromium
{
	class ClientImpl : public CefClient,
							CefLifeSpanHandler,
							CefRenderHandler
	{
	private:
		CefRefPtr<CefBrowser> mainBrowser;

		int browserBitmapHandle;

		int width;
		int height;

		bool mFocused;

		void* bitmapData;

		CefRect mPopupRect;
		CefRect mOriginalPopupRect;

	public:
		ClientImpl(int widthIn, int heightIn);

		~ClientImpl();

		CefRefPtr<CefBrowser> getMainBrowser() { return mainBrowser; }

		int getBrowserBitmap() { return browserBitmapHandle; }

		void executeCallback(const CefString& callbackName, CefRefPtr<CefListValue> values);

		void executeCallback(const CefString& callbackName, CefRefPtr<CefDictionaryValue> values);

		bool forceClose();

		void setFocused(bool focused);

		// CefClient interface
	public:
		virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return this; }

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

		// CefLifeSpanHandler interface
	public:
		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		// CefRenderHandler interface
	public:
		bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

		void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
			const RectList &dirtyRects, const void *buffer, int width, int height) override;

		void ClientImpl::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;

		void ClientImpl::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;

		CefRect ClientImpl::GetPopupRectInWebView(const CefRect& original_rect);

		void ClientImpl::ClearPopupRects();

		IMPLEMENT_REFCOUNTING(ClientImpl);
	};
}

#endif // CLIENTIMPL_H
