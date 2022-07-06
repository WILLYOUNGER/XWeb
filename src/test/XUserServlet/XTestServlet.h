/**
*  @file XTestServlet.h
*  @brief    测试servlet类
*  Details.
*
*  @author   wangxinxiao
*  @email    wxx1035@163.com
*  @version  1.0.0.0
*  @date     2022/5/4
*
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*  2022/5/4 | 1.0.0.0  | wangxinxiao      | Create file
*-
*/

#ifndef X_TEST_USERSERVLET_H
#define X_TEST_USERSERVLET_H

#include "XServlet.h"
#include "XServletDefine.h"

/**
 * @brief 测试servlet类
 * 
 */
class XTestServlet : public XServlet
{
public:
	XTestServlet()
	{
		setPath("/test");
	}

	virtual void doGet(XNETSTRUCT::XRequest &req, XNETSTRUCT::XResponse &res) override;
};

REGISETRSERVLET(XTestServlet)

#endif /* X_TEST_USERSERVLET_H */