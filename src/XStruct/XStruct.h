#ifndef X_STRUCT_H
#define X_STRUCT_H

#include <string>
#include <memory>
#include <functional>

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


class XMessage
{
public:
	XMessage():message(""), m_fd(0){};

	XMessage(XMessage *msg)
	{
		message = msg->message;
	}

	XMessage(char* _message, XSocket, epollfd, XSocket _fd)
	{
		message = _message;
		m_fd = _fd;
		m_epollfd = epollfd;
	}

	std::string getContent() const
	{
		return message;
	}

	void  setContent(char* _message)
	{
		message = _message;
	}

	void setSocket(XSocket _fd)
	{
		m_fd = _fd;
	}

	XSocket getSocket()
	{
		return m_fd;
	}

	XSocket getEpollfd();
private:
	std::string message;

	XSocket m_fd;

	XSocket m_epollfd;
};

typedef int XSocket;
typedef int XPipe;
typedef std::shared_ptr<XMessage> XMsgPtr;
typedef std::function<void(XSocket)> XSConCB;
typedef std::function<void(void)> XSCloseCB;
typedef std::function<void(XMsgPtr)> XSReadCB;
typedef std::function<void(XSocket)> XSWriteCB;

}



#endif