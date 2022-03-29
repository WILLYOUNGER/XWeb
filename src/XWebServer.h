#ifndef X_WEB_SERVER_H
#define X_WEB_SERVER_H

#include "./XNet/XBaseNet.h"
#include "./XStruct/XStruct.h"
#include "./XPthread/XPthreadPool.h"
#include "./XHTTP/XHttp.h"

class XWebServer
{
public:
	XWebServer();
private:
	void ConnectCallback(XNETSTRUCT::XSocket socket);

	void CloseCallback(void);

	void ReadCallback(XNETSTRUCT::XMsgPtr msg);

	void WriteCallback(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket socket);
private:
	XNETBASE::XServer* m_web_server;

	XPthreadPool<XNETSTRUCT::XMsgPtr, XHttp>* pool;
public:
	static std::map<XNETSTRUCT::XSocket, std::list<std::string>*> m_reply;
};

#endif