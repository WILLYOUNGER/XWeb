#include "XWebServer.h"
#include "./XUtils/XUtils.h"
#include "./XLog/XLog.h"
#include <sys/epoll.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;
using namespace XNETBASE;
using namespace XNETSTRUCT;
using namespace XUTILS;

std::map<XNETSTRUCT::XSocket, std::list<XResponse>*> XWebServer::m_reply;

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
    XLOG_DEBUG("a client connected.connfd: ", to_string(socket).c_str());
}

void XWebServer::CloseCallback(void)
{
}

void XWebServer::ReadCallback(XMsgPtr msg)
{
    pool->append(msg);
}

bool XWebServer::WriteCallback(XSocket epollfd, XSocket socket)
{
    if (XWebServer::m_reply.count(socket) == 0)
    {
        UTILS->modfd(epollfd, socket, EPOLLIN, 0);
        return true;
    }
    else
    {
        auto temp = XWebServer::m_reply[socket]->front();

        if (temp.isEmpty() == false)
        {
            struct iovec _iv[2];
            int _iv_count;
            char* _head = (char*)malloc(temp.getHeadString().size() * sizeof(char));
            strncpy(_head, temp.getHeadString().c_str(), temp.getHeadString().size());
            _iv[0].iov_base = _head;
            _iv[0].iov_len = temp.getHeadString().size();
            _iv[1].iov_base = temp.getFileAddress();
            _iv[1].iov_len = temp.getContentLength();
            _iv_count = 2;
            int bytes_to_send = _iv[0].iov_len + _iv[1].iov_len;
            int bytes_have_send = 0;

            _head[temp.getHeadString().size()] = '\0';

            int templen = writev(socket, _iv, _iv_count);
            if (templen < 0)
            {
                if (errno == EAGAIN)
                {
                    UTILS->modfd(epollfd, socket, EPOLLOUT, 0);
                }
            }
            else
            {

            }

            XWebServer::m_reply[socket]->pop_front();
            UTILS->modfd(epollfd, socket, EPOLLIN, 0);

            return temp.getNeedClose();
        }
        else
        {
            return true;
        }
    }
}