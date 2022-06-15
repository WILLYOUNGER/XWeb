#ifndef X_BASE_NET_H
#define X_BASE_NET_H

#include <pthread.h>
#include <netinet/in.h>
#include "XStruct.h"

namespace XNETBASE{

class XServer final
{
public:
	XServer();

	XServer(XServer &server) = delete;

	~XServer();

	XServer(std::string ip, int port, int mode = 0, XNETSTRUCT::PROTOTYPE protoType = XNETSTRUCT::TCPNET);

	void setConnectionCallback(XNETSTRUCT::XSConCB _fun) {m_connectCb = _fun; }

	void setCloseCallback(XNETSTRUCT::XSCloseCB _fun) {m_closeCb = _fun; }

	void setReadCallback(XNETSTRUCT::XSReadCB _fun) {m_readCb = _fun; }

	void setWriteCallback(XNETSTRUCT::XSWriteCB _fun) {m_writeCb = _fun; }

	void setMessageType(XNETSTRUCT::MESSAGETYPE _type) {m_messageType = _type; }

	void beginListen();

	void sendMessage(XNETSTRUCT::XMsgPtr _msg);

	void setMaxConnectNum(int _num) {m_max_connect_num = _num;};

	void stopListen();

private:
	static void* loop(void* _this);

	void run();

	static void sig_handler(int sig);
private:
	std::string m_ip {"0.0.0.0"};
	int m_port {23333};
	int m_max_connect_num {100};
	XNETSTRUCT::XSocket m_sockfd{-1};
	XNETSTRUCT::XSocket m_epollfd{-1};
	int m_mode{0};

	XNETSTRUCT::MESSAGETYPE m_messageType {XNETSTRUCT::BYTESTREAM};

	XNETSTRUCT::PROTOTYPE m_protoType {XNETSTRUCT::TCPNET};

	XNETSTRUCT::XSConCB m_connectCb {nullptr};
	XNETSTRUCT::XSCloseCB m_closeCb {nullptr};
	XNETSTRUCT::XSReadCB m_readCb {nullptr};
	XNETSTRUCT::XSWriteCB m_writeCb {nullptr};

	bool m_stop {true};

	pthread_t m_pthreadId;

	int m_sockfd_num {0};
};

class XClient final
{
public:

	XClient(XClient &client) = delete;

	~XClient();

	XClient(std::string ip, int port, int mode = 0, XNETSTRUCT::PROTOTYPE protoType = XNETSTRUCT::TCPNET);

	void setConnectionCallback(XNETSTRUCT::XSConCB _fun) {m_connectCb = _fun; }

	void setCloseCallback(XNETSTRUCT::XSCloseCB _fun) {m_closeCb = _fun; }

	void setReadCallback(XNETSTRUCT::XSReadCB _fun) {m_readCb = _fun; }

	void setWriteCallback(XNETSTRUCT::XSWriteCB _fun) {m_writeCb = _fun; }

	void setMessageType(XNETSTRUCT::MESSAGETYPE _type) {m_messageType = _type; }

	void beginConnect();

	void sendMessage(XNETSTRUCT::XMsgPtr _msg);

	void stopConnect();

private:
	XClient();

	static void* loop(void* _this);

	void run();

	static void sig_handler(int sig);
private:
	std::string m_ip;
	int m_port;
	XNETSTRUCT::XSocket m_sockfd{-1};
	XNETSTRUCT::XSocket m_epollfd{-1};
	int m_mode;

	XNETSTRUCT::MESSAGETYPE m_messageType {XNETSTRUCT::BYTESTREAM};

	XNETSTRUCT::PROTOTYPE m_protoType {XNETSTRUCT::TCPNET};

	XNETSTRUCT::XSConCB m_connectCb {nullptr};
	XNETSTRUCT::XSCloseCB m_closeCb {nullptr};
	XNETSTRUCT::XSReadCB m_readCb {nullptr};
	XNETSTRUCT::XSWriteCB m_writeCb {nullptr};

	bool m_stop {true};

	bool m_connected{false};

	pthread_t m_pthreadId;

	sockaddr_in m_address;
};


}

#endif