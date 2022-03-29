#include "XUtils.h"
#include "fcntl.h"
#include "signal.h"
#include "errno.h"
#include "assert.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>	//close(int sockfd)
#include <cstring>

using namespace std;
using namespace XUTILS;
using namespace XNETSTRUCT;

Utils* Utils::m_instance = nullptr;

Utils* Utils::getInstance()
{
	if (m_instance == nullptr)
	{
		m_instance->init();
	}
	return m_instance;
}

Utils::~Utils()
{
}

Utils::Utils()
{
}

void Utils::init()
{
}

int Utils::setnonblocking(XSocket fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option |= O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void Utils::addfd(XSocket epollfd, XSocket fd, bool one_shot, int mode)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLRDHUP;
	if (one_shot)
	{
		event.events |= EPOLLONESHOT;
	}
	if (mode == 1)
	{
		event.events |= EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

void Utils::removefd(XSocket epollfd, XSocket fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

void Utils::modfd(XSocket epollfd, XSocket fd, int ev, int mode)
{
	epoll_event event;
	event.data.fd = fd;
    if (1 == mode)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void Utils::addsig(int sig, void (handler)(int), bool restart)
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = handler;
	if (restart)
	{
		sa.sa_flags |= SA_RESTART;
	}
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig, &sa, NULL) != -1);
}

void Utils::senderror(XSocket connfd, const char* info)
{
	cout << info << endl;
	send(connfd, info, strlen(info), 0);
	close(connfd);
}

