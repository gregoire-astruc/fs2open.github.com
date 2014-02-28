
#ifndef CLIENTIMPL_H
#define CLIENTIMPL_H

#include "globalincs/pstypes.h"

#include "include/cef_client.h"

namespace chromium
{
	class ClientImpl : public CefClient,
							CefLifeSpanHandler,
							CefRenderHandler,
							CefBrowserProcessHandler
	{
	private:
		CefRefPtr<CefBrowser> mainBrowser;

		int browserBitmapHandle;

		int width;
		int height;

		void* bitmapData;

	public:
		ClientImpl(int widthIn, int heightIn);

		~ClientImpl();

		CefRefPtr<CefBrowser> getMainBrowser() { return mainBrowser; }

		int getBrowserBitmap() { return browserBitmapHandle; }

		// CefClient interface
	public:
		virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return this; }

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }

		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() { return this; }

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

		// CefBrowserProcessHandler interface
	public:
		virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info);

		// CefLifeSpanHandler interface
	public:
		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		// CefRenderHandler interface
	public:
		bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

		void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
			const RectList &dirtyRects, const void *buffer, int width, int height) override;


		IMPLEMENT_REFCOUNTING(ClientImpl);
	};
}

#endif // CLIENTIMPL_H
