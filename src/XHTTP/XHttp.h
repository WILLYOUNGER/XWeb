#ifndef X_HTTP_H
#define X_HTTP_H

#include "../XStruct/XStruct.h"
#include "../XStruct/XHttpStruct.h"
#include "XServlet.h"

class XHttp
{
public:
	enum CHECK_STATE {
		CHECK_STATE_REQUESTLINE = 0,
		CHECK_STATE_HEADER,
		CHECK_STATE_CONTENT
	};

	enum LINE_STATUS {
		LINE_OK = 0,
		LINE_BAD,
		LINE_OPEN,
	};

public:
	XHttp();

	~XHttp();

	XNETSTRUCT::HTTP_CODE process(XNETSTRUCT::XMsgPtr &msg);
private:
	void init();

	LINE_STATUS parse_line();

	XNETSTRUCT::HTTP_CODE generateRequest();

	XNETSTRUCT::HTTP_CODE parse_request_line(std::string &str);

	XNETSTRUCT::HTTP_CODE parse_request_header(std::string &str);

	XNETSTRUCT::HTTP_CODE parse_request_content(std::string &str);

	int split(std::string &str, char _sig, std::string &res)
	{
		int pos = 0;
		while (str.length() > 0 && str[pos] == _sig)
		{
			pos++;
		}
		str = str.substr(pos, str.length() - pos);
		pos = str.find(_sig);
		if (pos == -1)
		{
			res = str.substr(0, str.length());
			str = "";
			return 0;
		}
		else
		{
			res = str.substr(0, pos);
			str = str.substr(pos + 1, str.length() - pos);
			return 1;
		}
	}

	bool path2servlet(std::string path);

	bool xml2map();

	XNETSTRUCT::METHOD Str2MetHod(std::string &str) const;

	void sendResponse();

	void do_request(std::string &str);
private:
	int m_read_now{0};
	int m_read_end{0};
	int m_line_begin{0};
	int m_line_end{0};
	int m_content_length{0};
	XNETSTRUCT::METHOD m_method;
	CHECK_STATE m_check_state;

	XNETSTRUCT::XMsgPtr m_msg;
	std::string m_msg_str;

	XNETSTRUCT::XRequest m_request;
	XNETSTRUCT::XResponse m_response;

	std::map<std::string, XServlet*> m_servlet;
};

#endif