
#ifndef BROWSER_H
#define BROWSER_H
#pragma once

#include "globalincs/pstypes.h"
#include "chromium/ClientImpl.h"

#include <boost/smart_ptr.hpp>

namespace chromium
{
	class Browser
	{
	private:
		CefRefPtr<ClientImpl> mClient;

		Browser();
	public:
		~Browser();

		CefRefPtr<ClientImpl> GetClient() { return mClient; }

		bool Create(const CefString& url);

	public:
		static boost::shared_ptr<Browser> CreateBrowser(size_t width, size_t height);
	};
}

#endif // BROWSER_H
