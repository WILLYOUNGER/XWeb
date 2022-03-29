#ifndef X_STRUCT_H
#define X_STRUCT_H

#include <string>
#include <memory>
#include <functional>
#include <iostream>

namespace XNETSTRUCT {

static const int MAX_EVENT_NUMBER = 10000;

static const int MAX_FD = 65535;

static const int READ_BUFFER_SIZE = 2048;

static const int WRITE_BUFFER_SIZE = 1024;

typedef enum _messageType
{
	BYTESTREAM = 0,
	STRUCTURALBODY,
} MESSAGETYPE;

typedef enum _protoType
{
	TCPNET = 0,
	UDPNET,
} PROTOTYPE;

class XMessage;

typedef int XSocket;
typedef int XPipe;
typedef std::shared_ptr<XMessage> XMsgPtr;
typedef std::function<void(XSocket)> XSConCB;
typedef std::function<void(void)> XSCloseCB;
typedef std::function<void(XMsgPtr)> XSReadCB;
typedef std::function<void(XSocket, XSocket)> XSWriteCB;

class XMessage
{
public:
	XMessage():message(""), m_fd(0){};

	XMessage(XMessage *msg)
	{
		message = msg->message;
		m_fd = msg->m_fd;
		m_epollfd = msg->m_epollfd;
		std::cout << "copy message epollfd: " << m_epollfd << std::endl;
		std::cout << "copy message fd: " << m_fd << std::endl;
	}

	XMessage(char* _message, XSocket epollfd, XSocket _fd)
	{
		message = _message;
		m_epollfd = epollfd;
		m_fd = _fd;
		std::cout << "message epollfd: " << epollfd << std::endl;
		std::cout << "message m_epollfd: " << m_epollfd << std::endl;
		std::cout << "message fd: " << m_fd << std::endl;
	}

	std::string getContent() const
	{
		return message;
	}

	void setContent(char* _message)
	{
		message = _message;
	}

	void setSocket(XSocket _fd)
	{
		m_fd = _fd;
	}

	void setEpollfd(XSocket epollfd)
	{
		m_epollfd = epollfd;
	}

	XSocket getSocket()
	{
		return m_fd;
	}

	XSocket getEpollfd()
	{
		return m_epollfd;
	}
private:
	std::string message;

	XSocket m_fd;

	XSocket m_epollfd;
};

}



#endif