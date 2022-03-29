#ifndef X_TEST_SERVLET_H
#define X_TEST_SERVLET_H

#include "../XHTTP/XServlet.h"
#include "XServletDefine.h"

class XTestServlet : public XServlet
{
public:
	XTestServlet()
	{
		setPath("/test");
	}

	virtual void doGet(XNETSTRUCT::XRequest &req, XNETSTRUCT::XResponse &res) override;
};

REGISETRSERVLET(XTestServlet)

#endif