#include "XWebServer.h"
#include <iostream>

using namespace std;
using namespace XNETBASE;
using namespace XNETSTRUCT;
XWebServer::XWebServer()
{
    pool = new XPthreadPool<XMsgPtr, XHttp>(8, 10000);

	m_web_server = new XServer("0.0.0.0", 23333);
	auto connCb = bind(&XWebServer::ConnectCallback,this,std::placeholders::_1);
    m_web_server->setConnectionCallback(connCb);

    auto closeCb = bind(&XWebServer::CloseCallback, this);
    m_web_server->setCloseCallback(closeCb);

    auto readCb = bind(&XWebServer::ReadCallback,this,std::placeholders::_1);
    m_web_server->setReadCallback(readCb);

    auto writeCb = bind(&XWebServer::WriteCallback, this, std::placeholders::_1);
    m_web_server->setWriteCallback(writeCb);

    m_web_server->beginListen();
}

void XWebServer::ConnectCallback(XSocket socket)
{
    cout << "a client connected." << endl;
}

void XWebServer::CloseCallback(void)
{
    cout << "a client stopConnect." << endl;
}

void XWebServer::ReadCallback(XMsgPtr msg)
{
    //cout << "recv:"<< endl << msg->getContent() << endl;
    pool->append(msg);
}

void XWebServer::WriteCallback(XSocket socket)
{

}