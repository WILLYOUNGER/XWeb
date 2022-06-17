#include "XHttp.h"
#include <unistd.h>
#include "tinyxml.h"
#include "tinystr.h"
#include "XServletDefine.h"
#include "XUtils.h"
#include "XWebServer.h"
#include "XLog.h"

#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <regex>

using namespace std;
using namespace XNETSTRUCT;
using namespace XUTILS;

XHttp::XHttp()
{
	init();
}

XHttp::~XHttp()
{

}

void XHttp::init()
{
	xml2map();
	m_b_isKeepAlive = true;
}

void XHttp::process(XMsgPtr &msg)
{
	XLOG_INFO("%s", msg->getContent().c_str());
	m_msg = msg;
	m_read_now = 0;
	m_line_begin = 0;
	m_content_length == 0;
	m_request.clear();
	m_response.clear();
	m_read_end = msg->getContent().length();
	m_method = GET;
	m_msg_str = msg->getContent();
	m_check_state = CHECK_STATE_REQUESTLINE;
	HTTP_CODE res = generateRequest();
	if (res == GET_REQUEST)
	{
		path2servlet(m_request.getPath());
		if (m_response.isEmpty())
		{
			do_request(m_request.getPath());
		}
	}
	else if (res == BAD_REQUEST)
	{

	}
	else if (res == INTERNAL_ERROR)
	{

	}
	else if (res == NO_REQUEST)
	{

	}

	sendResponse();
}

HTTP_CODE XHttp::generateRequest()
{
	LINE_STATUS line_status = LINE_OK;

	HTTP_CODE ret = NO_REQUEST;
	string now_line;

	while (((m_check_state == CHECK_STATE_CONTENT) && (line_status == LINE_OK)) || (line_status = parse_line()) == LINE_OK)
	{
		string now_line = m_msg_str.substr(m_line_begin, m_line_end - m_line_begin);
		m_line_begin = m_read_now;
		//cout << "got a http line: " << now_line << endl;

		switch (m_check_state)
		{
			case CHECK_STATE_REQUESTLINE:
			{
				ret = parse_request_line(now_line);
				if (ret == BAD_REQUEST)
				{
					return BAD_REQUEST;
				}
				break;
			}
			case CHECK_STATE_HEADER:
			{
				ret = parse_request_header(now_line);
				if (ret == BAD_REQUEST)
				{
					return BAD_REQUEST;
				}
				else if (ret == GET_REQUEST)
				{
					return GET_REQUEST;
				}
				break;
			}
			case CHECK_STATE_CONTENT:
			{
				ret = parse_request_content(now_line);
				if (ret == GET_REQUEST)
				{
					return GET_REQUEST;;
				}
				line_status == LINE_OPEN;
				break;
			}
			default:
			{
				return INTERNAL_ERROR;
			}
		}
	}
	return NO_REQUEST;
}

