#include "XWebServer.h"
#include "./XUtils/XUtils.h"
#include "./XLog/XLog.h"
#include <sys/epoll.h>

using namespace std;
using namespace XNETBASE;
using namespace XNETSTRUCT;
using namespace XUTILS;

std::map<XNETSTRUCT::XSocket, std::list<std::string>*> XWebServer::m_reply;

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

    auto writeCb = bind(&XWebServer::WriteCallback, this, std::placeholders::_1, std::placeholders::_2);
    m_web_server->setWriteCallback(writeCb);

    m_web_server->beginListen();
}

void XWebServer::ConnectCallback(XSocket socket)
{
    XLOG->write_log(XLog::DEBUG, "a client connected.connfd: ", to_string(socket).c_str());
}

void XWebServer::CloseCallback(void)
{
    XLOG->write_log(XLog::DEBUG, "a client stopConnect.");
}

void XWebServer::ReadCallback(XMsgPtr msg)
{
    pool->append(msg);
    XLOG->write_log(XLog::DEBUG, "recv a request.");
}

void XWebServer::WriteCallback(XSocket epollfd, XSocket socket)
{
    if (XWebServer::m_reply.count(socket) == 0)
    {
        UTILS->modfd(epollfd, socket, EPOLLIN, 0);
        XLOG->write_log(XLog::DEBUG, "not find need send message.");
        return;
    }
    else
    {
        send(socket, XWebServer::m_reply[socket]->front().c_str(), XWebServer::m_reply[socket]->front().length(), 0);
        XWebServer::m_reply[socket]->pop_front();
        UTILS->modfd(epollfd, socket, EPOLLIN, 0);
        XLOG->write_log(XLog::DEBUG, "send message success.");
    }
}