
#include "globalincs/pstypes.h"

#include <include/cef_app.h>

namespace chromium
{
	void init();

	void addCallback(const CefString& name);

	void removeCallback(const CefString& name);

	void shutdown();
}