XHttp::LINE_STATUS XHttp::parse_line()
{
	char now;
	m_line_begin = m_read_now;
	for (; m_read_now < m_read_end; m_read_now ++)
	{
		now = m_msg_str[m_read_now];
		if (now == '\r')
		{
			if (m_read_now + 1 == m_read_end)
			{
				return LINE_OPEN;
			}
			else if (m_msg_str[m_read_now + 1] == '\n')
			{
				m_line_end = m_read_now;
				m_read_now+=2;
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if (now == '\n')
		{
			if (m_read_now > 0 && m_msg_str[m_read_now - 1] == '\r')
			{
				m_read_now++;
				m_line_end = m_read_now-1;
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
	return LINE_BAD;
}

HTTP_CODE XHttp::parse_request_line(string &str)
{
	// got a http line: GET / HTTP/1.1
	// what version http and what method
	string res;
	split(str, ' ', res);
	m_method = Str2MetHod(res);
	m_request.setMethod(m_method);
	split(str, ' ', res);
	if (res == "/")
	{
		res += "index.html";
	}
	m_request.setPath(res);
	split(str, ' ', res);
	m_request.setVersion(res);
	m_check_state = CHECK_STATE_HEADER;
	return GET_REQUEST;
}

HTTP_CODE XHttp::parse_request_header(string &str)
{
	if (str[0] == '\0')
	{
		if (m_content_length == 0)
		{
			return GET_REQUEST;
		}
		m_check_state = CHECK_STATE_CONTENT;
		return NO_REQUEST;
	}
	else
	{
		string key, value;
		split(str, ':', key);
		m_request.setAttribute(key, str);
		if (key == "Content-Length")
		{
			m_content_length = atol(str.c_str());
		}
		else if (key == "Connection")
		{
			if (value == " keep-alive" || value == "keep-alive")
			{
				m_b_isKeepAlive = true;
			}
			else
			{
				m_b_isKeepAlive = false;
			}
		}
	}
}

XNETSTRUCT::HTTP_CODE XHttp::parse_request_content(std::string &str)
{
	if (str.length() != m_content_length)
	{
		return NO_REQUEST;
	}

	string key, body;
	int ret = 1;
	while (ret)
	{
		ret = split(str, '&', body);
		split(body, '=', key);
		m_request.setAttribute(key, body);
	}

	return GET_REQUEST;
}

METHOD XHttp::Str2MetHod(string &str) const
{
	if (str == "GET")
	{
		return GET;
	}
	else if (str == "POST")
	{
		return POST;
	}
	else if (str == "HEAD")
	{
		return HEAD;
	}
	else if (str == "PUT")
	{
		return PUT;
	}
	else if (str == "DELETE")
	{
		return DELETE;
	}
	else if (str == "TRACE")
	{
		return TRACE;
	}
	else if (str == "OPTIONS")
	{
		return OPTIONS;
	}
	else if (str == "CONNECT")
	{
		return CONNECT;
	}
	else if (str == "PATCH")
	{
		return PATCH;
	}
	else
	{
		return NONE;
	}
}

bool XHttp::path2servlet(string path)
{
	XServlet* _servlet = m_servlet[path];
	if (_servlet  == nullptr)
	{
		return false;
	}
	if (m_request.getMethod() == GET)
	{
		_servlet->doGet(m_request, m_response);
	}
	else if (m_request.getMethod()  == POST)
	{
		_servlet->doPost(m_request, m_response);
	}
	else
	{

	}
	return true;
}

bool XHttp::xml2map()
{
	string appPath(getcwd(NULL, 0));
    string seperator = "/../src/XResourse/xml/web.xml";
    string fullPath = appPath + seperator;
    //cout << fullPath << endl;
    //创建一个XML的文档对象。
    TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
    myDocument->LoadFile();
    //获得根元素，即Persons。
    TiXmlElement *RootElement = myDocument->RootElement();
    //输出根元素名称，即输出Persons。
    //cout << RootElement->Value() << endl;

    for(TiXmlElement *servlet_mapping = RootElement->FirstChildElement(); servlet_mapping->FirstChildElement(); servlet_mapping = servlet_mapping->NextSiblingElement())
    {
	    TiXmlElement *NameElement = servlet_mapping->FirstChildElement();
	    TiXmlElement *urlElement = NameElement->NextSiblingElement();
	    //cout << "Servlet Name:" << NameElement->FirstChild()->Value() << endl;
	    //cout << "Url:" << urlElement->FirstChild()->Value() << endl;
	    string objName(NameElement->FirstChild()->Value());
	    string urlstr(urlElement->FirstChild()->Value());
	    XServlet* obj = XServletFactory::getInstance(objName);
	    m_servlet.insert(pair<string, XServlet*>(urlstr, obj));
    };

    myDocument->Clear();
}

void XHttp::sendResponse()
{
	if (m_response.isEmpty() || m_response.getHttpCode() == NO_REQUEST)
	{
		return;
	}

	string _httpStr = "";

	dealresponse(_httpStr);

	//string response = string(_httpStr) + string(m_response.getFileAddress());

	if (XWebServer::m_reply.count(m_msg->getSocket()) == 0)
	{
		list<XResponse> *replyList = new list<XResponse>();
		replyList->push_back(m_response);
		XWebServer::m_reply[m_msg->getSocket()] = replyList;
	}
	else
	{
		XWebServer::m_reply[m_msg->getSocket()]->push_back(m_response);
	}
	UTILS->modfd(m_msg->getEpollfd(), m_msg->getSocket(), EPOLLOUT, 0);
}

void XHttp::dealresponse(std::string &str)
{
	HTTP_CODE _code = m_response.getHttpCode();
	if (_code == NO_RESOURCE)
	{

	}
	else if (_code == FORBIDDEN_REQUEST)
	{

	}
	else if (_code == FILE_REQUEST)
	{
		str = str + "HTTP/1.1 200 OK\r\n";
		str = str + "Content-Length: " + to_string(m_response.getContentLength()) + "\r\n";
		str = str + "Connection:" + ((m_b_isKeepAlive == true) ? "keep-alive" : "close") + "\r\n";
		str = str + "\r\n";
	}
	else if (_code == INTERNAL_ERROR)
	{

	}
	else if (_code == BAD_REQUEST)
	{

	}
	else if (_code == ICO_REQUEST)
	{
		str = str + "HTTP/1.1 200 OK\r\n";
		str = str + "Content-Length: " + to_string(m_response.getContentLength()) + "\r\n";
		str = str + "Connection:" + ((m_b_isKeepAlive == true) ? "keep-alive" : "close") + "\r\n";
		str = str + "Content-Type:image/x-icon\r\n";
		str = str + "\r\n";
	}
	else if (_code == JPG_REQUEST)
	{
		str = str + "HTTP/1.1 200 OK\r\n";
		str = str + "Content-Length: " + to_string(m_response.getContentLength()) + "\r\n";
		str = str + "Connection:" + ((m_b_isKeepAlive == true) ? "keep-alive" : "close") + "\r\n";
		str = str + "Content-Type:image/x-icon\r\n";
		str = str + "\r\n";
	}
	else if (_code == GIF_REQUEST)
	{
		str = str + "HTTP/1.1 200 OK\r\n";
		str = str + "Content-Length: " + to_string(m_response.getContentLength()) + "\r\n";
		str = str + "Connection:" + ((m_b_isKeepAlive == true) ? "keep-alive" : "close") + "\r\n";
		str = str + "Content-Type:image/gif\r\n";
		str = str + "\r\n";
	}
	m_response.setHeadString(str);
	m_response.setNeedClose(!m_b_isKeepAlive);
}

void XHttp::do_request(string str)
{
	string appPath(getcwd(NULL, 0));
    string seperator = "/../src/XRoot";
    string fullPath = appPath + seperator + str;
    struct stat _file_stat;
	int _type = 0;
	std::regex html_reg(".+\\..+");
	bool ret = std::regex_match(str, html_reg);
	if (ret)
	{
		_type = -1;
	}
	// if (!ret)
	// {
	// 	std::regex ico_reg(".+\\.ico");
	// 	ret = std::regex_match(str, ico_reg);
	// 	_type = 1;
	// }
	// else
	// {
	// 	_type = -1;
	// }
	// if (!ret)
	// {
	// 	std::regex jpg_reg(".+\\.jpg");
	// 	ret = std::regex_match(str, jpg_reg);
	// 	_type = 2;
	// }
	// if (!ret)
	// {
	// 	std::regex gif_reg(".+\\.gif");
	// 	ret = std::regex_match(str, gif_reg);
	// 	_type = 3;
	// }

	if (_type != 0)
	{
		if (stat(fullPath.c_str(), &_file_stat) < 0)
		{
			m_response.setHttpCode(NO_RESOURCE);
		}

	    if (!(_file_stat.st_mode & S_IROTH))
	    {
	        m_response.setHttpCode(FORBIDDEN_REQUEST);
	    }

	    if (S_ISDIR(_file_stat.st_mode))
	    {
	        m_response.setHttpCode(BAD_REQUEST);
	    }


	    int fd = open(fullPath.c_str(), O_RDONLY);
	    m_response.setContentLength(_file_stat.st_size);
	    if (_type == -1)
	    {
	    	m_response.setHttpCode(FILE_REQUEST);
	    	char* _tmp = (char *)mmap(0, _file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	    	m_response.setFileAddress(_tmp);
	    }
	    else if (_type == 1)
	    {
	    	m_response.setHttpCode(ICO_REQUEST);
	    	char* _tmp = (char *)mmap(0, _file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	    	m_response.setFileAddress(_tmp);
	    }
	    else if (_type == 2)
	    {
	    	m_response.setHttpCode(JPG_REQUEST);
	    	char* _tmp = (char *)mmap(0, _file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	    	m_response.setFileAddress(_tmp);
	    }
	    else if (_type == 3)
	    {
	    	m_response.setHttpCode(GIF_REQUEST);
	    	char* _tmp = (char *)mmap(0, _file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	    	m_response.setFileAddress(_tmp);
	    }
	    close(fd);
	    m_response.setNotEmpty();
	}
}