#ifndef X_HTTP_STRUCT_H
#define X_HTTP_STRUCT_H

#include <map>
#include <string>
#include <unistd.h>
#include <sys/mman.h>

namespace XNETSTRUCT {
typedef enum _method {
	NONE = 0,
	GET,
	POST,
	HEAD,
	PUT,
	DELETE,
	TRACE,
	OPTIONS,
	CONNECT,
	PATCH
} METHOD;

typedef enum _http_code {
	NO_REQUEST,
	GET_REQUEST,
	BAD_REQUEST,
	NO_RESOURCE,
	FORBIDDEN_REQUEST,
	FILE_REQUEST,
	CSS_REQUEST,
	ICO_REQUEST,
	JPG_REQUEST,
	GIF_REQUEST,
	INTERNAL_ERROR,
	CLOSED_CONNECTION
} HTTP_CODE;

class XRequest
{
public:
	XRequest()
	{
		m_method = NONE;
	}

	void setMethod(METHOD method)
	{
		m_method = method;
	}

	METHOD getMethod() { return m_method;}

	std::string getAttribute(std::string key) {return m_attribute[key];}

	void setAttribute(std::string key, std::string value) {m_attribute.insert(std::pair<std::string, std::string>(key, value));}

	void clear()
	{
		m_attribute.clear();
		m_method = NONE;
		m_path = "/";
		m_version = "HTTP/1.0";
	}

	std::string getPath() {return m_path;}

	void setPath(std::string str) { m_path = str;}

	std::string getVersion() {return m_version;}

	void setVersion(std::string str) { m_version = str;}

	int getAttributeNum()
	{
		return m_attribute.size();
	}
private:
	METHOD m_method;

	std::string m_path;

	std::string m_version;

	std::map<std::string, std::string> m_attribute;
};

class XResponse
{
public:
	XResponse()
	{
		clear();
	}

	~XResponse()
	{
		munmap(m_file_address, m_content_length);
        m_file_address = 0;
	}

	void setHttpCode(HTTP_CODE code) {m_http_code = code;}

	HTTP_CODE getHttpCode() {return m_http_code;}

	void clear()
	{
		m_http_code = NO_RESOURCE;
		m_isEmpty = true;
		m_content_length = 0;
	}

	void setNotEmpty()
	{
		m_isEmpty = false;
	}

	bool isEmpty()
	{
		return m_isEmpty;
	}

	void setFileAddress(char* address)
	{
		m_file_address = address;
	}

	char* getFileAddress()
	{
		return m_file_address;
	}

	void setContentLength(int length)
	{
		m_content_length = length;
	}

	int getContentLength()
	{
		return m_content_length;
	}

	void setHeadString(std::string _head)
	{
		m_head = _head;
	}

	std::string getHeadString()
	{
		return m_head;
	}

	void setNeedClose(bool needClose)
	{
		m_b_isNeedClose = needClose;
	}

	bool getNeedClose()
	{
		return m_b_isNeedClose;
	}

private:
	HTTP_CODE m_http_code;

	bool m_isEmpty;

	char* m_file_address;

	int m_content_length;

	bool m_b_isNeedClose { true };

	std::string m_head;
};

}

#endif