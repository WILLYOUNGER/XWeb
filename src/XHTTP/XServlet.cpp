#include "XServlet.h"

using namespace std;

std::map<const std::string, funcPtr> XServletMap;

XServlet* XServletFactory::getInstance(const string &className)
{
	for (auto it : XServletMap)
	{
		if (className == it.first)
		{
			return it.second();
		}
	}
	return nullptr;
}

void XServletFactory::registerClass(const std::string &className, funcPtr func)
{
	XServletMap.insert(pair<string, funcPtr>(className, func));
}

Register::Register(const char* className, funcPtr fp)
{
    XServletFactory::registerClass(className, fp);
}
