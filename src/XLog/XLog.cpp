#include "XLog.h"
#include <cstring>
#include <cstdarg>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys/time.h"

using namespace std;

XLog::XLog()
{
	m_count = 0;
	m_is_async = false;
    memset(m_dir_name, '\0', sizeof(m_dir_name));
    strcpy(m_dir_name, "./log");
}

XLog::~XLog()
{
	if (m_fp != NULL)
	{
		fclose(m_fp);
	}
}

bool XLog::init(const char* file_name, int close_log, int level, int log_buf_size, int split_lines, int max_queue_size)
{
    memset(m_file_name, '\0', sizeof(m_file_name));
    strcpy(m_file_name, file_name);
    m_close_log = close_log;
    m_split_lines = split_lines;
    m_log_buf_sizes = log_buf_size;
    m_level = level;
	if (max_queue_size > 0)
	{
		m_is_async = true;
		m_log_queue = new XBlockQueue<XLOGCONTENT>(max_queue_size);
		pthread_t pid;
		pthread_create(&pid, NULL, flush_log_thread, NULL);
	}
	m_buf = new char[m_log_buf_sizes];
	memset(m_buf, '\0', m_log_buf_sizes);

	time_t t = time(NULL);
	struct tm *sys_tm = localtime(&t);
	struct tm my_tm = *sys_tm;

    std::string file_full_name = std::string(m_dir_name) + "/" + to_string(my_tm.tm_year + 1900) + "_" + to_string(my_tm.tm_mon + 1) + "_" + to_string(my_tm.tm_mday) + std::string(m_file_name);

    int ret = access(m_dir_name, F_OK);
    if (!ret)
    {
        ret = mkdir(m_dir_name, S_IRWXU);
        if (!ret)
        {
            return false;
        }
    }

    const char* file_full_name_c = file_full_name.c_str();

	m_today = my_tm.tm_mday;

	m_fp = fopen(file_full_name_c, "a");

	if (m_fp == NULL)
	{
		return false;
	}
	return true;
}

void XLog::write_log(int level, string pos, const char *format, ...)
{
    if (level < m_level || m_close_log == 0)
    {
        return;
    }
	struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    XLOGCONTENT _logContent_content;
    char s[16] = {0};
    _logContent_content._str_logContent = pos;
    switch (level)
    {
    case 0:
        strcpy(s, "\033[1;32;40m");
        _logContent_content._str_logContent += "[debug] ";
        break;
    case 1:
        strcpy(s, "\033[1;34;40m");
        _logContent_content._str_logContent += "[info] ";
        break;
    case 2:
        strcpy(s, "\033[1;33;40m");
        _logContent_content._str_logContent += "[warn] ";
        break;
    case 3:
        strcpy(s, "\033[1;31;40m");
        _logContent_content._str_logContent += "[error] ";
        break;
    }
    _logContent_content._str_beginColor = s;

    //写入一个log，对m_count++, m_split_lines最大行数
    m_mutex.lock();
    m_count++;

    if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0) //everyday log
    {
        fflush(m_fp);
        fclose(m_fp);

        if (m_today != my_tm.tm_mday)
        {
            std::string file_full_name = string(m_dir_name) + "/" + to_string(my_tm.tm_year + 1900) + "_" + to_string(my_tm.tm_mon + 1) + "_" + to_string(my_tm.tm_mday) + string(m_file_name);
            const char* file_full_name_c = file_full_name.c_str();
            m_fp = fopen(file_full_name_c, "a");
        }
        else
        {
            std::string file_full_name = string(m_dir_name) + "/" + to_string(my_tm.tm_year + 1900) + "_" + to_string(my_tm.tm_mon + 1) + "_" + to_string(my_tm.tm_mday) + "_" + to_string(m_count / m_split_lines) + std::string(m_file_name);
            const char* file_full_name_c = file_full_name.c_str();
            m_fp = fopen(file_full_name_c, "a");
        }
    }

    m_mutex.unlock();

    va_list valst;
    va_start(valst, format);

    m_mutex.lock();

    char _c_buffer_ptr[48] = {0};

    //写入的具体时间内容格式
    int n = snprintf(_c_buffer_ptr, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld  ",
                     my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                     my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec);
    _logContent_content._str_time = string(_c_buffer_ptr, 0, n);

    int m = vsnprintf(m_buf, m_log_buf_sizes - 1, format, valst);
    va_end(valst);
    m_buf[m] = '\n';
    m_buf[m + 1] = '\0';
    _logContent_content._str_logContent += string(m_buf, 0, m + 1);

    _logContent_content._str_endColor = "\033[0m";

    m_mutex.unlock();

    if (m_close_log != 3)
    {
        if (m_is_async && !m_log_queue->full())
        {
            m_log_queue->push(_logContent_content);
        }
        else
        {
            m_mutex.lock();
			fputs((_logContent_content._str_time + _logContent_content._str_logContent).c_str(), m_fp);
			m_mutex.unlock();
        }
    }
    if (m_close_log != 2)
    {
        if (m_is_async && !m_log_queue->full())
        {
        }
        else
        {
            m_mutex.lock();
            std::string _str_logWithColor = _logContent_content._str_time + _logContent_content._str_beginColor + _logContent_content._str_logContent + _logContent_content._str_endColor;
   			printf("%s", _str_logWithColor.c_str());
            m_mutex.unlock();
        }
    }

    va_end(valst);
}

string XLog::getFileLineFunctionName(const char *format, ...)
{
    va_list valst;
    va_start(valst, format);
    char _char_pos_ptr[201] = {0};
    int m = vsnprintf(_char_pos_ptr, 200, format, valst);
    va_end(valst);

    string _str_pos = string(_char_pos_ptr, 0, m);

    size_t _size_found = _str_pos.rfind("/");
    if (_size_found != std::string::npos)
    {
        string _str_posTemp_ptr = _str_pos.substr(0, _size_found);
        _str_pos = _str_pos.substr(_size_found, m - _size_found);
        size_t _size_foundTemp = _str_posTemp_ptr.rfind("/");
        if (_size_foundTemp != std::string::npos)
        {
            _str_pos = _str_posTemp_ptr.substr(_size_foundTemp, _str_posTemp_ptr.length() - _size_foundTemp) + _str_pos;
        }
    }

    return _str_pos;
}

void XLog::flush(void)
{
	m_mutex.lock();
	fflush(m_fp);
	m_mutex.unlock();
}