#ifndef X_UTILS_H
#define X_UTILS_H

#include "XStruct.h"

namespace XUTILS{

class Utils
{
public:
	static Utils* getInstance();

	int setnonblocking(int fd);
	//@parm one_shot : one Deal
	//@parm mode : 1 : ET / 0 : LT
	void addfd(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket fd, bool one_shot, int mode);

	void removefd(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket fd);

	void modfd(XNETSTRUCT::XSocket epollfd, XNETSTRUCT::XSocket fd, int ev, int mode);

	void addsig(int sig, void (handler)(int), bool restart = true );

	void senderror(XNETSTRUCT::XSocket connfd, const char* info);
	//set close socket now !!!
	void setFdCloseNow(XNETSTRUCT::XSocket connfd, int isCloseNow, int time);
private:
	Utils();

	Utils(Utils &utils) = delete;

	~Utils();

	void init();
private:
	static Utils* m_instance;
};

#define UTILS Utils::getInstance()
}


#endif