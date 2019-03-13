/*
 * ThreadFactory.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_THREADFACTORY_H_
#define TBOX_THREADFACTORY_H_

// standard
#include <condition_variable>
#include <memory>
#include <mutex>
#include <ostream>
#include <vector>

// local
#include "IRunnable.h"

class ThreadWrapper;

class ThreadFactory final
{
public:
	friend class ThreadWrapper;

	using thread_t = std::shared_ptr<ThreadWrapper>;

	ThreadFactory();
	~ThreadFactory();

	static ThreadFactory& instance();

	thread_t& createThread(IRunnable& runnable);

	void waitOnThreadTerminations();

	void streamThreadInformation(std::ostream& stream) const;

private:

	void onThreadDied(const ThreadWrapper& thread);

	void logActiveThreads() const;

	static std::unique_ptr<ThreadFactory> s_instance;

	mutable std::mutex m_mutex;
	mutable std::condition_variable m_signal;
	std::vector<thread_t> m_threads;
};

#endif /* TBOX_THREADFACTORY_H_ */
