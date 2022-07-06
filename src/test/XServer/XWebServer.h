/**
*  @file XWebServer.h
*  @brief    Web服务器
*  Details.
*
*  @author   wangxinxiao
*  @email    wxx1035@163.com
*  @version  1.0.1.0
*  @date     2022/6/30
*
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*  2022/5/4 | 1.0.0.0  | wangxinxiao      | Create file
*  2022/6/30 | 1.0.1.0  | wangxinxiao      | 改为继承服务器基类 XServerBase
*-
*/

#ifndef X_TEST_WEBSERVER_H
#define X_TEST_WEBSERVER_H

#include "XServerBase.h"
#include "XPthreadPool.h"
#include "XHttp.h"

/**
 * @brief Web服务器类
 * 
 */
class XWebServer : public XServerBase
{
public:
	XWebServer(std::string ip = "0.0.0.0", int port = 23333);
private:
	void init(std::string ip, int port);

	virtual void ConnectCallback(XNETSTRUCT::XSocket socket) override;

	virtual void CloseCallback(void) override;

	virtual void ReadCallback(XNETSTRUCT::XMsgPtr msg) override;

	virtual bool WriteCallback(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket socket) override;
private:
	XPthreadPool<XNETSTRUCT::XMsgPtr, XHttp>* pool;
};

#endif /* X_TEST_WEBSERVER_H */