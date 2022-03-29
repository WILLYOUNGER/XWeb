#include "XWebServer.h"
#include "./XUtils/XUtils.h"
#include <iostream>
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
    cout << "a client connected." << endl;
    cout << "connfd: " << socket << endl;
}

void XWebServer::CloseCallback(void)
{
    cout << "a client stopConnect." << endl;
}

void XWebServer::ReadCallback(XMsgPtr msg)
{
    cout << "recv:"<< endl << msg->getContent() << endl;
    pool->append(msg);
    cout << "read epollfd: " << msg->getEpollfd() << endl;
    cout << "read fd: " << msg->getSocket() << endl;
}

void XWebServer::WriteCallback(XSocket epollfd, XSocket socket)
{
    cout << "sending message!" << endl;
    if (XWebServer::m_reply.count(socket) == 0)
    {
        UTILS->modfd(epollfd, socket, EPOLLIN, 0);
        return;
    }
    else
    {
        send(socket, XWebServer::m_reply[socket]->front().c_str(), XWebServer::m_reply[socket]->front().length(), 0);
        XWebServer::m_reply[socket]->pop_front();
        UTILS->modfd(epollfd, socket, EPOLLIN, 0);
        cout << "sending message success!" << endl;
    }
}