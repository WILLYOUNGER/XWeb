#include "XBaseNet.h"
#include <sys/types.h>	//socket's types
#include <sys/socket.h>	//socket
#include <netinet/in.h>	//socket's len about inet_ntop
#include <arpa/inet.h>	//inet_ntop and inet_pton
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>		//close
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <pthread.h>

#include "XUtils.h"
#include "XLog.h"

using namespace std;
using namespace XNETBASE;
using namespace XNETSTRUCT;
using namespace XUTILS;

XServer::XServer()
{
	XServer("0.0.0.0", 23333);
}

XServer::XServer(std::string ip, int port, int mode, PROTOTYPE protoType)
{
	m_ip = ip;
	m_port = port;
	m_mode = mode;
}

XServer::~XServer()
{
	stopListen();
}

void XServer::beginListen()
{
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, m_ip.c_str(), &address.sin_addr);
	address.sin_port = htons(m_port);

	m_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(m_sockfd >= 0);

	UTILS->setFdCloseNow(m_sockfd, 1, 0);

	int ret = bind(m_sockfd, (struct sockaddr*) &address, sizeof(address) );
	assert(ret != -1);

	ret = listen(m_sockfd, m_max_connect_num);
	assert(ret != -1);

	m_epollfd = epoll_create(5);

	XLOG_DEBUG("server epollfd:%d, port: %d", m_epollfd, m_port);

	assert(m_epollfd != -1);
	UTILS->setnonblocking(m_epollfd);
	UTILS->addfd(m_epollfd, m_sockfd, false, m_mode);

	UTILS->addsig(SIGPIPE, SIG_IGN);

	assert(m_epollfd >= 0);

	int reuse = 1;
	setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	m_stop = false;
	pthread_create(&m_pthreadId, NULL, loop, (void*)this);
}

void XServer::sig_handler(int sig)
{
	int save_errno = errno;
	int msg = sig;
	errno = save_errno;
}

void* XServer::loop(void* _this)
{
	XServer* object = (XServer*)_this;
	object->run();
	return _this;
}

void XServer::run()
{
	while (!m_stop)
	{
		epoll_event events[MAX_EVENT_NUMBER];
		int ret = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		for (int i = 0; i < ret; i++)
		{
			XSocket sockfd = events[i].data.fd;
			if (sockfd == m_sockfd)
			{
				struct sockaddr_in client_address;
				socklen_t client_addresslength = sizeof(client_address);
				XSocket connfd = accept(m_sockfd, (struct sockaddr*) &client_address, &client_addresslength);
				if (connfd < 0)
				{
					XLOG_ERROR("error:%d", errno);
				}
				if (m_sockfd_num >= MAX_FD)
				{
					UTILS->senderror(connfd, "this server is busy");
					continue;
				}
				if (m_connectCb == nullptr)
				{
					UTILS->senderror(connfd, "not find server");
				}
				else
				{
					m_connectCb(connfd);
					//_Debug
					int reuse = 1;
					setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
					UTILS->addfd(m_epollfd, connfd, true, m_mode);
					m_sockfd_num++;
				}
			}
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				UTILS->removefd(m_epollfd, sockfd);
				m_sockfd_num--;
			}
			else if (events[i].events & EPOLLIN)
			{
				if (m_readCb == nullptr)
				{
					UTILS->senderror(sockfd, "server cant recv message");
				}
				else
				{
					int _read_bytes_num = 0;
					int _read_bytes_idx = 0;
					char _read_buf[READ_BUFFER_SIZE];
					memset(_read_buf, '\0', READ_BUFFER_SIZE);
					int _isError = false;
					while (true)
					{
						if (_read_bytes_idx > READ_BUFFER_SIZE)
						{
							break;
						}
						_read_bytes_num = recv(sockfd, _read_buf + _read_bytes_idx, READ_BUFFER_SIZE - _read_bytes_idx, 0);
						if (_read_bytes_num == -1)
						{
							if (errno == EAGAIN || errno == EWOULDBLOCK)
							{
								break;
							}
							UTILS->senderror(sockfd, "socket is error");
							_isError = true;
							break;
						}
						else if (_read_bytes_num == 0)
						{
							UTILS->senderror(sockfd, "recv a null requset");
							_isError = true;
							break;
						}
						_read_bytes_idx += _read_bytes_num;
					}
					if (!_isError)
					{
						auto mes = make_shared<XMessage>(new XMessage(_read_buf, m_epollfd, sockfd));
						m_readCb(mes);
					}
				}
			}
			else if (events[i].events & EPOLLOUT)
			{
				if (m_writeCb == nullptr)
				{
					UTILS->senderror(sockfd, "server cant send message");
				}
				else
				{
					if (m_writeCb(m_epollfd, sockfd))
					{
						UTILS->removefd(m_epollfd, sockfd);
						m_sockfd_num--;
					}
				}
			}
		}
	}
}

