#include "XNet/XBaseNet.h"
#include "XWebServer.h"
#include "XLog/XLog.h"
#include <libgen.h>
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace XNETBASE;
int main(int argc, char* argv[])
{
	//XServer _server;
	//_server.beginListen();
	if (!XLOG->init("XWebLog.txt", 1))
	{
		return 0;
	}

	XLOG_DEBUG("debug");
	XLOG_INFO("info");
	XLOG_WARN("warn");
	XLOG_ERROR("error");

	XWebServer server;
	while (true)
	{
		sleep(1000);
	}

	return 0;
}