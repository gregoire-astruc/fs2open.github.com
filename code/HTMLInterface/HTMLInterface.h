
#ifndef _HTML_INTERFACE_H
#define _HTML_INTERFACE_H

#include "globalincs/pstypes.h"
#include "HTMLInterface/OpenGLSurface.h"
#include "HTMLInterface/HTMLWidget.h"

#include <boost/smart_ptr.hpp>
#include <Awesomium/WebCore.h>

class RestrictingResourceInterceptor;

class HTMLInterface
{
private:
	Awesomium::WebCore* webCore;

	boost::shared_ptr<OpenGLSurfaceFactory> openglFactory;

	boost::shared_ptr<RestrictingResourceInterceptor> interceptor;

	SCP_vector<boost::shared_ptr<HTMLWidget>> widgets;

	uint nextWidgetID;
public:
	HTMLInterface();
	~HTMLInterface();

	void update();

	boost::weak_ptr<HTMLWidget> createDisplay(int width, int height);
	bool removeWidget(uint widgetID);
};

#endif // _HTML_INTERFACE_H