#ifndef X_LOG_H
#define X_LOG_H

#include <string>
#include <cstdio>
#include "../XUtils/XLock.h"
#include "../XUtils/XBlockQueue.h"

typedef struct XLOGCONTENT
{
	std::string _str_time;
	std::string _str_beginColor;
	std::string _str_logContent;
	std::string _str_endColor;
} XLOGCONTENT;

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

	/**
	 * @brief 初始化日志类
	 * 
	 * @param file_name 文件全称 如："XWebLog.txt"
	 * @param close_log 开启状态：0:close 1:file and cmd 2: file 3: cmd
	 * @param level 	打印大于这个等级的日志 0：debug 1：info 2：warn 3：error
	 * @param log_buf_size 每行日志最大长度
	 * @param split_lines  每个文件最大行数，超出会自动创建新文件
	 * @param max_queue_size 如果大于0为异步日志，输入为队列长度，否则为同步日志
	 * @return true 
	 * @return false 失败，这里可以检查一下./log文件夹有没有创建
	 */
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
		XLOGCONTENT single_log;
		while (m_log_queue->pop(single_log))
		{
			m_mutex.lock();
			fputs((single_log._str_time + single_log._str_logContent).c_str(), m_fp);
			if (m_close_log != 2)
   			{
				std::string _str_logWithColor = single_log._str_time + single_log._str_beginColor + single_log._str_logContent + single_log._str_endColor;
   				printf("%s", _str_logWithColor.c_str());
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
	XBlockQueue<XLOGCONTENT> *m_log_queue;
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