void XServer::stopListen()
{
	if (m_stop == false)
	{
		m_stop = true;
		pthread_join(m_pthreadId, NULL);
	}
}

XClient::XClient(string ip, int port, int mode, PROTOTYPE protoType)
	:m_ip(ip), m_port(port), m_mode(mode), m_protoType(protoType)
{
}

XClient::~XClient()
{
	stopConnect();
}

void XClient::beginConnect()
{
	memset(&m_address, 0, sizeof(m_address));
	m_address.sin_family = AF_INET;
	inet_pton(AF_INET, m_ip.c_str(), &m_address.sin_addr);
	m_address.sin_port = htons(m_port);

	m_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(m_sockfd >= 0);

	m_epollfd = epoll_create(5);

	m_stop = false;
	m_connected  = false;
	pthread_create(&m_pthreadId, NULL, loop, (void*)this);
}

void XClient::sendMessage(XMsgPtr _msg)
{
	;
}

void XClient::stopConnect()
{
	if (m_stop == false)
	{
		m_stop = true;
		pthread_join(m_pthreadId, NULL);
	}
}

void* XClient::loop(void* _this)
{
	XClient* _client = (XClient*) _this;
	_client->run();
	return _this;
}

void XClient::run()
{
	while (!m_stop)
	{
		if (m_connected)
		{
			epoll_event events[MAX_EVENT_NUMBER];
			int number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
			if (number < 0 && errno != EINTR)
			{
				XLOG_ERROR("epoll failure");
				break;
			}
			for (int i = 0; i < number; i++)
			{
				int sockfd = events[i].data.fd;
				if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				{
					UTILS->removefd(m_epollfd, m_sockfd);
					m_connected = false;
				}
				else if (events[i].events & EPOLLIN)
				{
					if (m_readCb)
					{
						char _read_buf[READ_BUFFER_SIZE];
						int _read_bytes_num = 0;
						int _read_bytes_idx = 0;
						bool _isError = false;
						memset(_read_buf, '\0', sizeof(_read_buf));
						while (true)
						{
							if (_read_bytes_idx > READ_BUFFER_SIZE)
							{
								break;
							}
							_read_bytes_num = recv(m_sockfd, _read_buf + _read_bytes_idx, READ_BUFFER_SIZE - _read_bytes_idx, 0);
							if (_read_bytes_num < 0)
							{
								if (errno == EAGAIN || errno == EWOULDBLOCK)
								{
									break;
								}
								_isError = true;
								break;
							}
							else if (_read_bytes_num == 0)
							{
								_isError = true;
								break;
							}
							_read_bytes_idx += _read_bytes_num;
						}
						if (!_isError)
						{
							auto _msg = make_shared<XMessage>(new XMessage(_read_buf, m_epollfd, sockfd));
							m_readCb(_msg);
						}
					}
					else
					{
						XLOG_ERROR("client's readCallback is null.");
					}
				}
				else if (events[i].events & EPOLLIN)
				{
					if (m_writeCb)
					{
						m_writeCb(m_epollfd, sockfd);
					}
					else
					{
						XLOG_ERROR("client's wirteCallback is null.");
					}
				}
			}
		}
		else
		{
			if (connect(m_sockfd, (struct sockaddr*)&m_address, sizeof(m_address)) >= 0)
			{
				if (m_connectCb)
				{
					UTILS->addfd(m_epollfd, m_sockfd, true, m_mode);
					m_connectCb(m_sockfd);
					m_connected = true;

				}
				else
				{
					close(m_sockfd);
					XLOG_ERROR("client's connectCallback is null.");
				}
			}
		}
	}
}