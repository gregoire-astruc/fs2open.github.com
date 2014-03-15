
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
		bool mPaintingPopup;

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

		bool isFocused() { return mFocused; }

		// CefClient interface
	public:
		virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

		// CefLifeSpanHandler interface
	public:
		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
			const CefString& target_url, const CefString& target_frame_name, const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings,
			bool* no_javascript_access) override;

		// CefRenderHandler interface
	public:
		bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

		bool ClientImpl::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

		void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
			const RectList &dirtyRects, const void *buffer, int width, int height) override;

		void ActualPaint(PaintElementType type,
			const RectList &dirtyRects, const void *buffer, int width, int height);

		void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;

		bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY,
			int& screenX, int& screenY) override;

		void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;

		CefRect GetPopupRectInWebView(const CefRect& original_rect);

		void ClearPopupRects();

		IMPLEMENT_REFCOUNTING(ClientImpl);
	};
}

#endif // CLIENTIMPL_H
