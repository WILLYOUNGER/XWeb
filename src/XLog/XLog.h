#ifndef X_LOG_H
#define X_LOG_H

#include <string>
#include <cstdio>
#include "../XUtils/XLock.h"
#include "../XUtils/XBlockQueue.h"

class XLog
{
public:
	enum XLevel
	{
		DEBUG = 0,
		INFO = 1,
		WARN = 2,
		ERROR = 3,
	};
public:
	static XLog* getInstance()
	{
		static XLog instance;
		return &instance;
	}

	static void *flush_log_thread(void *args)
	{
		return XLog::getInstance()->async_write_log();
	}

	bool init(const char* file_name, int close_log, int level = 0, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

	void write_log(int level, const char *format, ...);

	bool isOpen()
	{
		return m_close_log;
	}

	void flush(void);

	~XLog();
private:
	XLog();

	void *async_write_log()
	{
		std::string single_log;
		while (m_log_queue->pop(single_log))
		{
			m_mutex.lock();
			fputs(single_log.c_str(), m_fp);
			if (m_close_log != 2)
   			{
   				printf("%s\n", single_log.c_str());
			}
			m_mutex.unlock();
		}
	}
private:
	char m_dir_name[128];
	char m_file_name[128];
	int m_split_lines;
	int m_log_buf_sizes;
	long long m_count;
	int m_today;
	FILE* m_fp;
	char* m_buf;
	XBlockQueue<std::string> *m_log_queue;
	bool m_is_async;
	locker m_mutex;
	int m_close_log; 	//0:close 1:file and cmd 2: file 3: cmd
	int m_level;
};

#define XLOG XLog::getInstance()

#define XLOG_DEBUG(format, ...) if(XLOG->isOpen()) {XLOG->write_log(0, format, ##__VA_ARGS__); XLOG->flush();}
#define XLOG_INFO(format, ...) if(XLOG->isOpen()) {XLOG->write_log(1, format, ##__VA_ARGS__); XLOG->flush();}
#define XLOG_WARN(format, ...) if(XLOG->isOpen()) {XLOG->write_log(2, format, ##__VA_ARGS__); XLOG->flush();}
#define XLOG_ERROR(format, ...) if(XLOG->isOpen()) {XLOG->write_log(3, format, ##__VA_ARGS__); XLOG->flush();}


#endif