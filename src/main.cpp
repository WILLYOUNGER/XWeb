#include "XBaseNet.h"
#include "XWebServer.h"
#include "XLog/XLog.h"
#include <libgen.h>
#include <iostream>
#include <unistd.h>
//#include <Config.h>
using namespace std;
using namespace XNETBASE;
int main(int argc, char* argv[])
{
	if (!XLOG->init("XWebLog.txt", 1))
	{
		return 0;
	}

	//XLOG_INFO("%s, version: %d.%d.%d", argv[0], XWEB_VERSION_MAJOR, XWEB_VERSION_MINOR, XWEB_VERSION_PATCH);

	XWebServer server;
	while (true)
	{
		sleep(1000);
	}

	return 0;
}