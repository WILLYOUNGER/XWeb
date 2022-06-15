#ifndef X_WEB_SERVER_H
#define X_WEB_SERVER_H

#include "./XNet/XBaseNet.h"
#include "./XStruct/XStruct.h"
#include "./XPthread/XPthreadPool.h"
#include "./XHTTP/XHttp.h"

using namespace XNETSTRUCT;
class XWebServer
{
public:
	XWebServer();
private:
	void ConnectCallback(XNETSTRUCT::XSocket socket);

	void CloseCallback(void);

	void ReadCallback(XNETSTRUCT::XMsgPtr msg);

	bool WriteCallback(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket socket);
private:
	XNETBASE::XServer* m_web_server;

	XPthreadPool<XNETSTRUCT::XMsgPtr, XHttp>* pool;
public:
	static std::map<XNETSTRUCT::XSocket, std::list<XResponse>*> m_reply;
};

#endif