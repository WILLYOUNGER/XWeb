#ifndef X_PTHREAD_POOL_H
#define X_PTHREAD_POOL_H

#include <pthread.h>
#include <list>
#include <map>
#include "../XUtils/XLock.h"
#include "../XLog/XLog.h"
#include <exception>
#include <unistd.h>


template<typename T, typename E>
class XPthreadPool
{
public:
	XPthreadPool(int thread_number = 8, int max_requests = 10000);

	~XPthreadPool();

	bool append(T request);

private:
	static void *worker(void*);

	void run();
private:
	int m_thread_number;
	int m_max_requests;
	pthread_t* m_threads{nullptr};
	std::list<T> m_workqueue;
	std::map<pthread_t, E*> m_worker;
	locker m_mutex;
	sem m_queuestat;
	bool m_stop;
};

template<typename T, typename E>
XPthreadPool<T, E>::XPthreadPool(int thread_number, int max_requests)
:m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false)
{
	if (m_thread_number <= 0 || m_max_requests <= 0)
	{
		throw std::exception();
	}

	m_threads = new pthread_t[m_thread_number];

	if (!m_threads)
	{
		throw std::exception();
	}

	for (int i = 0; i < m_thread_number; i++)
	{
		//XLog::getInstance()->write_log(0, "create the " , to_string(i).c_str(), " thread.");
		if (pthread_create(m_threads + i, NULL, worker, this) != 0)
		{
			delete [] m_threads;
			throw std::exception();
		}

		if (pthread_detach(m_threads[i]))
		{
			delete [] m_threads;
			throw std::exception();
		}

		E *_worker = new E();
		m_worker.insert(std::pair<pthread_t, E*>(*(m_threads + i), _worker));
	}
}
template<typename T, typename E>
XPthreadPool<T, E>::~XPthreadPool()
{
	for (int i = 0; i < m_thread_number; i++)
	{
		delete m_worker[*(m_threads + i)];
		m_worker.erase(*(m_threads + i));
	}
	delete [] m_threads;
	m_stop = true;
}

template<typename T, typename E>
bool XPthreadPool<T, E>::append(T request)
{
	m_mutex.lock();
	if (m_workqueue.size() > m_max_requests)
	{
		m_mutex.unlock();
		return false;
	}
	m_workqueue.push_back(request);
	m_mutex.unlock();
	m_queuestat.post();
	return true;
}
template<typename T, typename E>
void* XPthreadPool<T, E>::worker(void* arg)
{
	XPthreadPool* pool = (XPthreadPool*) arg;
	pool->run();
	return pool;
}
template<typename T, typename E>
void XPthreadPool<T, E>::run()
{
	while (!m_stop)
	{
		m_queuestat.wait();
		m_mutex.lock();
		if  (m_workqueue.empty())
		{
			m_mutex.unlock();
			continue;
		}
		T request = m_workqueue.front();
		m_workqueue.pop_front();
		m_mutex.unlock();
		if (!request)
		{
			continue;
		}
		pthread_t _temp = pthread_self();
		m_worker[_temp]->process(request);
	}
}

#endif