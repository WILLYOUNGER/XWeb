#ifndef X_SERVLET_H
#define X_SERVLET_H

#include "../XStruct/XHttpStruct.h"
#include <map>

class XServlet;

typedef XServlet* (*funcPtr)();

class XServlet
{
public:
	virtual void doGet(XNETSTRUCT::XRequest &req, XNETSTRUCT::XResponse &res) {}

	virtual void doPost(XNETSTRUCT::XRequest &req, XNETSTRUCT::XResponse &res) {}

	std::string getPath() {return m_path;}

protected:
	void setPath(std::string str) { m_path = str;}

	static std::map<std::string, std::string> m_secion;

	std::string m_path;
};

class XServletFactory
{
public:
	static XServlet* getInstance(const std::string &className);
	static void registerClass(const std::string &className, funcPtr func);
};

class Register
{
public:
    Register(const char* className, funcPtr fp);
};

#endif