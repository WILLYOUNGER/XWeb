#include "XTestServlet.h"
#include "XHttpStruct.h"
#include <iostream>

using namespace XNETSTRUCT;
using namespace std;

void XTestServlet::doGet(XRequest &req, XResponse &res)
{
	cout << "mothed:" << req.getMethod() << endl;
	cout << "path:" << req.getPath() << endl;
	cout << "version:" << req.getVersion() << endl;
	cout << "key-value Num:" << req.getAttributeNum() << endl;
	cout << "deal a event !" << endl;
}