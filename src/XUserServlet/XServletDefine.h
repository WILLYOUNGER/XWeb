#ifndef X_SERVLET_DEFINE_H
#define X_SERVLET_DEFINE_H

#include <map>
#include <string>
#include "../XHTTP/XServlet.h"

extern std::map<const std::string, funcPtr> XServletMap;

#define REGISETRSERVLET(ClassName) \
	class Register##ClassName \
	{	\
		public: \
		static XServlet* getInstance() \
		{	\
			return new ClassName; \
		}	\
		private:	\
		static const Register _staticRegister;	\
	};	\
	const Register Register##ClassName::_staticRegister(#ClassName, Register##ClassName::getInstance);

#endif