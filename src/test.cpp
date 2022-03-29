#include "XNet/XBaseNet.h"
#include "XWebServer.h"
#include <libgen.h>
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace XNETBASE;
int main(int argc, char* argv[])
{
	//XServer _server;
	//_server.beginListen();

	XWebServer server;
	while (true)
	{
		sleep(1000);
	}

	return 0;
}